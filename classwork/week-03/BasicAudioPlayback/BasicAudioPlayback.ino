#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioPlaySdWav           audioSample01;
AudioOutputI2S           audioOutput;
AudioConnection          patchCord1(audioSample01, 0, audioOutput, 0);
AudioConnection          patchCord2(audioSample01, 1, audioOutput, 0);
AudioControlSGTL5000     audioControl;
// GUItool: end automatically generated code

///////////////////////////////////
// copy the Design Tool code here
///////////////////////////////////

#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

void setup() {
  Serial.begin(9600);
  
  AudioMemory(8);
  
  audioControl.enable();
  audioControl.volume(0.75);
  
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
  if(!audioSample01.isPlaying()) {
    audioSample01.play("tob.wav");
    delay(5);
  }

  float amplitude = analogRead(A1) / 1023.0f;
  audioControl.volume(amplitude);
}
