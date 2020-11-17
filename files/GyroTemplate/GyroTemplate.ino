#include<Wire.h>
#include<movingAvg.h>

const int MPU_ADDRESS = 0x68;

int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ; 

const int BUTTON_01_PIN = 0;
const int BUTTON_02_PIN = 1;

movingAvg xAvg(8);
movingAvg yAvg(8);

int xOldVal = -1;
int yOldVal = -1;

elapsedMillis Msec = 0;

void setup() {
  Wire.begin();
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  
  Serial.begin(9600);
}

void loop() {
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDRESS, 14, true);

  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();

  Serial.print("GyX = "); Serial.print(GyX);
  Serial.print(" | GyY = "); Serial.println(GyY);
  delay(50);
}
