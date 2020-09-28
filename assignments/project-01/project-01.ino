int ledPins[] = {1, 3, 5, 7, 9};
int ledPinNum = 5;

int potPin = A0;

int maxDelayTime = 100;

// SETUP
void setup() {
  Serial.begin(9600);

  initPinModes();
}

void initPinModes() {
  for(int i = 0; i < ledPinNum; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
}

// LOOP
void loop() {
  writeLedPins();
}

void writeLedPins() {
  for(int i = 0; i < ledPinNum; i++) {
    digitalWrite(ledPins[i], HIGH);
    delay(readDelayTime());
    digitalWrite(ledPins[i], LOW);
  }
}

int readDelayTime() {
  float potValue = 1.0f - analogRead(potPin) / 1023.0f;
  Serial.println(potValue, maxDelayTime * potValue);
  return maxDelayTime * potValue;
}
