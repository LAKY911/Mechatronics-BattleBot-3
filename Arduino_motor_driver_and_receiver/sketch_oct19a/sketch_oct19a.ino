#include <RC_Receiver.h>
#include "ICM20600.h"
#include <Wire.h>

ICM20600 icm20600(true);
#define enA 11 //Left motor
#define enB 5 //Right motor
#define in1 12
#define in2 13
#define in3 9
#define in4 10
int orientation = 1;
long SR = 0;
long SL = 0;
float turnVariable = 2.55; 

void driveMotors(long ver, long hor, int orientation){
  if (hor >= 0 && hor <= 100) {
    int adjver = map(ver, 0, 100, -255, 255);
    int adjustedorientation = orientation * adjver;
    int adjhor = map(hor, 0, 100, 255, -255);
    //Serial.println(adjhor);
    //Serial.println(adjver);
    //delay(200);
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

  if (adjhor > 0 && adjver > 0){
    SR = adjver - adjhor/turnVariable;
    SL = adjver + adjhor/turnVariable;
    if (SR < 0){
      SR = 0;
    }
    if(SL > 255){
      SL = 255;
    }
  }
  //TOP LEFT CORNER
  if (adjhor < 0 && adjver > 0){
  SR = adjver - adjhor/turnVariable ;
  SL = adjver + adjhor/turnVariable;
  if (SL < 0){
    SL = 0;
  }
  if(SR > 255){
    SR = 255;
  }
  }
  //BOTTOM LEFT CORNER
  if (adjhor < 0 && adjver < 0){
  SR = adjver + adjhor/turnVariable;
  SL = adjver - adjhor/turnVariable;
  if (SR < -255){
    SR = -255;
  }
  if(SL > 0){
    SL = 0;
  }
  }
    //BOTTOM RIGHT CORNER
  if (adjhor > 0 && adjver < 0){
  SR = adjver + adjhor/turnVariable;
  SL = adjver - adjhor/turnVariable;
  if (SR > 0){
    SR = 0;
  }
  if(SL < -255){
    SL = -255;
  }

  }
  analogWrite(enA, abs(SL));
  analogWrite(enB, abs(SR));
  }
}



int minMax[8][2] = 
{
  {2020,1010}, 
  {1010,2020}, 
  {1010,2020}, 
  {1010,2020}, 
  {1010,2020}, 
  {1010,2020}, 
  {1010,2020}, 
  {1010,2020}
};


RC_Receiver receiver(8, 6); //speed, turn

void setup() {
  Wire.begin();

  Serial.begin(9600);
  icm20600.initialize();
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  
  // Set initial rotation direction
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  
  
  //analogWrite(enA, 255);
  receiver.setMinMax(minMax);  
}

void loop() {


    if (icm20600.getAccelerationZ() > 0) {
      orientation = 1; 
    }
    else {
      orientation = -1;
    }

  //Serial.println(receiver.getMap(1)*2.5);
  /*for (int i = 50; i <255; i+=20){
    analogWrite(enA, i);
    delay(100);
  }*/

  driveMotors(receiver.getMap(1), receiver.getMap(2), orientation);
  delay(100);
  
}
