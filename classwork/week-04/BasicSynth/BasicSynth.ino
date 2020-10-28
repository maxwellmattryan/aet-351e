#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>

// Audio components
AudioSynthWaveformDc     dc01;           
AudioSynthWaveform       waveform02; 
AudioSynthWaveform       waveform01;     
AudioMixer4              mixer01;        
AudioEffectEnvelope      envelope02;    
AudioFilterStateVariable filter01;       
AudioEffectEnvelope      envelope01;      
AudioOutputI2S           output;           
AudioControlSGTL5000     control;        

// Patches and connections
AudioConnection          patchCord1(dc01, envelope02);
AudioConnection          patchCord2(waveform02, 0, mixer01, 1);
AudioConnection          patchCord3(waveform01, 0, mixer01, 0);
AudioConnection          patchCord4(mixer01, 0, filter01, 0);
AudioConnection          patchCord5(envelope02, 0, filter01, 1);
AudioConnection          patchCord6(filter01, 0, envelope01, 0);
AudioConnection          patchCord7(envelope01, 0, output, 0);
AudioConnection          patchCord8(envelope01, 0, output, 1);

// External controls
int pot01 = A2;
int pot02 = A3;
Bounce button01 = Bounce(8, 40);

// Other parameters
int octave = 1;
int octaveRandom;

// Pre-processor definitions for SD card
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

void setup() {
  Serial.begin(9600);

  initPins();
  initAudioComponents();
  initSDCard();
}

void initPins() {
  pinMode(8, INPUT_PULLUP);
}

void initAudioComponents() {
  AudioMemory(8);
  
  control.enable();
  control.volume(0.8f);

  waveform01.begin(0.6, 50, WAVEFORM_SQUARE);
  waveform02.begin(0.6, 50, WAVEFORM_TRIANGLE);

  filter01.frequency(100.0f);
  filter01.resonance(1.6f);
  filter01.octaveControl(6);

  envelope01.attack(25);
  envelope01.decay(40);
  envelope01.sustain(0.7f);
  envelope01.release(1000);

  dc01.amplitude(1.0f);

  envelope02.attack(25);
  envelope02.decay(40);
  envelope02.sustain(0.7f);
  envelope02.release(1000);

  mixer01.gain(0, 0.5f);
  mixer01.gain(1, 0.5f);
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
  int pot01Value = analogRead(pot01) * octave;
  int pot02Value = analogRead(pot02) * octave;
  waveform01.frequency(pot01Value);
  waveform02.frequency(pot02Value);

  button01.update();
  if(button01.fallingEdge()) {
    octaveRandom = random(100);
    octave = octaveRandom >= 90 ? 2 : 1;
    
    envelope01.noteOn();
    envelope02.noteOn();
  }
  if(button01.risingEdge()) {
    envelope01.noteOff();
    envelope02.noteOff();
  }
  
  float ampValue = analogRead(A1) / 1023.0f;
  control.volume(ampValue);
}
