int touchPin = 15;
int ledPin = 10;

void setup() {
  pinMode(touchPin, INPUT);
  pinMode(ledPin, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  int touchVal = touchRead(touchPin);
  Serial.println(touchVal);

  digitalWrite(ledPin, touchVal >= 1000 ? HIGH : LOW);
  
  delay(50);
}
