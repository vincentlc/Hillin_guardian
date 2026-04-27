#include <ModbusMaster.h>
#include <SoftwareSerial.h> 

// ── Pines y parámetros ──────────────────────────────────────────────────────
constexpr uint8_t  PIN_DE       = 3;
constexpr uint8_t  PIN_RE       = 2;
constexpr uint8_t  PIN_RX       = 10;
constexpr uint8_t  PIN_TX       = 11;
constexpr uint8_t  SLAVE_ADDR   = 0x14;   // DR-DO1 dirección por defecto
constexpr uint16_t REG_DO       = 0x14;   // registro oxígeno disuelto
constexpr uint16_t REG_TEMP     = 0x11;   // registro temperatura
constexpr uint32_t INTERVAL_MS  = 5000;   // intervalo de muestreo

#define BAUDE_RATE 9600

// ── Estado global ───────────────────────────────────────────────────────────
SoftwareSerial RS485Serial(PIN_RX, PIN_TX);
ModbusMaster node;
float g_doMgL = 0.0f;
float g_tempC = 0.0f;
bool  g_valid = false;

// ── RS485 callbacks ─────────────────────────────────────────────────────────
void preTransmission()  { digitalWrite(PIN_RE, HIGH); digitalWrite(PIN_DE, HIGH); }
void postTransmission() { digitalWrite(PIN_RE, LOW);  digitalWrite(PIN_DE, LOW);  }

// ── Lectura del sensor ──────────────────────────────────────────────────────
bool readDO(float &doOut, float &tempOut) {
  uint8_t res;

  res = node.readHoldingRegisters(REG_DO, 1);
  if (res != ModbusMaster::ku8MBSuccess) return false;
  doOut = node.getResponseBuffer(0) / 100.0f;
  node.clearResponseBuffer();

  res = node.readHoldingRegisters(REG_TEMP, 1);
  if (res != ModbusMaster::ku8MBSuccess) return false;
  tempOut = node.getResponseBuffer(0) / 100.0f;
  node.clearResponseBuffer();

  return true;
}

// ── Salida serial (mismo formato que antes: "v1,v2,v3") ────────────────────
void printData(float do_, float temp, bool ok) {
  if (ok) {
    // Repetimos DO tres veces para mantener compatibilidad con el sistema
    Serial.print(do_, 2);  Serial.print(",");
    Serial.print(do_, 2);  Serial.print(",");
    //Serial.print(do_, 2);  Serial.print(",");
    Serial.println(do_, 2);
    //Serial.println(temp, 2);   // tercer campo = temperatura (antes era otro DO)
  } else {
    Serial.println("Failed,Failed,Failed");
  }
}

// ── Setup ───────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(BAUDE_RATE);

  pinMode(PIN_DE, OUTPUT);
  pinMode(PIN_RE, OUTPUT);
  digitalWrite(PIN_DE, LOW);
  digitalWrite(PIN_RE, LOW);
  
  RS485Serial.begin(BAUDE_RATE);
  node.begin(SLAVE_ADDR, RS485Serial);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  Serial.println("DR-DO1 listo");
}

// ── Loop ────────────────────────────────────────────────────────────────────
void loop() {
  static uint32_t lastMs = 0;

  if (millis() - lastMs >= INTERVAL_MS) {
    lastMs = millis();
    g_valid = readDO(g_doMgL, g_tempC);
    printData(g_doMgL, g_tempC, g_valid);
  }
}
