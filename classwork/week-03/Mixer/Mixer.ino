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
AudioPlaySdWav           audioSample02;     //xy=248,310
AudioPlaySdWav           audioSample01;     //xy=251,192
AudioAmplifier           ampRightChannel; //xy=442,331
AudioAmplifier           ampLeftChannel;           //xy=446,288
AudioMixer4              mixerLeftChannel;         //xy=685,190
AudioMixer4              mixerRightChannel;         //xy=690,294
AudioOutputI2S           i2s2;           //xy=923,246
AudioConnection          patchCord1(audioSample02, 0, ampLeftChannel, 0);
AudioConnection          patchCord2(audioSample02, 1, ampRightChannel, 0);
AudioConnection          patchCord3(audioSample01, 0, mixerLeftChannel, 0);
AudioConnection          patchCord4(audioSample01, 1, mixerRightChannel, 0);
AudioConnection          patchCord5(ampRightChannel, 0, mixerRightChannel, 1);
AudioConnection          patchCord6(ampLeftChannel, 0, mixerLeftChannel, 1);
AudioConnection          patchCord7(mixerLeftChannel, 0, i2s2, 0);
AudioConnection          patchCord8(mixerRightChannel, 0, i2s2, 1);
AudioControlSGTL5000     audioControl;     //xy=91,30
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

  mixerLeftChannel.gain(0, 0.5f);
  mixerLeftChannel.gain(1, 0.5f);
  mixerRightChannel.gain(0, 0.5f);
  mixerRightChannel.gain(1, 0.5f);

  ampLeftChannel.gain(2.0f);
  ampRightChannel.gain(2.0f);
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
  int xAxisMap = map(xAxisLimit, -17000, 17000, 0, 10000);

  if(currentTime - previousTime > 50) {
    Serial.print("GyX = "); Serial.print(GyX);
    Serial.print("| AvgX = "); Serial.println(xAxisLimit);
    previousTime = currentTime;
  }
  
  if(!audioSample01.isPlaying()) {
    audioSample01.play("drm.wav");
    delay(5);
  } 
  
  if(!audioSample02.isPlaying()) {
    audioSample02.play("amn.wav");
    delay(5);
  }
  
  float mixValue1 = xAxisMap / 10000.0f;
  float mixValue2 = 1.0f - mixValue1;
  mixerLeftChannel.gain(0, mixValue1);
  mixerLeftChannel.gain(1, mixValue2);
  mixerRightChannel.gain(0, mixValue1);
  mixerRightChannel.gain(1, mixValue2);

  float volume = analogRead(A1) / 1023.0f;
  audioControl.volume(volume );
}
