/*
  #define RX2 16 // Reciever
  #define TX2 17 // Transmitter
void setup() {
  // put your setup code here, to run once:
  //Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RX2, TX2);
}

void loop() {
    Serial.println("Message Received: ");
    Serial.println(Serial2.readString());
}
*/
#include <HardwareSerial.h>

void setup() {
  Serial2.begin(115200); // Initialize serial communication on UART2 at 115200 baud
  Serial.begin(115200);  // Initialize the main serial communication for debugging
}

void loop() {
  if (Serial2.available()) {
    String receivedMessage = Serial2.readStringUntil('\n'); // Read the incoming string on UART2
    Serial.println("Received: " + receivedMessage); // Print the received string
  }
}