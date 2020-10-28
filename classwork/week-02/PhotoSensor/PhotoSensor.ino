int photoPin = A0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  int photoVal = analogRead(photoPin);
  Serial.println(photoVal);

  delay(50);
}
