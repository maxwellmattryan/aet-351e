#include<movingAvg.h>
#include<Wire.h>

// I2C address of the GY-521
const int MPU_addr = 0x68;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ; 

unsigned long currentTime = 0;
unsigned long previousTime = 0;

movingAvg xAxis(64);

///////////////////////////////////
// copy the Design Tool code here
///////////////////////////////////

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioPlaySdWav           audioSample01;     //xy=230,234.00000476837158
AudioFilterStateVariable filter02;        //xy=478.51563262939453,289.51562881469727
AudioFilterStateVariable filter01;        //xy=480.515625,193.515625
AudioOutputI2S           output;           //xy=763.9999961853027,235.00000476837158
AudioConnection          patchCord1(audioSample01, 0, filter01, 0);
AudioConnection          patchCord2(audioSample01, 1, filter02, 0);
AudioConnection          patchCord3(filter02, 0, output, 1);
AudioConnection          patchCord4(filter01, 0, output, 0);
AudioControlSGTL5000     audioControl;     //xy=191,91.00000190734863
// GUItool: end automatically generated code

///////////////////////////////////
// copy the Design Tool code here
///////////////////////////////////

#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

void setup() {
  Serial.begin(9600);

  xAxis.begin();

  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  // PWR_MGMT_1 register
  Wire.write(0x6B);
  // set to zero (wakes up the GY-521)
  Wire.write(0); Wire.endTransmission(true);
  
  AudioMemory(8);
  
  audioControl.enable();
  audioControl.volume(0.25);
  
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
  
  delay(1000);
}

void loop() {
  currentTime = millis();
  
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true);

  GyX = Wire.read() << 8 | Wire.read();
  int avgXAxis = xAxis.reading(GyX);
  int xAxisLimit = constrain(avgXAxis, -17000, 17000);
  int xAxisMap = map(xAxisLimit, -17000, 17000, 60, 7000);

  filter01.frequency(xAxisMap);
  filter02.frequency(xAxisMap);

  filter01.resonance(3.0f);
  filter02.resonance(3.0f);

  if(currentTime - previousTime > 50) {
    Serial.print("GyX = "); Serial.print(GyX);
    Serial.print("| AvgX = "); Serial.println(xAxisLimit);
    previousTime = currentTime;
  }
  
  if(!audioSample01.isPlaying()) {
    audioSample01.play("drm.wav");
    delay(5);
  } 

  // ONBOARD VOLUME
  float volume = analogRead(A1) / 1023.0f;
  audioControl.volume(volume );
}
