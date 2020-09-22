// defines pins numbers
const int stepPin = 5; 
const int dirPin = 4; 
const int ms1Pin = 8;
const int ms2Pin = 9;
const int ms3Pin = 10;
 
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
}

void simpleAcceleration(int steps) {
  int lowSpeed = 2000;
  int highSpeed = 250;
  int change = 2;

  int rampUpStop = (lowSpeed - highSpeed)/2;
  if (rampUpStop > steps/2) {
    rampUpStop = steps/2 ;
  }
  int rampDownStart = steps-rampUpStop;

  int d = lowSpeed;

  for (int i=0; i<steps; i++) {
    digitalWrite(stepPin, HIGH);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(d);

    if (i<rampUpStop){
      d -= change;
    }
    else if (i > rampDownStart) {
      d += change;
    }
  }
}

void loop() {
  digitalWrite(dirPin, HIGH);
  simpleAcceleration(10000);
  delay(200);
  digitalWrite(dirPin, LOW);
  simpleAcceleration(10000);
  delay(200);
}
