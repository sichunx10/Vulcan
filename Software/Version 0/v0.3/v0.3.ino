/*
 * Author: Sichun Xu
 * Date Created: 2020-11-11
 * 
 * License: This is open source, so do whatever you want with it. 
 * You may credit me if you would like, but it is not required
 * 
 * Version 0.3
 * Plan is to move away from a delay system entirely and move to a micros() archetecture.
 */

// constants
const int ledPin = 13;
const int toMax = 1984;
float waitTime1 = 0.003;
float waitTime2 = 0.003;
float waitTime3 = 0.003;
int roundedWaitTime1 = 0;
int roundedWaitTime2 = 0;
int roundedWaitTime3 = 0;
int waitTime;

//stepper driver microstepping pins
const int ms1Pin = 10;
const int ms2Pin = 9;
const int ms3Pin = 8;

//actuator pins and variables
const int stepPin1 = 5; //motor 1 
const int dirPin1 = 4; 
const int hallPin1 = 3;
int accelChange1;
int accelBin1 = 0;
int decelBin1 = 0;
long currentPos1 = 0;
long targetPos1 = 0;
unsigned long changeTime1 = 0;

const int stepPin2 = 5; //motor 2 (REMEMBER TO CHANGE!)
const int dirPin2 = 4; 
const int hallPin2 = 3;
int accelChange2;
int accelBin2 = 0;
int decelBin2 = 0;
long currentPos2 = 0;
long targetPos2 = 0;
unsigned long changeTime2 = 0;

const int stepPin3 = 5; //motor 3 (REMEMBER TO CHANGE! )
const int dirPin3 = 4; 
const int hallPin3 = 3;
int accelChange3;
int accelBin3 = 0;
int decelBin3 = 0;
long currentPos3 = 0;
long targetPos3 = 0;
unsigned long changeTime3 = 0;

//motor position data structure
struct motorsAccel {
  int Accel1, Accel2, Accel3, target1, target2, target3;
};

//program variables
int roundedWaitTime;
int hallStatus;
long targetPos;
unsigned long currentTime;
unsigned long difference;
 
void setup() {
  //change the microstepping
  pinMode(ms1Pin,OUTPUT);
  pinMode(ms2Pin,OUTPUT);
  pinMode(ms3Pin,OUTPUT);
  digitalWrite(ms1Pin,LOW);
  digitalWrite(ms2Pin,HIGH);
  digitalWrite(ms3Pin,LOW);

  //setup the actuator pins
  pinMode(stepPin1,OUTPUT); //motor 1
  pinMode(dirPin1,OUTPUT);
  pinMode(hallPin1,INPUT);

  pinMode(stepPin2,OUTPUT); //motor 2
  pinMode(dirPin2,OUTPUT);
  pinMode(hallPin2,INPUT);

  pinMode(stepPin3,OUTPUT); //motor 3
  pinMode(dirPin3,OUTPUT);
  pinMode(hallPin3,INPUT);

  //onboard LED for troubleshooting
  pinMode(ledPin,OUTPUT); 

  Serial.begin(115200);
  // startHoming(stepPin1, dirPin1, hallPin1);
  // startHoming(stepPin2, dirPin2, hallPin2);
  // startHoming(stepPin3, dirPin3, hallPin3);
}

//Homing function
void startHoming(int stepPin, int dirPin, int hallPin){
  int courseDelay = 400;
  int fineDelay = 2000;

  //reset hallStatus variable
  hallStatus = HIGH;

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
    digitalWrite(stepPin1,HIGH);
    delayMicroseconds(fineDelay);
    digitalWrite(stepPin1,LOW);
    hallStatus = digitalRead(hallPin);
  }
  Serial.println("FINE HOMING COMPLETE");
}

