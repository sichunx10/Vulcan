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

  Serial.begin(9600);
}

void constantAcceleration(int steps) {
  int delays[steps];
  float angle = 1;
  float accel = 0.01;
  float c0 = 1500;
  float lastDelay = 0;
  int highSpeed = 240;

  for (int i=0; i<steps; i++){
    float d = c0;
    if (i>0){
      d = lastDelay - (2*lastDelay) / (4*i+1);
    }
    if (d < highSpeed){
      d = highSpeed;
    }
    delays[i] = d;
    lastDelay = d;
    Serial.println(d);
  }

  for (int i =0; i < steps; i++){
    digitalWrite(stepPin, HIGH);
    delayMicroseconds( delays[i]);
    digitalWrite(stepPin, LOW);
  }

  for (int i =0; i < steps; i++){
    digitalWrite(stepPin, HIGH);
    delayMicroseconds( delays[steps-i-1]);
    digitalWrite(stepPin, LOW);
  }

}

void loop() {
  digitalWrite(dirPin, HIGH);
  constantAcceleration(5000);
  delay(200);
  digitalWrite(dirPin, LOW);
  constantAcceleration(5000);
  delay(200);

  while(true);
}
