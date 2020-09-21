// defines pins numbers
const int stepPin = 5; 
const int dirPin = 4; 
const int ms1Pin = 8;
const int ms2Pin = 9;
const int ms3Pin = 10;

//for Full step, use 800 delay
//for Half step, use 400 delay, and so on
const int stepperDelay = 1000;
 
void setup() {
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
  pinMode(ms1Pin,OUTPUT);
  pinMode(ms2Pin,OUTPUT);
  pinMode(ms3Pin,OUTPUT);
}
void loop() {
  digitalWrite(ms1Pin,LOW);
  digitalWrite(ms2Pin,LOW);
  digitalWrite(ms3Pin,LOW);

  digitalWrite(dirPin,HIGH); //clockwise rotation
  for(int x = 0; x < 400; x++) {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(stepperDelay); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(stepperDelay); 
  }
  delay(200); // 20 ms delay
  
  digitalWrite(dirPin,LOW); //counterclockwise rotation
  for(int x = 0; x < 400; x++) {
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(stepperDelay);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(stepperDelay);
  }
  delay(200);
}