//based on a target position, this creates a plan of the motion to reach target
int motionPlanning(int currentPos, int targetPos, int dirPin){

  //find and return the step where acceleration changes
  int totalSteps = abs(targetPos-currentPos);
  int accelChange;
  if(totalSteps > 2*toMax){
    accelChange = totalSteps - toMax;
  }else{
    accelChange = round(totalSteps/2);
  }

  //set the direction pin based on the direction of the target
  if(targetPos > currentPos){
    digitalWrite(dirPin, HIGH);
  }else{
    digitalWrite(dirPin, LOW);
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

//function that returns the next delay needed for any motor
float nextTime(int *accelBin, int *decelBin, float waitTime){
  if(*accelBin !=0){
    waitTime = positiveAcceleration(waitTime);
    (*accelBin)--;
  } else if(*decelBin !=0){
    waitTime = negativeAcceleration(waitTime);
    (*decelBin)--;
  } else {
    waitTime = 0;
  }

  return waitTime;
}


//
motorsAccel motorParser() {
  //Motor target parser
  while (Serial.available() > 0){
    targetPos1 = Serial.parseInt();
    targetPos2 = Serial.parseInt();
    targetPos3 = Serial.parseInt(); 
    
    //newline indicates finished
    if (Serial.read() == '\n'){
      //constrain target so that motors don't overextend (CHANGE TO CORRECT ENDPOINTS!)
      targetPos1 = constrain(targetPos1, 0, 1000);
      targetPos2 = constrain(targetPos2, 0, 1000);
      targetPos3 = constrain(targetPos3, 0, 1000);

      //run motion planning
      accelChange1 = motionPlanning(currentPos1, targetPos1, dirPin1);
      accelChange2 = motionPlanning(currentPos2, targetPos2, dirPin2);
      accelChange3 = motionPlanning(currentPos3, targetPos3, dirPin3);

      motorsAccel Vulcan = {accelChange1, accelChange2, accelChange3, targetPos1, targetPos2, targetPos3};
      return Vulcan;
    }
  }
}

void loop() {
  //run parser
  if (Serial.available() > 0){
    motorsAccel Vulcan = motorParser();

    //get accelBin and decelBin
    accelBin1 = Vulcan.Accel1;
    accelBin2 = Vulcan.Accel2;
    accelBin3 = Vulcan.Accel3;
    decelBin1 = abs(Vulcan.target1 - currentPos1) - accelBin1;
    decelBin2 = abs(Vulcan.target2 - currentPos2) - accelBin2;
    decelBin3 = abs(Vulcan.target3 - currentPos3) - accelBin3;

    waitTime1 = nextTime(&accelBin1, &decelBin1, waitTime1);
    waitTime2 = nextTime(&accelBin2, &decelBin2, waitTime2);
    waitTime3 = nextTime(&accelBin3, &decelBin3, waitTime3);

    roundedWaitTime1 = round(waitTime1*1000000);
    roundedWaitTime2 = round(waitTime2*1000000);
    roundedWaitTime3 = round(waitTime3*1000000);

    changeTime2 = micros() + roundedWaitTime2;
    changeTime1 = micros() + roundedWaitTime1;
    changeTime3 = micros() + roundedWaitTime3;
  }

  currentTime = micros();
  
  if (currentTime > changeTime1){
    digitalWrite(stepPin1, HIGH);
    digitalWrite(stepPin1, LOW);
    if (accelBin1 != 0 || decelBin1 != 0){
      waitTime1 = nextTime(&accelBin1, &decelBin1, waitTime1);
      roundedWaitTime1 = round(waitTime1*1000000);
      changeTime1 = currentTime + roundedWaitTime1;
    }
  }

  if (currentTime > changeTime2){
    digitalWrite(stepPin2, HIGH);
    digitalWrite(stepPin2, LOW);
    if (accelBin2 != 0 || decelBin2 != 0){
      waitTime2 = nextTime(&accelBin2, &decelBin2, waitTime2);
      roundedWaitTime2 = round(waitTime2*1000000);
      changeTime2 = currentTime + roundedWaitTime2;
    }
  }

  if (currentTime > changeTime3){
    digitalWrite(stepPin3, HIGH);
    digitalWrite(stepPin3, LOW);
    if (accelBin3 != 0 || decelBin3 != 0){
      waitTime3 = nextTime(&accelBin3, &decelBin3, waitTime3);
      roundedWaitTime3 = round(waitTime3*1000000);
      changeTime3 = currentTime + roundedWaitTime3;
    }
  }
}