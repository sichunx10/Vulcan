/*
 * Author: Sichun Xu
 * Date Created: 2020-11-11
 * 
 * License: This is open source, so do whatever you want with it. 
 * You may credit me if you would like, but it is not required
 * 
 * Version 0.1
 * Success: 
 * - Added parser that is able to take 3 serial inputs for motor locations (scalable) in a single line (ex. '10,20,40')
 * - edited the variable initialization for 3 stepper motors, as well as other functions that depend on that.
 * - 
 * 
 * Failed: implement 'intelligent' delay()-based motor control for multiple actuators. The fundamental approach fell apart because 
 * there is a function that is should be able to output the next delay time for a specified motor based on the previous delay time. 
 * However, the control is designed such that loop steps a stepper motor. Because of this, the delay time can get cut by the end of
 * the loop. This messes up the delay function as it needs to take the entire previous delay value to function correctly.
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

const int stepPin2 = 5; //motor 2 (REMEMBER TO CHANGE!)
const int dirPin2 = 4; 
const int hallPin2 = 3;
int accelChange2;
int accelBin2 = 0;
int decelBin2 = 0;
long currentPos2 = 0;
long targetPos2 = 0;

const int stepPin3 = 5; //motor 3 (REMEMBER TO CHANGE! )
const int dirPin3 = 4; 
const int hallPin3 = 3;
int accelChange3;
int accelBin3 = 0;
int decelBin3 = 0;
long currentPos3 = 0;
long targetPos3 = 0;

//motor position data structure
struct motorsAccel {
  int Accel1, Accel2, Accel3, target1, target2, target3;
};

//program variables
int roundedWaitTime;
int hallStatus;
long targetPos;
unsigned long timer1;
unsigned long timer2;
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

//finds smallest number based on 3 numbers
int smallest(int a, int b, int c){
  if (a<b && a<c && a!=0){
    return 1;
  } else if (b<c && b<a && b!=0){
    return 2;
  } else if (c<a && c<b && c!=0){
    return 3;
  } else if (a==b && a<c && a!=0){
    return 4;
  } else if (a==c && a<b && a!=0){
    return 5;
  } else if (b==c && b<a && b!=0){
    return 6;
  } else if (a==b && b==c && b!=0){
    return 7;
  } else {
    return 0;
  }
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
  }

  int currentmotor = smallest(roundedWaitTime1, roundedWaitTime2, roundedWaitTime3); //FUTURE: FIX SO THAT WHEN MOTORS HAVE SAME VALUE, IT WOULDN'T BREAK

  //switch case based on which motor is next to be stepped
  if (currentmotor == 1){ //motor 1 active
    //find new waittimes for next loop
    waitTime1 = nextTime(&accelBin1, &decelBin1, waitTime1);
    waitTime2 = waitTime2 - waitTime1;
    waitTime3 = waitTime3 - waitTime1;
    roundedWaitTime1 = round(waitTime1*1000000);
    roundedWaitTime2 = round(waitTime2*1000000);
    roundedWaitTime3 = round(waitTime3*1000000);

    delayMicroseconds(roundedWaitTime1);
    digitalWrite(stepPin1, HIGH);
    digitalWrite(stepPin1, LOW);
    Serial.print("Case 1: ");
    Serial.print("a");
    Serial.println(roundedWaitTime1);
    Serial.print("b");
    Serial.println(roundedWaitTime2);
    Serial.print("c");
    Serial.println(roundedWaitTime3);
  }else if (currentmotor == 2){ //motor 2 active
    //find new waittimes for next loop
    waitTime1 = waitTime1 - waitTime2;
    waitTime2 = nextTime(&accelBin2, &decelBin2, waitTime2);
    waitTime3 = waitTime3 - waitTime2;
    roundedWaitTime1 = round(waitTime1*1000000);
    roundedWaitTime2 = round(waitTime2*1000000);
    roundedWaitTime3 = round(waitTime3*1000000);
    
    delayMicroseconds(roundedWaitTime2);
    digitalWrite(stepPin2, HIGH);
    digitalWrite(stepPin2, LOW);
    Serial.print("Case 2: ");
    Serial.print("a");
    Serial.println(roundedWaitTime1);
    Serial.print("b");
    Serial.println(roundedWaitTime2);
    Serial.print("c");
    Serial.println(roundedWaitTime3);
  }else if (currentmotor == 3){ // motor 3 active
    //find new waittimes for next loop
    waitTime1 = waitTime1 - waitTime3;
    waitTime2 = waitTime2 - waitTime3;
    waitTime3 = nextTime(&accelBin3, &decelBin3, waitTime3);
    roundedWaitTime1 = round(waitTime1*1000000);
    roundedWaitTime2 = round(waitTime2*1000000);
    roundedWaitTime3 = round(waitTime3*1000000);

    delayMicroseconds(roundedWaitTime3);
    digitalWrite(stepPin3, HIGH);
    digitalWrite(stepPin3, LOW);
    Serial.println("Case 3: ");
    Serial.print("a");
    Serial.println(roundedWaitTime1);
    Serial.print("b");
    Serial.println(roundedWaitTime2);
    Serial.print("c");
    Serial.println(roundedWaitTime3);
  }else if (currentmotor == 4){ // motor 1+2 active
    waitTime1 = nextTime(&accelBin1, &decelBin1, waitTime1);
    waitTime2 = nextTime(&accelBin2, &decelBin2, waitTime2);
    waitTime3 = waitTime3 - waitTime1;
    roundedWaitTime1 = round(waitTime1*1000000);
    roundedWaitTime2 = round(waitTime2*1000000);
    roundedWaitTime3 = round(waitTime3*1000000);

    delayMicroseconds(roundedWaitTime1);
    digitalWrite(stepPin1, HIGH);
    digitalWrite(stepPin2, HIGH);
    digitalWrite(stepPin1, LOW);
    digitalWrite(stepPin2, LOW);
    Serial.println("Case 4: ");
    Serial.print("a");
    Serial.println(roundedWaitTime1);
    Serial.print("b");
    Serial.println(roundedWaitTime2);
    Serial.print("c");
    Serial.println(roundedWaitTime3);
  }else if (currentmotor == 5){ // motor 1+3 active
    waitTime1 = nextTime(&accelBin1, &decelBin1, waitTime1);
    waitTime2 = waitTime2 - waitTime1;
    waitTime3 = nextTime(&accelBin3, &decelBin3, waitTime3);
    roundedWaitTime1 = round(waitTime1*1000000);
    roundedWaitTime2 = round(waitTime2*1000000);
    roundedWaitTime3 = round(waitTime3*1000000);

    delayMicroseconds(roundedWaitTime1);
    digitalWrite(stepPin1, HIGH);
    digitalWrite(stepPin3, HIGH);
    digitalWrite(stepPin1, LOW);
    digitalWrite(stepPin3, LOW);
    Serial.println("Case 6: ");
    Serial.print("a");
    Serial.println(roundedWaitTime1);
    Serial.print("b");
    Serial.println(roundedWaitTime2);
    Serial.print("c");
    Serial.println(roundedWaitTime3);
  }else if (currentmotor == 6){ // motor 2+3 active
    waitTime1 = waitTime1 - waitTime2;
    waitTime2 = nextTime(&accelBin2, &decelBin2, waitTime2);
    waitTime3 = nextTime(&accelBin3, &decelBin3, waitTime3);
    roundedWaitTime1 = round(waitTime1*1000000);
    roundedWaitTime2 = round(waitTime2*1000000);
    roundedWaitTime3 = round(waitTime3*1000000);

    delayMicroseconds(roundedWaitTime2);
    digitalWrite(stepPin2, HIGH);
    digitalWrite(stepPin3, HIGH);
    digitalWrite(stepPin2, LOW);
    digitalWrite(stepPin3, LOW);
    Serial.println("Case 5: ");
    Serial.print("a");
    Serial.println(roundedWaitTime1);
    Serial.print("b");
    Serial.println(roundedWaitTime2);
    Serial.print("c");
    Serial.println(roundedWaitTime3);
  }else if (currentmotor == 7){ // motor 1+2+3 active
    waitTime1 = nextTime(&accelBin1, &decelBin1, waitTime1);
    waitTime2 = nextTime(&accelBin2, &decelBin2, waitTime2);
    waitTime3 = nextTime(&accelBin3, &decelBin3, waitTime3);
    roundedWaitTime1 = round(waitTime1*1000000);
    roundedWaitTime2 = round(waitTime2*1000000);
    roundedWaitTime3 = round(waitTime3*1000000);

    delayMicroseconds(roundedWaitTime1);
    digitalWrite(stepPin1, HIGH);
    digitalWrite(stepPin1, HIGH);
    digitalWrite(stepPin3, HIGH);
    digitalWrite(stepPin1, LOW);
    digitalWrite(stepPin2, LOW);
    digitalWrite(stepPin3, LOW);
    Serial.println("Case 7: ");
    Serial.print("a");
    Serial.println(roundedWaitTime1);
    Serial.print("b");
    Serial.println(roundedWaitTime2);
    Serial.print("c");
    Serial.println(roundedWaitTime3);
  }
}