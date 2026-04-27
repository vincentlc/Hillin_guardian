#include <ModbusMaster.h>
#include <HardwareSerial.h> // Para ESP32, usamos HardwareSerial

// Note: this file was received as it is from previous developer, but was never test in the last huillin
// so might be needed to do some adjustment to make it work

// Definir el pin de dirección y el pin de habilitación para el MAX485
#define MAX485_DE      4
#define MAX485_RE_NEG  5

union 
{
  unsigned int ints[2];
  float toFloat;
} foo;

// Crear una instancia de la clase ModbusMaster
ModbusMaster node;
//datos oxigeno
String D1ox;
String D2ox;
String D3ox;
String Dox;

String Dato;
float num;

// Función callback que se ejecuta antes de la transmisión
void preTransmission() {
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
}

// Función callback que se ejecuta después de la transmisión
void postTransmission() {
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
}

void setup() {
  // Inicializar el puerto serial para la comunicación con el ESP32
  Serial.begin(115200); // Velocidad de baudios puede variar según tu configuración

  // Inicializar el puerto serial para la comunicación con el módulo RS485
  HardwareSerial &RS485Serial = Serial; // Cambia Serial2 a Serial1, Serial3, etc. según el pin que estés utilizando
  RS485Serial.begin(9600, SERIAL_8N1); // Configuración del puerto serial: 9600 baudios, 8 bits de datos, sin paridad, 1 bit de parada

  // Inicializar los pines de dirección y habilitación del MAX485
  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  
  // Establecer los pines de dirección y habilitación en bajo para la comunicación RS485
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
   uint8_t result;
  uint16_t data[6];
  // Inicializar la instancia de ModbusMaster
  node.begin(0x14, RS485Serial); // 20 El primer parámetro es la dirección del dispositivo esclavo

  // Asignar las funciones de callback
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  //node.writeSingleRegister(1, 8);

  //delay(1000);
  //result = node.readHoldingRegisters(83,6);
  // if (result == node.ku8MBSuccess)
  // {
  //   foo.ints[1]= node.getResponseBuffer(0x00);
  //   foo.ints[0]= node.getResponseBuffer(0x01);
  //   foo.ints[1]= node.getResponseBuffer(0x02);
  //   foo.ints[0]= node.getResponseBuffer(0x03);
  //   foo.ints[1]= node.getResponseBuffer(0x04);
  //   foo.ints[0]= node.getResponseBuffer(0x05);
  //   //Serial.println();     
  // }

}

void loop() {
 
  uint8_t result;
  uint16_t data[6];
  Serial.println("Activado");
  //delay(300000);
  //result = node.readHoldingRegisters(83,6);
  result = node.readHoldingRegisters(0x14,1);
  if (result == node.ku8MBSuccess)
  {
    uint16_t rawDO = node.getResponseBuffer(0x00);
    float doValue = rawDO / 100.0;
    String s = String(doValue, 2);

    // Mantener formato original: D1ox,D2ox,D3ox
    D1ox = s + ",";
    D2ox = s + ",";
    D3ox = s;          // o aquí podrías poner la temperatura si la lees
    Dox = D1ox + D2ox + D3ox;

    delay(1000);
    Dato = Dox;
    Serial.println(Dato);
  }
  else
  {
    Dox = "Failed,Failed,Failed";  // mantener igual
  }
  delay(1000);
  Dato = Dox;
  Serial.println(Dato);
  node.clearResponseBuffer();
 
}
