// defines pins numbers
const int stepPin = 5; 
const int dirPin = 4; 
const int ms1Pin = 10;
const int ms2Pin = 9;
const int ms3Pin = 8;
const int ledPin = 13;
float waitTime = 0.003;
int accelerationTerm = 4000;
int roundedWaitTime;
unsigned long timer1;
unsigned long timer2;
unsigned long difference;
 
void setup() {
  //setup the pins
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
  pinMode(ms1Pin,OUTPUT);
  pinMode(ms2Pin,OUTPUT);
  pinMode(ms3Pin,OUTPUT);
  pinMode(ledPin, OUTPUT);  

  //change the microstepping
  digitalWrite(ms1Pin,LOW);
  digitalWrite(ms2Pin,HIGH);
  digitalWrite(ms3Pin,LOW);
}

//function to return the next delay value for positive acceleration
float positiveAcceleration(float waitTime) {
    float dVelocity = waitTime * accelerationTerm;
    waitTime = 1/(dVelocity+1/waitTime);
    if (waitTime < 0.00025){
      waitTime = 0.00025;
    }
    return waitTime;
}

//function to return the next delay value for negative acceleration
float negativeAcceleration(float waitTime) {
    float dVelocity = waitTime * -1 * accelerationTerm;
    waitTime = 1/(dVelocity+1/waitTime);
    if (waitTime > 0.003){
      waitTime = 0.003;
    }
    return waitTime;
}

void loop() {
  digitalWrite(dirPin,HIGH);

  //accelerate for 3000 steps
  for(int x = 0; x < 6000; x++) {
    waitTime = positiveAcceleration(waitTime);
    roundedWaitTime = round(waitTime*1000000);
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(roundedWaitTime);
    digitalWrite(stepPin, LOW);
  } 

  //deccelerate for 1984 steps
  for(int x = 0; x < 1984; x++) {
    waitTime = negativeAcceleration(waitTime);
    roundedWaitTime = round(waitTime*1000000);
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(roundedWaitTime);
    digitalWrite(stepPin, LOW);
  } 

  delay(1000);
  
}
