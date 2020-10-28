#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

#include <Bounce.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform01;      //xy=80,150.00000286102295
AudioSynthWaveform       waveform02; //xy=82,204.0000056028366
AudioEffectDigitalCombine combinerL;       //xy=260.00000762939453,157.00000190734863
AudioEffectDigitalCombine combinerR; //xy=285.0000305175781,201.0000171661377
AudioEffectEnvelope      ampEgL;      //xy=457.0000114440918,157.00000381469727
AudioEffectEnvelope      ampEgR; //xy=459.0000114440918,200.0000057220459
AudioFilterStateVariable filterR; //xy=617.0000190734863,207.00000476837158
AudioFilterStateVariable filterL;        //xy=618.0000152587891,164.00000190734863
AudioOutputI2S           output;           //xy=782.7502899169922,156.75001907348633
AudioConnection          patchCord1(waveform01, 0, combinerL, 0);
AudioConnection          patchCord2(waveform01, 0, combinerR, 1);
AudioConnection          patchCord3(waveform02, 0, combinerL, 1);
AudioConnection          patchCord4(waveform02, 0, combinerR, 0);
AudioConnection          patchCord5(combinerL, ampEgL);
AudioConnection          patchCord6(combinerR, ampEgR);
AudioConnection          patchCord7(ampEgL, 0, filterL, 0);
AudioConnection          patchCord8(ampEgR, 0, filterR, 0);
AudioConnection          patchCord9(filterR, 0, output, 1);
AudioConnection          patchCord10(filterL, 0, output, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=55,20
// GUItool: end automatically generated code


int potPin01 = A3;
int potPin02 = A2;
int potPin03 = A6;

float maxFrequency = 250.0f;

int envButtonPin = 8;
Bounce envButton = Bounce(envButtonPin, 20);

void setup() {
  Serial.begin(9600);
  AudioMemory(8);
  control.enable();
  control.volume(0.75);
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
  delay(1000);

  waveform01.begin(0.4f, 100, WAVEFORM_TRIANGLE);
  waveform02.begin(0.4f, 100, WAVEFORM_TRIANGLE);

  lfoWaveform.begin(1.0f, 100, WAVEFORM_SAWTOOTH);

  combine.setCombineMode(AudioEffectDigitalCombine::AND);

  filter.resonance(2.4f);
  filter.octaveControl(4);
  filter.frequency(300.0f);
  
  envelope.attack(10.0f);
  envelope.decay(50.0f);
  envelope.sustain(0.3f);
  envelope.release(300.0f);
  pinMode(envButtonPin, INPUT_PULLUP);

  bitcrusher.bits(16);
}

void loop() {
  envButton.update();
  if(envButton.fallingEdge()) {
    envelope.noteOn();
  } else if (envButton.risingEdge()) {
    envelope.noteOff();
  }
  
  float potVal01 = analogRead(potPin01) / 1023.0f;
  waveform01.frequency(potVal01 * maxFrequency);
  
  float potVal02 = analogRead(potPin02) / 1023.0f;
  waveform02.frequency(potVal02 * maxFrequency);

  lfoWaveform.frequency(potVal02 * 30.0f);

  float potVal03 = analogRead(potPin03) / 1023.0f;
  bitcrusher.sampleRate(potVal03 * 192000.0f);
  
  float ampl = analogRead(A1) / 1023.0f;
  control.volume(ampl);
}
