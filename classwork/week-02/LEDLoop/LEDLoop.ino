int ledA = 9;
int ledB = 10;
int ledC = 11;
int ledD = 12;

int potPin = A0;

void setup() {
  pinMode(ledA, OUTPUT);
  pinMode(ledB, OUTPUT);
  pinMode(ledC, OUTPUT);
  pinMode(ledD, OUTPUT);
}

void loop() {
  toggleLed(ledA);
  toggleLed(ledB);
  toggleLed(ledC);
  toggleLed(ledD);
  
  toggleLed(ledC);
  toggleLed(ledB);
  toggleLed(ledA);
}

void toggleLed(int ledPin) {
  digitalWrite(ledPin, HIGH);
  
  int delayValue = analogRead(potPin) / 2;
  delay(delayValue);
  
  digitalWrite(ledPin, LOW);
}
