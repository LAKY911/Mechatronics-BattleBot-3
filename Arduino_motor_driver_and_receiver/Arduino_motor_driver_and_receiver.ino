#include <RC_Receiver.h>

#define enA 11
#define in1 12
#define in2 13

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
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  
  // Set initial rotation direction
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  
  
  //analogWrite(enA, 255);
  Serial.begin(9600);
  receiver.setMinMax(minMax);
  
}

void loop() {
  Serial.println(receiver.getMap(1)*2.5);
  /*for (int i = 50; i <255; i+=20){
    analogWrite(enA, i);
    delay(100);
  }*/
  analogWrite(enA, receiver.getMap(1)*2.5);
  delay(200);
  
}
