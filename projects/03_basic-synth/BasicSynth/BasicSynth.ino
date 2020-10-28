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
AudioSynthWaveform       variableOsc;      //xy=232.75002670288086,210.75002479553223
AudioSynthWaveform       staticOsc;      //xy=233.75,151.75000286102295
AudioMixer4              oscMixer;         //xy=434.75,178.75
AudioEffectEnvelope      ampEg;      //xy=587.7500610351562,179.75001525878906
AudioFilterStateVariable filter;        //xy=771.75,168.75
AudioOutputI2S           output;           //xy=987.0003662109375,174.00003051757812
AudioConnection          patchCord1(variableOsc, 0, oscMixer, 1);
AudioConnection          patchCord2(staticOsc, 0, oscMixer, 0);
AudioConnection          patchCord3(oscMixer, ampEg);
AudioConnection          patchCord4(ampEg, 0, filter, 0);
AudioConnection          patchCord5(filter, 0, output, 0);
AudioConnection          patchCord6(filter, 0, output, 1);
AudioControlSGTL5000     control;     //xy=226.00001525878906,50.99999809265137
// GUItool: end automatically generated code

float highFrequencyThreshold = 1800.0f;
float lowFrequencyThreshold = 30.0f;

int staticOscFrequencyPot = A7;
int variableOscFrequencyPot = A6;

int variableOscWaveformButtonPin = 5;
Bounce variableOscWaveformButton = Bounce(variableOscWaveformButtonPin, 20);
int variableOscWaveformIndex = 0;

int filterFrequencyPot = A3;
float highFilterFrequencyThreshold = 5200.0f;
float lowFilterFrequencyThreshold = 30.0f;

bool isInSequence = false;
float sequenceStartTime = 0.0f;
int sequenceTimePot = A2;
float lowSequenceTimeThreshold = 10.0f;
int sequenceStartButtonPin = 8;
Bounce sequenceStartButton = Bounce(sequenceStartButtonPin, 20);

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
  control.volume(0.75f);
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
  pinMode(variableOscWaveformButtonPin, INPUT_PULLUP);
  pinMode(sequenceStartButtonPin, INPUT_PULLUP);
}

void initComponents() {
  staticOsc.begin(1.0f, 200.0f, WAVEFORM_SINE);
  variableOsc.begin(1.0f, 200.0f, WAVEFORM_TRIANGLE);

  oscMixer.gain(0, 1.0f);
  oscMixer.gain(1, 1.0f);

  ampEg.attack(10.0f);
  ampEg.decay(60.0f);
  ampEg.sustain(0.2f);
  ampEg.release(120.0f);

  filter.frequency(500.0f);
  filter.resonance(1.4f);
}

void loop() {
  sequenceStartButton.update();
  if(sequenceStartButton.fallingEdge()) {
    isInSequence = !isInSequence;

    if(isInSequence) {      
      ampEg.noteOn();

      sequenceStartTime = millis();
    } else {
      ampEg.noteOff();
    }
  }

  if(isInSequence) {
    float sequenceTimeIn = analogRead(sequenceTimePot) + lowSequenceTimeThreshold;
    ampEg.release(sequenceTimeIn);
    
    if(millis() - sequenceStartTime > sequenceTimeIn) {
      ampEg.noteOff();

      sequenceStartTime = millis();

      ampEg.noteOn();
    }
  }

  variableOscWaveformButton.update();
  if(variableOscWaveformButton.fallingEdge()) {
    variableOscWaveformIndex += 1;
    variableOscWaveformIndex %= 5;

    Serial.println(variableOscWaveformIndex);

    variableOsc.begin(variableOscWaveformIndex);
  }
  
  float staticOscFrequencyIn = analogRead(staticOscFrequencyPot) / 1023.0f;
  float staticOscFrequency = staticOscFrequencyIn * (highFrequencyThreshold - lowFrequencyThreshold) + lowFrequencyThreshold;
  staticOsc.frequency(staticOscFrequency);

  float variableOscFrequencyIn = analogRead(variableOscFrequencyPot) / 1023.0f;
  float variableOscFrequency = variableOscFrequencyIn * (highFrequencyThreshold - lowFrequencyThreshold) + lowFrequencyThreshold;
  variableOsc.frequency(variableOscFrequency);

  float filterFrequencyIn = analogRead(filterFrequencyPot) / 1023.0f;
  float filterFrequency = filterFrequencyIn * (highFilterFrequencyThreshold - lowFilterFrequencyThreshold) + lowFilterFrequencyThreshold;
  filter.frequency(filterFrequency);

  float amplitude = analogRead(A1) / 1023.0f;
  control.volume(amplitude);
}
