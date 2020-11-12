// defines pins numbers
const int buttonPin = 12;
const int stepPin = 5; 
const int dirPin = 4; 
const int ms1Pin = 10;
const int ms2Pin = 9;
const int ms3Pin = 8;
const int ledPin = 13;
float waitTime = 0.003;
int buttonStatus;
int roundedWaitTime;
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
  pinMode(ledPin, OUTPUT);  

  //change the microstepping
  digitalWrite(ms1Pin,LOW);
  digitalWrite(ms2Pin,HIGH);
  digitalWrite(ms3Pin,LOW);

  //Serial.begin(115200);
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
  digitalWrite(dirPin,HIGH);
  //timer1 = micros();
  buttonStatus = digitalRead(buttonPin);
  if(buttonStatus == LOW){
    waitTime = positiveAcceleration(waitTime);
    roundedWaitTime = round(waitTime*1000000);
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(roundedWaitTime);
    digitalWrite(stepPin, LOW);
    //Serial.println(roundedWaitTime);
  } 
  else {
    waitTime = negativeAcceleration(waitTime);
    roundedWaitTime = round(waitTime*1000000);
    if (roundedWaitTime < 3000){
      digitalWrite(stepPin,HIGH);
      delayMicroseconds(roundedWaitTime);
      digitalWrite(stepPin, LOW);
    }
    //Serial.println(roundedWaitTime);
  }
  //timer2 = micros();
  //difference = timer2-timer1;
  //Serial.println(difference);
  
}
