#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// Audio components
AudioPlaySdWav           sample01;   
AudioPlaySdWav           sample02; 

AudioAmplifier           sample02AmpRight;
AudioAmplifier           sample01AmpRight; 
AudioAmplifier           sample01AmpLeft; 
AudioAmplifier           sample02AmpLeft; 

AudioFilterStateVariable mainFilterLeft;       
AudioFilterStateVariable mainFilterRight; 

AudioAmplifier           mainAmpLeft;          
AudioAmplifier           mainAmpRight;      

AudioOutputI2S           output;          

AudioControlSGTL5000     control;

// Patches and connections
AudioConnection          patchCord1(sample01, 0, sample01AmpLeft, 0);
AudioConnection          patchCord2(sample01, 1, sample01AmpRight, 0);
AudioConnection          patchCord3(sample02, 0, sample02AmpLeft, 0);
AudioConnection          patchCord4(sample02, 1, sample02AmpRight, 0);
AudioConnection          patchCord5(sample02AmpRight, 0, mainFilterRight, 0);
AudioConnection          patchCord6(sample01AmpRight, 0, mainFilterRight, 0);
AudioConnection          patchCord7(sample01AmpLeft, 0, mainFilterLeft, 0);
AudioConnection          patchCord8(sample02AmpLeft, 0, mainFilterLeft, 0);
AudioConnection          patchCord9(mainFilterLeft, 2, mainAmpLeft, 0);
AudioConnection          patchCord10(mainFilterRight, 2, mainAmpRight, 0);
AudioConnection          patchCord11(mainAmpLeft, 0, output, 0);
AudioConnection          patchCord12(mainAmpRight, 0, output, 1);

// Pot pins
int sample01AmpPot = A3;
int sample02AmpPot = A2;
int mainFilterPot = A6;

// Pre-processor definitions for SD card
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

void setup() {
  Serial.begin(9600);
  
  initAudioComponents();
  initSDCard();
}

void initAudioComponents() {
  AudioMemory(8);
  
  control.enable();
  control.volume(0.8f);

  mainFilterLeft.resonance(1.2f);
  mainFilterRight.resonance(1.2f);

  mainAmpLeft.gain(1.0f);
  mainAmpLeft.gain(1.0f);
}

void initSDCard() {
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if(!(SD.begin(SDCARD_CS_PIN))) {
    while(1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
}

void loop() {
  if(!sample01.isPlaying()) {
    sample01.play("cult.wav");
    delay(1);
  }

  if(!sample02.isPlaying()) {
    sample02.play("age.wav");
    delay(1);
  }

  float sample01Amp = readAmplitude(sample01AmpPot);
  sample01AmpLeft.gain(sample01Amp);
  sample01AmpRight.gain(sample01Amp);
  float sample02Amp = readAmplitude(sample02AmpPot);
  sample02AmpLeft.gain(sample02Amp);
  sample02AmpRight.gain(sample02Amp);

  float filterFrequency = readFilter(mainFilterPot);
  mainFilterRight.frequency(filterFrequency);
  mainFilterLeft.frequency(filterFrequency);

  float ampValue = analogRead(A1) / 1023.0f;
  mainAmpLeft.gain(ampValue);
  mainAmpRight.gain(ampValue);
}

float readAmplitude(int ampPot) {
  return constrain(analogRead(ampPot) / 1023.0f, 0.01f, 1.0f);
}

float readFilter(int filterPot) {
  float value = analogRead(filterPot);
  return map(value, 0.0f, 1023.0f, 0.0f, 1000.0f);
}
