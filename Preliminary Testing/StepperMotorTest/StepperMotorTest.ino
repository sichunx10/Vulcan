// defines pins numbers
const int stepPin = 5; 
const int dirPin = 4; 
const int ms1Pin = 8;
const int ms2Pin = 9;
const int ms3Pin = 10;

const int stepperDelay = 800;
 
void setup() {
  // Sets the two pins as Outputs
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
  pinMode(ms1Pin,OUTPUT);
}
void loop() {
  digitalWrite(ms1Pin,LOW);
  digitalWrite(ms2Pin,LOW);
  digitalWrite(ms3Pin,LOW);

  digitalWrite(dirPin,HIGH);
  for(int x = 0; x < 6400; x++) {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(stepperDelay); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(stepperDelay); 
  }
  delay(200); // One second delay
  
  digitalWrite(dirPin,LOW); //Changes the rotations direction
  // Makes 400 pulses for making two full cycle rotation
  for(int x = 0; x < 6400; x++) {
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(stepperDelay);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(stepperDelay);
  }
  delay(200);
}
