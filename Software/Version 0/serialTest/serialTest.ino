int targetPos1;
int targetPos2;
int targetPos3;

struct motors {
  int targetPos1, targetPos2, targetPos3;
};

void setup() {
  Serial.begin(9600);
}

motors motorParser() {
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

      motors example = {targetPos1, targetPos2, targetPos3};
      return example;
    }
  }
}

void loop() {
  if (Serial.available() > 0){
    motors stepper = motorParser();
    
    Serial.println(stepper.targetPos1);
    Serial.println(stepper.targetPos2);
    Serial.println(stepper.targetPos3);
  }
}