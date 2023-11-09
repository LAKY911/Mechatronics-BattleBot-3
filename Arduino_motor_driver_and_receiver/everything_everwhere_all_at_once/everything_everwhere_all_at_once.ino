#include <RC_Receiver.h>
//#include "ICM20600.h"
#include <Wire.h>

//ICM20600 icm20600(true);


// PINS USED
// 0(TX) 1(RX) 2 3 4 5 8 9 11 12 13 A0 A1 A2 (remaining A3 A4 A5)

#define dirL 12 //Left motor
#define pwmL 3
#define brakeL 9
#define currentL A0

#define dirR 13 //Right motor
#define pwmR 11
#define brakeR 8
#define currentR A1

#define weaponPin 5 //Weapon motor

#define VOLTAGE_PIN A2
#define BMS_CUTOUT 4

#define RC_FWD 6 //channel 2 // ver
#define RC_RL_DIR 10 //channel 1 // hor
#define RC_WEAPON 7 //channel 3
#define RC_KILL 2

// pins section end



#define DEADZONE 10
#define DIFF_VER 15
#define DIFF_HOR 10
int disconnected = 0;
int orientation = 1;
int SR = 0, SL = 0;
float turnVariable = 2.55;

int weaponVal = 0, horizontalVal = 50, verticalVal = 50;

int voltage = 0;

unsigned long prevTime;

// Define the number of samples to keep track of. The higher the number, the
// more the readings will be smoothed, but the slower the output will respond to
// the input. Using a constant rather than a normal variable lets us use this
// value to determine the size of the readings array.
const int numReadings = 50;

int readings[numReadings];  // the readings from the analog input
int readIndex = 0;          // the index of the current reading
unsigned long total = 0;              // the running total
int averageVoltage = 0;            // the average

int readCurrentL(){
  int currentReadLeft = analogRead(currentL);
  currentReadLeft = map(currentReadLeft, 0, 675, 0, 20);
  return currentReadLeft;
}

int readCurrentR(){
  int currentReadRight = analogRead(currentR);
  currentReadRight = map(currentReadRight, 0, 675, 0, 20);
  return currentReadRight;
}

void driveMotors(long ver, long hor) {
  if (ver <= -50 && hor <= -50) {  // Disconnected from the transmitter -> NEEDS TO BE DONE/PROCESSED LATER !!!
    //Serial.println("Disconnected from the transmitter");
    disconnected = 1;
    SL = 0;
    SR = 0;
    analogWrite(pwmL, 0);  // write values to H-bridge -> motor
    analogWrite(pwmR, 0);
    analogWrite(weaponPin, 0);
  }
  else if (ver > 50 - DEADZONE && ver < 50 + DEADZONE && hor > 50 - DEADZONE && hor < 50 + DEADZONE) { // deadzone -> turn off
    SL = 0;
    SR = 0;
    analogWrite(pwmL, 0);  // write values to H-bridge -> motor
    analogWrite(pwmR, 0);
  }
  else if (ver > 50 - DIFF_VER && ver < 50 + DIFF_VER && (hor < 50 - DIFF_HOR || hor > 50 + DIFF_HOR) ) { // differential drive (turning) -> wheels turn opposite direction
    if (hor > 50) {
      digitalWrite(dirL, HIGH);
      digitalWrite(dirR, LOW);
    }
    else {
      digitalWrite(dirL, LOW);
      digitalWrite(dirR, HIGH);
    }
  }
  else if (hor >= 0 && hor <= 100) {
    int adjver = map(constrain(ver, 0, 100), 0, 100, -255, 255);
    int adjustedorientation = orientation * adjver;
    int adjhor = map(constrain(hor, 0, 100), 0, 100, -255, 255);
    if (adjver < 0) {
      digitalWrite(dirL, LOW);
      digitalWrite(dirR, LOW);
    }
    else {
      digitalWrite(dirL, HIGH);
      digitalWrite(dirR, HIGH);
    }
  


    
    //TOP RIGHT CORNER
    if (adjhor > DEADZONE && adjver > DEADZONE) {
      SR = adjver - adjhor / turnVariable;
      SL = adjver + adjhor / turnVariable;
      if (SR < 0) {
        SR = 0;
      }
      if (SL > 255) {
        SL = 255;
      }
    }
    //TOP LEFT CORNER
    else if (adjhor < -DEADZONE && adjver > DEADZONE) {
      SR = adjver - adjhor / turnVariable ;
      SL = adjver + adjhor / turnVariable;
      if (SL < 0) {
        SL = 0;
      }
      if (SR > 255) {
        SR = 255;
      }
    }
    //BOTTOM LEFT CORNER
    else if (adjhor < -DEADZONE && adjver < -DEADZONE) {
      SR = adjver + adjhor / turnVariable;
      SL = adjver - adjhor / turnVariable;
      if (SR < -255) {
        SR = -255;
      }
      if (SL > 0) {
        SL = 0;
      }
    }
    //BOTTOM RIGHT CORNER
    else if (adjhor > DEADZONE && adjver < -DEADZONE) {
      SR = adjver + adjhor / turnVariable;
      SL = adjver - adjhor / turnVariable;
      if (SR > 0) {
        SR = 0;
      }
      if (SL < -255) {
        SL = -255;
      }
    }
    else {
      SL = 0;
      SR = 0;
    }


  }else{
    disconnected = 0;
  }
  if (ver > 50 - DIFF_VER && ver < 50 + DIFF_VER && (hor < 50 - DIFF_HOR || hor > 50 + DIFF_HOR)) { // Differential drive area
    //Serial.println(map(hor, 0, 100, -255, 255));
    SL = map(hor, 0, 100, -255, 255);
    SR = SL;
  }
  //Serial.print("Ver: ");
  /*Serial.print(ver);
  Serial.print(";");
  Serial.print(hor);
  Serial.print(";");
  Serial.print(SL);
  Serial.print(";");
  Serial.println(SR);*/

  analogWrite(pwmL, constrain(abs(SL), 0, 255));  // write values to H-bridge -> motor
  analogWrite(pwmR, constrain(abs(SR), 0, 255));
}


