// defines pins numbers
int stepPin = 5; 
int dirPin = 4; 
 
void setup() {
  // Sets the two pins as Outputs
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
  digitalWrite(dirPin,HIGH);
}

void loop() {
  int stepperDelay = 3000;

  //accelerate from 3000 microsecond delay to 700 microsecond delay 
  for(int x = 0; x < 2300; x++) {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(stepperDelay); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(stepperDelay); 

    //decrease stepper delay value by 1 microsecond
    stepperDelay--;
  }

  //run at constant velocity for 2000 steps
  for(int x = 0; x < 2000; x++) {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(700); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(700); 
  }

  delay(1000); // One second delay
}
