// defines pins numbers
const int stepPin = 5; 
const int dirPin = 4; 
const int ms1Pin = 8;
const int ms2Pin = 9;
const int ms3Pin = 10;

//for Full step, use 800 delay
//for Half step, use 400 delay, and so on
const int stepperDelay = 1600;
 
void setup() {
  //setup the pins
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
  pinMode(ms1Pin,OUTPUT);
  pinMode(ms2Pin,OUTPUT);
  pinMode(ms3Pin,OUTPUT);

  //change the microstepping
  digitalWrite(ms1Pin,LOW);
  digitalWrite(ms2Pin,LOW);
  digitalWrite(ms3Pin,LOW);
}

void simpleMove(int steps) {
  for (int i=0; i<steps; i++) {
    digitalWrite(stepPin, HIGH);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepperDelay);
  }
}

void loop() {
  digitalWrite(dirPin, HIGH);
  simpleMove(1600);
  delay(200);
  digitalWrite(dirPin, LOW);
  simpleMove(1600);
  delay(1000);
}
