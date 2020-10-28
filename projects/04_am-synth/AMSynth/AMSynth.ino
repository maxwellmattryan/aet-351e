#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

#include <Bounce.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

AudioSynthWaveform       osc01;   
AudioSynthWaveform       osc02; 
AudioEffectDigitalCombine combinerL;       
AudioEffectDigitalCombine combinerR; 
AudioSynthWaveform       lfo;    
AudioEffectEnvelope      ampEgL;   
AudioEffectEnvelope      ampEgR; 
AudioFilterStateVariable filterR; 
AudioFilterStateVariable filterL;        
AudioOutputI2S           output;         
AudioConnection          patchCord1(osc01, 0, combinerL, 0);
AudioConnection          patchCord2(osc01, 0, combinerR, 1);
AudioConnection          patchCord3(osc02, 0, combinerL, 1);
AudioConnection          patchCord4(osc02, 0, combinerR, 0);
AudioConnection          patchCord5(combinerL, ampEgL);
AudioConnection          patchCord6(combinerR, ampEgR);
AudioConnection          patchCord7(lfo, 0, filterL, 1);
AudioConnection          patchCord8(lfo, 0, filterR, 1);
AudioConnection          patchCord9(ampEgL, 0, filterL, 0);
AudioConnection          patchCord10(ampEgR, 0, filterR, 0);
AudioConnection          patchCord11(filterR, 0, output, 1);
AudioConnection          patchCord12(filterL, 0, output, 0);
AudioControlSGTL5000     control;     

// PARAMETERS
float oscFrequencyThreshold = 5400.0f;
int oscWaveformIndex = 0;
int numOscWaveforms = 4;

int combineModeIndex = 2;
int numCombineModes = 3;

float lfoRateThreshold = 30.0f;

float filterCutoffFrequencyThreshold = 5200.0f;

// POTS
int osc01Pot = A7;
int osc02Pot = A6;
int lfoPot = A3;
int filterPot = A2;

// BUTTONS
int oscWaveformButtonPin = 0;
Bounce oscWaveformButton = Bounce(oscWaveformButtonPin, 15);
int combineModeButtonPin = 1;
Bounce combineModeButton = Bounce(combineModeButtonPin, 15);
int ampEgButtonPin = 2;
Bounce ampEgButton = Bounce(ampEgButtonPin, 15);

void setup() {
  Serial.begin(9600);
  
  initAudio();
  initSdCard();
  
  delay(1000);

  initControls();
  initComponents();
}

void initAudio() {
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
  pinMode(oscWaveformButtonPin, INPUT_PULLUP);
  pinMode(combineModeButtonPin, INPUT_PULLUP);
  pinMode(ampEgButtonPin, INPUT_PULLUP);
}

void initComponents() {
  osc01.begin(0.5f, 100.0f, WAVEFORM_TRIANGLE);
  osc02.begin(0.5f, 50.0f, WAVEFORM_SINE);

  combinerL.setCombineMode(AudioEffectDigitalCombine::AND);
  combinerR.setCombineMode(AudioEffectDigitalCombine::AND);

  ampEgL.attack(12.0f);
  ampEgL.decay(72.0f);
  ampEgL.sustain(0.4f);
  ampEgL.release(460.0f);

  ampEgR.attack(12.0f);
  ampEgR.decay(72.0f);
  ampEgR.sustain(0.4f);
  ampEgR.release(460.0f);

  lfo.begin(1.0f, 100.0f, WAVEFORM_SINE);

  filterL.frequency(240.0f);
  filterL.resonance(1.6f);
  filterL.octaveControl(4);

  filterR.frequency(240.0f);
  filterR.resonance(1.6f);
  filterR.octaveControl(4);
}

void loop() {
  float osc01Frequency = (analogRead(osc01Pot) / 1023.0f) * oscFrequencyThreshold;
  osc01.frequency(osc01Frequency);

  float osc02Frequency = (analogRead(osc02Pot) / 1023.0f) * oscFrequencyThreshold;
  osc02.frequency(osc02Frequency);

  oscWaveformButton.update();
  if(oscWaveformButton.fallingEdge()) {
    oscWaveformIndex += 1;
    oscWaveformIndex %= numOscWaveforms;
    
    osc02.begin(oscWaveformIndex);
  }

  combineModeButton.update();
  if(combineModeButton.fallingEdge()) {
    combineModeIndex += 1;
    combineModeIndex %= numCombineModes;

    combinerL.setCombineMode(combineModeIndex);
    combinerR.setCombineMode(combineModeIndex);
  }
  
  ampEgButton.update();
  if(ampEgButton.fallingEdge()) {
    ampEgL.noteOn();
    ampEgR.noteOn();
  } else if(ampEgButton.risingEdge()) {
    ampEgL.noteOff();
    ampEgR.noteOff();
  }

  float lfoPotVal = analogRead(lfoPot) / 1023.0f;
  lfo.amplitude(lfoPotVal);
  lfo.frequency(lfoPotVal * lfoRateThreshold);

  float filterCutoffFrequency = (analogRead(filterPot) / 1023.0f) * filterCutoffFrequencyThreshold;
  filterL.frequency(filterCutoffFrequency);
  filterR.frequency(filterCutoffFrequency);

  delay(20);
  
  float amplitude = analogRead(A1) / 1023.0f;
  control.volume(amplitude);
}
