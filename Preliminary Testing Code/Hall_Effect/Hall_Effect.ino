const int hallPin = 2;

void setup() {
  pinMode(hallPin, INPUT);
  Serial.begin(9600);

}

void loop() {
  int hallValue = digitalRead(hallPin);
  Serial.println(hallValue);

}