int minMax[8][2] =
{
  {1010, 2020},
  {1010, 2020},
  {1010, 2020},
  {1010, 2020},
  {1010, 2020},
  {1010, 2020},
  {1010, 2020},
  {1010, 2020}
};


RC_Receiver receiver(RC_RL_DIR, RC_FWD, RC_WEAPON); //, RC_KILL); // FWD/BWD, Right/Left, Weapon, kill switch

void setup() {
  Wire.begin();

  Serial.begin(9600);
  //Serial.println("Started...");

  pinMode(dirL, OUTPUT);  // Setup the output pins
  pinMode(pwmL, OUTPUT);
  pinMode(brakeL, OUTPUT);

  pinMode(dirR, OUTPUT);
  pinMode(pwmR, OUTPUT);
  pinMode(brakeR, OUTPUT);

  pinMode(weaponPin, OUTPUT);
  pinMode(BMS_CUTOUT, OUTPUT);

  
  digitalWrite(dirL, HIGH);   // Set initial rotation direction
  digitalWrite(dirR, HIGH);

  digitalWrite(brakeL, LOW);
  digitalWrite(brakeR, LOW);

  digitalWrite(BMS_CUTOUT, LOW);
  
  receiver.setMinMax(minMax); // set minimum and maximum of the RC Receiver

  voltage = map(analogRead(VOLTAGE_PIN), 0, 1023, 0, 1390); // read the battery voltage 
  //Serial.println(voltage);

  total = 0;
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = voltage;
    total += readings[thisReading];
  }
  averageVoltage = total / numReadings;/*
  Serial.print("First total: ");
  Serial.print(total);
  Serial.print(", ");
  Serial.println(averageVoltage);
  Serial.print(", ");
  Serial.println(voltage);*/
}


void loop() {
  /*Serial.print("Loop time: ");
  Serial.println(millis() - prevTime);*/

  while (millis() - prevTime < 250) { // This while cycle limits the number of runs - one in every at least 100 ms 
    delay(5);
  }
  prevTime = millis();

  verticalVal = receiver.getMap(1);
  horizontalVal = receiver.getMap(2);
  driveMotors(verticalVal, horizontalVal);  // executing the driveMotors function
  
  weaponVal = receiver.getMap(3);   // driving the weapon motor
  weaponVal = map(weaponVal, 0, 100, 0, 255);
  
  /*Serial.print("Weapon: ");
  Serial.println(weaponVal);*/

  if (weaponVal < 10) {
    analogWrite(weaponPin, 0);
  } else {
    analogWrite(weaponPin, weaponVal);
  }

  
    voltage = map(analogRead(VOLTAGE_PIN), 0, 1023, 0, 1390);

    if (voltage > 1100){
      total = total - readings[readIndex] + voltage;
      readings[readIndex] = voltage;
      readIndex += 1;
  
      if (readIndex >= numReadings) {
        readIndex = 0;
      }/*
      Serial.print(total);
      Serial.print(", ");
      Serial.print(readIndex);
      Serial.print(", ");
      Serial.println(readings[readIndex]);*/
    }
  averageVoltage = total / numReadings; // calculate the average:
  /*
  Serial.print("Average Voltage: ");
  Serial.print(averageVoltage);
  Serial.print(", ");
  Serial.println(voltage);
  */
  if(averageVoltage < 1185){
  digitalWrite(BMS_CUTOUT, LOW);
  //Serial.println("BATTERY CUT");
  //Serial.println("Voltage: ");
  //Serial.println(averageVoltage);
  }
  else{
  digitalWrite(BMS_CUTOUT, LOW);
  }/*
  Serial.print("LEFTCURRENT: ");
  Serial.println(readCurrentL());
  Serial.print("RIGHTCURRENT: ");
  Serial.println(readCurrentR());*/
  // Stuff we want on ESP32
  Serial.print("v");
  Serial.print(averageVoltage);
  Serial.print(";");

  Serial.print("l");
  Serial.print(readCurrentL());
  Serial.print(";");

  Serial.print("r");
  Serial.print(readCurrentR());
  Serial.print(";");

  Serial.print("w");
  Serial.print(constrain(weaponVal, 0, 255));
  Serial.print(";");
  
  Serial.print("d");
  Serial.print(disconnected);
  Serial.print(";");

  Serial.print("\n");
}