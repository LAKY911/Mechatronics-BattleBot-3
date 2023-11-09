#define RXD2 16
#define TXD2 17
String receivedString = "";
String voltage = "";
String weapon = "";
String leftcurrent = "";
String rightcurrent = "";
String disconnected = "";

void setup()
{
  // Initialize serial and wait for port to open:
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(100);
  // Defined in thingProperties.h
  //initProperties();
  // Connect to Arduino IoT Cloud
  //ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
 */
  //setDebugMessageLevel(2);
  //ArduinoCloud.printDebugInfo();
}
void loop()
{
  
  if (Serial2.available()) {
    char incomingChar = Serial2.read();
    if (incomingChar == 'v'){
      while (incomingChar != ";"){
        voltage += Serial2.read();
      }
      receivedString += voltage ;
    }
    else if (incomingChar == 'l'){
      while (incomingChar != ";"){
        leftcurrent += Serial2.read();
      }
      receivedString += leftcurrent;
    }
    else if(incomingChar == 'r'){
      while (incomingChar != ";"){
        rightcurrent += Serial2.read();
      }
      receivedString += rightcurrent;
    }
    else if(incomingChar == 'w'){
      while (incomingChar != ";"){
        weapon += Serial2.read();
      }
      receivedString += weapon;
    }
    else if(incomingChar == 'd'){
      while (incomingChar != ";"){
        disconnected += Serial2.read();
      }
      receivedString += disconnected;
    }
    else if (incomingChar == '\n') {
      // End of the string, print it and clear the buffer
      Serial.println(receivedString);
      receivedString = "";
    } 
  }
}