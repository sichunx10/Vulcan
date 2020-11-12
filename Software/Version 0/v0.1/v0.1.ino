/*
 * Author: Sichun Xu
 * Date Created: 2020-11-11
 * 
 * License: This is open source, so do whatever you want with it. 
 * You may credit me if you would like, but it is not required
 * 
 * Version 0.1
 * This is the first real iternation of the Vulcan code for a single 
 * stepper actuator. It first calibrates the start position with a 
 * hall effect sensor and impliments a smooth acceleration function
 */

// defines pins numbers
const int buttonPin = 12;
const int stepPin = 5; 
const int dirPin = 4; 
const int ms1Pin = 10;
const int ms2Pin = 9;
const int ms3Pin = 8;
const int ledPin = 13;
const int hallPin = 3;
const int toMax = 1984;
float waitTime = 0.003;
int buttonStatus;
int roundedWaitTime;
int hallStatus;
int accelBin = 0;
int decelBin = 0;
int stepperDirection = 0;
long currentPos = 0;
long targetPos;
unsigned long timer1;
unsigned long timer2;
unsigned long difference;
 
void setup() {
  //setup the pins
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
  pinMode(ms1Pin,OUTPUT);
  pinMode(ms2Pin,OUTPUT);
  pinMode(ms3Pin,OUTPUT);
  pinMode(ledPin,OUTPUT);  
  pinMode(hallPin,INPUT);

  //change the microstepping
  digitalWrite(ms1Pin,LOW);
  digitalWrite(ms2Pin,HIGH);
  digitalWrite(ms3Pin,LOW);

  Serial.begin(115200);
//  startHoming();
}

//Homing function
void startHoming(){
  int courseDelay = 400;
  int fineDelay = 2000;
  
  //start course homing
  digitalWrite(dirPin,LOW);
  hallStatus = digitalRead(hallPin);
  while(hallStatus == HIGH){
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(courseDelay);
    digitalWrite(stepPin,LOW);
    hallStatus = digitalRead(hallPin);
  }
  Serial.println("COURSE HOMING COMPLETE");

  //rewind the actuator a bit to prepare for fine homing
  digitalWrite(dirPin,HIGH);
  for(int i = 0; i<500; i++){
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(courseDelay);
    digitalWrite(stepPin,LOW);
  }
  Serial.println("REWIND COMPLETE");

  //start fine homing
  digitalWrite(dirPin,LOW);
  hallStatus = digitalRead(hallPin);
  while(hallStatus == HIGH){
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(fineDelay);
    digitalWrite(stepPin,LOW);
    hallStatus = digitalRead(hallPin);
  }
  Serial.println("FINE HOMING COMPLETE");
}

//based on a target position, this creates a plan of the motion to reach target
int motionPlanning(int currentPos, int targetPos){

  //find and return the step where acceleration changes
  int totalSteps = abs(targetPos-currentPos);
  int accelChange;
  if(totalSteps > 2*toMax){
    accelChange = totalSteps - toMax;
  }else{
    accelChange = round(totalSteps/2);
  }
  return accelChange;
}
  
//function to return the next delay value for positive acceleration
float positiveAcceleration(float waitTime) {
    float dVelocity = waitTime * 4000;
    waitTime = 1/(dVelocity+1/waitTime);
    if (waitTime < 0.00025){
      waitTime = 0.00025;
    }
    return waitTime;
}

//function to return the next delay value for negative acceleration
float negativeAcceleration(float waitTime) {
    float dVelocity = waitTime * -4000;
    waitTime = 1/(dVelocity+1/waitTime);
    if (waitTime > 0.003){
      waitTime = 0.003;
    }
    return waitTime;
}


void loop() {
  //check if anything is sent to serial monitor
  if(Serial.available() > 0){
    int incoming = Serial.parseInt();
    if (incoming != 0){
      targetPos = incoming;
      accelBin = motionPlanning(currentPos, targetPos);
      int totalSteps = abs(targetPos-currentPos);
      decelBin = totalSteps - accelBin;
//      Serial.println(accelBin);
//      Serial.println(decelBin);
      
      //set the direction pin based on the target position
      if(targetPos > currentPos){
        digitalWrite(dirPin, HIGH);
        stepperDirection = 1;
      }else{
        digitalWrite(dirPin, LOW);
        stepperDirection = -1;
      }
    }
  }
  //accelerate if the acceleration "bin" is not empty
  if(accelBin != 0){
    waitTime = positiveAcceleration(waitTime);
    roundedWaitTime = round(waitTime*1000000);
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(roundedWaitTime);
    digitalWrite(stepPin, LOW);
//    Serial.println(currentPos);
    if(stepperDirection == 1){
      currentPos++;
    }else{
      currentPos--;
    }
    accelBin--;
  }else if(decelBin != 0){
    waitTime = negativeAcceleration(waitTime);
    roundedWaitTime = round(waitTime*1000000);
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(roundedWaitTime);
    digitalWrite(stepPin, LOW);
//    Serial.println(currentPos);
    if(stepperDirection == 1){
      currentPos++;
    }else{
      currentPos--;
    }
    decelBin--;
  }
}
