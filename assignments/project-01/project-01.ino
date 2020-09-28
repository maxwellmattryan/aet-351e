int ledPins[] = {1, 3, 5, 7, 9};
int ledPinNum = 5;

int ledDelayTime = 50;

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
    delay(ledDelayTime);
    digitalWrite(ledPins[i], LOW);
  }
}
