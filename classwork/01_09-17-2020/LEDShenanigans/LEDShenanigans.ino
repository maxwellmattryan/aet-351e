int potPin = A0; // 14 also works
int ledPin = 10;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  int potValue = analogRead(potPin);
  analogWrite(ledPin, potValue);
  Serial.println(potValue);

  delay(20);
}
