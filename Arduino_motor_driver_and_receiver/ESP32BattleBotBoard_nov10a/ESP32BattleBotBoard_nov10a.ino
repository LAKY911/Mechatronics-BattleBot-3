#include "arduino_secrets.h"
/*
  Sketch generated by the Arduino IoT Cloud Thing "Untitled"
  https://create.arduino.cc/cloud/things/43e4a1d8-0300-499e-a437-8d64de8a4a03

  Arduino IoT Cloud Variables description

  The following variables are automatically generated and updated when changes are made to the Thing

  float batteryVoltage;
  float leftCurrent;
  float rightCurrent;
  int weaponSpeed;
  bool disconnectedBool;

  Variables which are marked as READ/WRITE in the Cloud Thing will also have functions
  which are called when their values are changed from the Dashboard.
  These functions are generated with the Thing and added at the end of this sketch.
*/

#include "thingProperties.h"

#define RXD2 16
#define TXD2 17
String receivedString = "";
String voltage = "";
String weapon = "";
String leftcurrent = "";
String rightcurrent = "";
String disconnected = "";

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
  */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update();
  // Your code here
  while (Serial2.available() > 0) {
    char incomingChar = Serial2.read();
    Serial.print(incomingChar);
    if (incomingChar == 'v') {
      voltage = "";
      while (incomingChar != ';') {
        incomingChar = Serial2.read();
        voltage += incomingChar;
      }
      batteryVoltage = voltage.toFloat() / 100.0;
      //receivedString += voltage ;
    }
    else if (incomingChar == 'l') {
      leftcurrent = "";
      while (incomingChar != ';') {
        incomingChar = Serial2.read();
        leftcurrent += incomingChar;
      }
      leftCurrent = leftcurrent.toFloat()/10.0;
      //receivedString += leftcurrent;
    }
    else if (incomingChar == 'r') {
      rightcurrent = "";
      while (incomingChar != ';') {
        incomingChar = Serial2.read();
        rightcurrent += incomingChar;
      }
      rightCurrent = rightcurrent.toFloat()/10.0;
      //receivedString += rightcurrent;
    }
    else if (incomingChar == 'w') {
      weapon = "";
      while (incomingChar != ';') {
        incomingChar = Serial2.read();
        weapon += incomingChar;
      }
      weaponSpeed = map(weapon.toInt(), 0, 255, 0, 100);
      //receivedString += weapon;
    }
    else if (incomingChar == 'd') {
      disconnected = "";
      while (incomingChar != ';') {
        incomingChar = Serial2.read();
        disconnected += incomingChar;
      }
      disconnectedBool = disconnected.toInt();
      if (disconnectedBool == 1) {
        disconnectedBool = true;
      }
      else {
        disconnectedBool = false;
      }
      //receivedString += disconnected;
    }
    else if (incomingChar == '\n') {
      // End of the string, print it and clear the buffer
      Serial.println(receivedString);
      receivedString = "";
      voltage = "";
      disconnected = "";
      rightcurrent = "";
      leftcurrent = "";
      weapon = "";
    }
  }
  Serial.println("Transmitting data: ");
  Serial.print(batteryVoltage);
  Serial.print(", ");
  Serial.print(weaponSpeed);
  Serial.print(", ");
  Serial.print(leftCurrent);
  Serial.print(", ");
  Serial.print(rightCurrent);
  Serial.print(", ");
  Serial.println(disconnectedBool);

  Serial.print(voltage);
  Serial.print(", ");
  Serial.print(weapon);
  Serial.print(", ");
  Serial.print(leftcurrent);
  Serial.print(", ");
  Serial.print(rightcurrent);
  Serial.print(", ");
  Serial.println(disconnected);
}






