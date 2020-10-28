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
AudioPlaySdWav           sampler;     //xy=59.99999237060547,220.6666784286499
AudioEffectGranular      granular01;      //xy=234.66665649414062,81.66666412353516
AudioEffectGranular      granular02; //xy=234.6666717529297,131.6666717529297
AudioEffectGranular      granular03; //xy=234.6666717529297,187.6666717529297
AudioEffectGranular      granular04; //xy=234.6666717529297,265.6666564941406
AudioEffectGranular      granular05; //xy=234.6666717529297,373.6666564941406
AudioEffectGranular      granular06; //xy=235.6666717529297,319.6666564941406
AudioMixer4              mixerR; //xy=407.66666412353516,322.66666412353516
AudioMixer4              mixerL;         //xy=410.66666412353516,138.66667556762695
AudioFilterStateVariable filterR; //xy=572.6666870117188,319.6666564941406
AudioFilterStateVariable filterL;        //xy=584.6666564941406,138.66666412353516
AudioEffectEnvelope      ampEgL;      //xy=747.3333358764648,141.66667556762695
AudioEffectEnvelope      ampEgR; //xy=750.3333129882812,319.6666564941406
AudioOutputI2S           output;           //xy=969.7502174377441,148.75002002716064
AudioOutputUSB           usbOutput;           //xy=973.5156326293945,312.51566886901855
AudioConnection          patchCord1(sampler, 0, granular01, 0);
AudioConnection          patchCord2(sampler, 0, granular02, 0);
AudioConnection          patchCord3(sampler, 0, granular03, 0);
AudioConnection          patchCord4(sampler, 1, granular04, 0);
AudioConnection          patchCord5(sampler, 1, granular05, 0);
AudioConnection          patchCord6(sampler, 1, granular06, 0);
AudioConnection          patchCord7(granular01, 0, mixerL, 0);
AudioConnection          patchCord8(granular02, 0, mixerL, 1);
AudioConnection          patchCord9(granular03, 0, mixerL, 2);
AudioConnection          patchCord10(granular04, 0, mixerR, 0);
AudioConnection          patchCord11(granular06, 0, mixerR, 2);
AudioConnection          patchCord12(granular05, 0, mixerR, 1);
AudioConnection          patchCord13(mixerR, 0, filterR, 0);
AudioConnection          patchCord14(mixerL, 0, filterL, 0);
AudioConnection          patchCord15(filterR, 0, ampEgR, 0);
AudioConnection          patchCord16(filterL, 0, ampEgL, 0);
AudioConnection          patchCord17(ampEgL, 0, output, 0);
AudioConnection          patchCord18(ampEgL, 0, usbOutput, 0);
AudioConnection          patchCord19(ampEgR, 0, output, 1);
AudioConnection          patchCord20(ampEgR, 0, usbOutput, 1);
AudioControlSGTL5000     control;     //xy=55,20
// GUItool: end automatically generated code



int numSamples = 14700;
int16_t granularMemory[14700]; // define granular memory

int button01Pin = 0;
Bounce button01 = Bounce(button01Pin, 15);
int button02Pin = 1;
Bounce button02 = Bounce(button02Pin, 15);

int potA = A2;
int potB = A3;
int potC = A6;

void setup() {
  Serial.begin(9600);
  
  initAudio();
  initSdCard();
  
  delay(1000);

  initControls();
  initComponents();
}

void initAudio() {
  granular01.begin(granularMemory, numSamples);
  granular02.begin(granularMemory, numSamples);
  granular03.begin(granularMemory, numSamples);
  granular04.begin(granularMemory, numSamples);
  granular05.begin(granularMemory, numSamples);
  granular06.begin(granularMemory, numSamples);
       
  AudioMemory(8);
  
  control.enable();
  control.volume(0.75);
}

void initSdCard() {
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
}

void initControls() {
  pinMode(button01Pin, INPUT_PULLUP);
  pinMode(button02Pin, INPUT_PULLUP);
}

void initComponents() {
  granular01.beginFreeze(100);
  granular02.beginFreeze(100);
  granular03.beginFreeze(100);
  granular04.beginFreeze(100);
  granular05.beginFreeze(100);
  granular06.beginFreeze(100);

  mixerL.gain(0, 0.2f);
  mixerL.gain(1, 0.2f);
  mixerL.gain(2, 0.2f);

  mixerR.gain(0, 0.2f);
  mixerR.gain(1, 0.2f);
  mixerR.gain(2, 0.2f);

  filterL.frequency(500.0f);
  filterL.resonance(1.4f);
  filterL.octaveControl(4);

  filterR.frequency(500.0f);
  filterR.resonance(1.4f);
  filterR.octaveControl(4);

  ampEgL.attack(360.0f);
  ampEgL.decay(240.0f);
  ampEgL.sustain(0.4f);
  ampEgL.release(1200.0f);

  ampEgR.attack(360.0f);
  ampEgR.decay(240.0f);
  ampEgR.sustain(0.4f);
  ampEgR.release(1200.0f);
}

void loop() {
  if (sampler.isPlaying() == false) {
    sampler.play("age.wav");
    delay(10);
  }

  float potAVal = analogRead(potA) / 1023.f;
  int grainSize01 = potAVal * 250.0f + 10.0f;
  int grainSize02 = potAVal * 100.0f + 12.0f;
  int grainSize03 = potAVal * 50.0f + 14.0f;
  int grainSize04 = potAVal * 25.0f + 16.0f;
  int grainSize05 = potAVal * 12.5f + 18.0f;
  int grainSize06 = potAVal * 6.25f + 20.0f;

  button01.update();
  if(button01.fallingEdge()) {
    granular01.beginFreeze(grainSize01);
    granular02.beginFreeze(grainSize02);
    granular03.beginFreeze(grainSize03);
    granular04.beginFreeze(grainSize04);
    granular05.beginFreeze(grainSize05);
    granular06.beginFreeze(grainSize06);
  }

  float pitchVal = (analogRead(potB) / 1023.0f) * 2.0f;
  granular01.setSpeed(pitchVal);
  granular03.setSpeed(pitchVal);
  granular05.setSpeed(pitchVal);

  button02.update();
  if(button02.fallingEdge()) {
    ampEgL.noteOn();
    ampEgR.noteOn();
  } else if(button02.risingEdge()) {
    ampEgL.noteOff();
    ampEgR.noteOff();
  }

  float cutoffFrequency = (analogRead(potC) / 1023.0f) * 5200.0f;
  filterL.frequency(cutoffFrequency);
  filterR.frequency(cutoffFrequency);

  float amplitude = analogRead(A1) / 1023.0f;
  control.volume(amplitude);
}
