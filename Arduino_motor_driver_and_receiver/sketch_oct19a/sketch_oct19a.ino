#include <RC_Receiver.h>
#include "ICM20600.h"
#include <Wire.h>
 
//ICM20600 icm20600(true);
#define enA 11 //Left motor
#define enB 5 //Right motor
#define enC 3 //Weapon motor
#define in1 12
#define in2 13
#define in3 9
#define in4 10
 
#define DEADZONE 5
#define DIFF_VER 15
#define DIFF_HOR 15
 
int orientation = 1;
int SR = 0, SL = 0;
float turnVariable = 2.55;
 
int weaponVal = 0, horizontalVal = 50, verticalVal = 50;
 
void driveMotors(long ver, long hor) {
  Serial.print(ver);
  Serial.print(", ");
  Serial.println(hor);
  if (ver == 200 || hor == -100) {  // Disconnected from the transmitter -> NEEDS TO BE DONE/PROCESSED LATER !!!
    Serial.println("Disconnected from the transmitter");
  }
  if (ver > 50-DEADZONE && ver < 50+DEADZONE && hor > 50-DEADZONE && hor < 50+DEADZONE) { // deadzone -> turn off
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, LOW);
    }
  else if (ver > 50-DIFF_VER && ver < 50+DIFF_VER && (hor < 50-DIFF_HOR || hor > 50+DIFF_HOR) ) { // differential drive (turning) -> wheels turn opposite direction
    Serial.println("diff");
    if (hor > 50){
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
    }
    else {
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
    }
  }
  else if (hor >= 0 && hor <= 100) {
    int adjver = map(ver, 0, 100, -255, 255);
    int adjustedorientation = orientation * adjver;
    int adjhor = map(hor, 0, 100, -255, 255);
    if (adjver > 0) {
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
    }
    else {
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
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
 
    
  }
  if (ver > 50-DIFF_VER && ver < 50+DIFF_VER && (hor < 50-DIFF_HOR || hor > 50+DIFF_HOR)) { // Differential drive area
      Serial.println(map(hor, 0, 100, -255, 255));
      SL = map(hor, 0, 100, -255, 255);
      SR = SL;
    }
  analogWrite(enA, abs(SL));  // write values to H-bridge -> motor
  analogWrite(enB, abs(SR));
}
 
 
int minMax[8][2] =
{
  {2020, 1010},
  {1010, 2020},
  {1010, 2020},
  {1010, 2020},
  {1010, 2020},
  {1010, 2020},
  {1010, 2020},
  {1010, 2020}
};
 
 
RC_Receiver receiver(8, 6, 7); // FWD/BWD, Right/Left, Weapon
 
void setup() {
  Wire.begin();
 
  Serial.begin(9600);
  Serial.println("Started...");
  //icm20600.initialize();
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(enC, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
 
 
  // Set initial rotation direction
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
 
 
  receiver.setMinMax(minMax);
}
 
void loop() {
 
  /*if (icm20600.getAccelerationZ() > 0) {
    orientation = 1;
    }
    else {
    orientation = -1;
    }
 
    //Serial.println(receiver.getMap(1)*2.5);
    // /*for (int i = 50; i <255; i+=20){
    analogWrite(enA, i);
    delay(100);
    }*/

  verticalVal = receiver.getMap(1);
  horizontalVal = receiver.getMap(2);
  driveMotors(verticalVal, horizontalVal);
  weaponVal = receiver.getMap(3);
  weaponVal = map(weaponVal, 0, 100, 0, 255);
  if (weaponVal == -2) {
      analogWrite(enC, 0);
  } else{
    analogWrite(enC, weaponVal);
  }

  //Serial.println(weaponVal);
  delay(100);
}