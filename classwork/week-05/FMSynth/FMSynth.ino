#include <Bounce.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       modulator02; //xy=258.75000381469727,217.7500057220459
AudioSynthWaveform       modulator01;      //xy=261.7500228881836,161.74999475479126
AudioSynthWaveform       modulator03; //xy=261.75,285.7499990463257
AudioMixer4              mixer;         //xy=444.7500228881836,203.75000381469727
AudioEffectEnvelope      modEnvelope;      //xy=632.7500228881836,201.7500286102295
AudioSynthWaveformModulated carrier;   //xy=823.7499885559082,201.75000953674316
AudioEffectEnvelope      envelope;      //xy=988.7500305175781,202.75000953674316
AudioOutputI2S           output;           //xy=1150.0003623962402,203.00001335144043
AudioConnection          patchCord1(modulator02, 0, mixer, 1);
AudioConnection          patchCord2(modulator01, 0, mixer, 0);
AudioConnection          patchCord3(modulator03, 0, mixer, 2);
AudioConnection          patchCord4(mixer, modEnvelope);
AudioConnection          patchCord5(modEnvelope, 0, carrier, 0);
AudioConnection          patchCord6(carrier, envelope);
AudioConnection          patchCord7(envelope, 0, output, 0);
AudioConnection          patchCord8(envelope, 0, output, 1);
AudioControlSGTL5000     control;     //xy=245.00001525878906,61
// GUItool: end automatically generated code


#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

int frequencyPot = A2;
int modulationIndexPot = A3;

int envelopeButtonPin = 8;
Bounce envelopeButton = Bounce(envelopeButtonPin, 20);

int killswitchButtonPin = 20;
Bounce killswitchButton = Bounce(killswitchButtonPin, 20);

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

  pinMode(envelopeButtonPin, INPUT_PULLUP);

  envelope.attack(20.0f);
  envelope.decay(50.0f);
  envelope.sustain(0.1f);
  envelope.release(160.0f);

  modEnvelope.attack(180.0f);
  modEnvelope.decay(120.0f);
  modEnvelope.sustain(0.4f);
  modEnvelope.release(240.0f);

  modulator01.begin(1.0f, 75.0f, WAVEFORM_SINE);
  modulator02.begin(1.0f, 125.0f, WAVEFORM_SINE);
  modulator03.begin(1.0f, 250.0f, WAVEFORM_SINE);
  
  carrier.begin(0.6, 200.0f, WAVEFORM_SINE);

  pinMode(killswitchButtonPin, INPUT_PULLUP);
}

void loop() {
  envelopeButton.update();
  if(envelopeButton.fallingEdge()) {
    envelope.noteOn();
    modEnvelope.noteOn();
  } else if(envelopeButton.risingEdge()) {
    envelope.noteOff();
    modEnvelope.noteOff();
  }
  
  float frequencyValueA = analogRead(frequencyPot);
  float frequencyValueB = frequencyValueA * 0.501f;
  float frequencyValueC = frequencyValueA * 2.02f;
  modulator01.frequency(frequencyValueA);
  modulator02.frequency(frequencyValueB);
  modulator03.frequency(frequencyValueC);
  
  float modulationIndex = analogRead(modulationIndexPot) / 1023.0f;
  mixer.gain(0, modulationIndex);
  mixer.gain(1, modulationIndex);
  mixer.gain(2, modulationIndex);
     
  float amplitude = analogRead(A1) / 1023.0f;
  killswitchButton.update();
  if(killswitchButton.fallingEdge()) {
    amplitude = 0.0f;
  }
  control.volume(amplitude);
}
