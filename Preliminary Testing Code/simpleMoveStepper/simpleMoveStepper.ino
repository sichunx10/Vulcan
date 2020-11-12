// defines pins numbers
const int stepPin = 5; 
const int dirPin = 4; 
const int ms1Pin = 10;
const int ms2Pin = 9;
const int ms3Pin = 8;

//for Full step, use 1600 delay
//for Half step, use 800 delay, and so on
const int stepperDelay = 400;
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

  //change the microstepping
  digitalWrite(ms1Pin,LOW);
  digitalWrite(ms2Pin,HIGH);
  digitalWrite(ms3Pin,LOW);

  Serial.begin(115200);
}

void simpleMove(int steps) {
  for (int i=0; i<steps; i++) {
    timer1 = micros();
    digitalWrite(stepPin, HIGH);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepperDelay);
    timer2 = micros();
    difference = timer2 - timer1;
    Serial.println(difference);
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
