/*
 * Author: Sichun Xu
 * Date Created: 2020-11-12
 * 
 * License: This is open source, so do whatever you want with it. 
 * You may credit me if you would like, but it is not required
 * 
 * Version 0.1
 * This is to find the toMax variable that is used in the main Vulcan 
 * program. This constant indicates the number of steps it takes to 
 * accelerate the stepper motor from stationary to maximum angular velocity.
 */

float waitTime = 0.003;
int steps = 0;
int flag = 1;

float positiveAcceleration(float waitTime) {
    float dVelocity = waitTime * 4000;
    waitTime = 1/(dVelocity+1/waitTime);
    if (waitTime < 0.00025){
      waitTime = 0.00025;
    }
    return waitTime;
}

void setup() {
  Serial.begin(115200);

}

void loop() {
  while(flag){
    waitTime = positiveAcceleration(waitTime);
    Serial.println(waitTime*1000000);
    steps++;
    if(waitTime*1000000 <= 250.1){
      Serial.print("The ToMax variable is equal to: ");
      Serial.println(steps);
      flag = 0;
    }
  }
  
  

}
