#include <RC_Receiver.h>
#include "ICM20600.h"
#include <Wire.h>

ICM20600 icm20600(true);
#define enA 11
#define in1 12
#define in2 13
int orientation = 1;
void driveMotors(int orientation, long speed) {
  if (speed >= 0 && speed <= 100) {
    int adjustedspeed = map(speed, 0, 100, -255, 255);
    int adjustedorientation = orientation * adjustedspeed;
    if (adjustedorientation > 0) {
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
    } else {
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
    }
    delay(100);
    analogWrite(enA, abs(adjustedspeed));
    Serial.println(adjustedspeed);
  } else {
    analogWrite(enA, 0);
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


RC_Receiver receiver(8);

void setup() {
  Wire.begin();

  Serial.begin(9600);
  icm20600.initialize();
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  
  // Set initial rotation direction
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  
  
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
  delay(200);
  
}
