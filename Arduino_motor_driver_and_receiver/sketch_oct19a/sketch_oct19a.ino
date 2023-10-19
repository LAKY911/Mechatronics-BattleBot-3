#include <RC_Receiver.h>
#include "ICM20600.h"
#include <Wire.h>

ICM20600 icm20600(true);
#define enA 11
#define enB 7
#define in1 12
#define in2 13
#define in3 9
#define in4 10
int orientation = 1;
void driveMotors(int orientation, long speed) {
  if (speed >= 0 && speed <= 100) {
    int adjustedspeed = map(speed, 0, 100, -255, 255);
    int adjustedorientation = orientation * adjustedspeed;
    if (adjustedorientation > 0) {
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        digitalWrite(in3, LOW);
        digitalWrite(in4, HIGH);
    } else {
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        digitalWrite(in3, HIGH);
        digitalWrite(in4, LOW);
    }
    delay(100);
    analogWrite(enA, abs(adjustedspeed));
    Serial.println(adjustedspeed);
  } else {
    analogWrite(enA, 0);
  }

}

void turn(long speed, long turn){
  if (speed >= 0 && speed <= 100) {
    int adjustedspeed = map(speed, 0, 100, -255, 255);
    int adjustedorientation = orientation * adjustedspeed;
    int adjustedturn = map(turn, 0, 100, -255, 255);
    if (adjustedorientation > 0) {
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        digitalWrite(in3, LOW);
        digitalWrite(in4, HIGH);
    } else {
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        digitalWrite(in3, HIGH);
        digitalWrite(in4, LOW);
    }
    delay(100);
    analogWrite(enB, abs(adjustedturn));
    Serial.println(adjustedturn);
  } else {
    analogWrite(enB, 0);
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
  driveMotors(orientation, receiver.getMap(1));
  turn(receiver.getMap(1), receiver.getMap(2));
  delay(200);
  
}
