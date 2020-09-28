int ledPins[] = {1, 3, 5, 7, 9};
int ledPinNum = 5;
int idleLedPin = 2;

int randomnessPotPin = A0;
int ratePotPin = A1;

int minDelayTime = 5;
int maxDelayTime = 250;

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
  if(analogRead(ratePotPin) <= 1) {
    digitalWrite(ledPins[idleLedPin], HIGH);
  } else {
    digitalWrite(ledPins[idleLedPin], LOW);
    
    for(int i = 0; i < ledPinNum; i++) {
      int index = random(1024) < analogRead(randomnessPotPin) ? random(ledPinNum) : i;
      
      digitalWrite(ledPins[index], HIGH);
      delay(readDelayTime());
      digitalWrite(ledPins[index], LOW);
    }
  }
}

int readDelayTime() {
  float potValue = 1.0f - analogRead(ratePotPin) / 1023.0f;
  return minDelayTime + (maxDelayTime - minDelayTime) * potValue;
}
