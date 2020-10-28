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
AudioPlaySdWav           sampler;     //xy=69,76.75000047683716
AudioSynthWaveformDc     dc;            //xy=78.66665649414062,154.6666660308838
AudioEffectEnvelope      modEg;      //xy=221.66667556762695,154.66667556762695
AudioEffectGranular      granular;      //xy=230.75,79.75
AudioFilterStateVariable filter;        //xy=396.6666679382324,81.66666603088379
AudioEffectEnvelope      ampEg;      //xy=545.6667022705078,72.66666221618652
AudioOutputI2S           output;           //xy=697.7501602172852,68.74999904632568
AudioConnection          patchCord1(sampler, 0, granular, 0);
AudioConnection          patchCord2(dc, modEg);
AudioConnection          patchCord3(modEg, 0, filter, 1);
AudioConnection          patchCord4(granular, 0, filter, 0);
AudioConnection          patchCord5(filter, 0, ampEg, 0);
AudioConnection          patchCord6(ampEg, 0, output, 0);
AudioConnection          patchCord7(ampEg, 0, output, 1);
AudioControlSGTL5000     control;     //xy=55,20
// GUItool: end automatically generated code

int numSamples = 14700;
int16_t granularMemory[14700]; // define granular memory

int timePot = A3;
int pitchPot = A2;

int buttonAPin = 0;
Bounce buttonA = Bounce(buttonAPin, 15);
int buttonBPin = 1;
Bounce buttonB = Bounce(buttonBPin, 15);

void setup() {
  Serial.begin(9600);
  
  initAudio();
  initSdCard();
  
  delay(1000);

  initControls();
  initComponents();
}

void initAudio() {
  granular.begin(granularMemory, numSamples);
       
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
  pinMode(buttonAPin, INPUT_PULLUP);
  pinMode(buttonBPin, INPUT_PULLUP);
}

void initComponents() {
  dc.amplitude(1.0f);

  ampEg.attack(20.0f);
  ampEg.decay(180.0f);
  ampEg.sustain(0.4f);
  ampEg.release(1200.0f);

  modEg.attack(30.0f);
  modEg.decay(240.0f);
  modEg.sustain(0.6f);
  modEg.release(1200.0f);

  filter.frequency(200.0f);
  filter.resonance(1.4f);
  filter.octaveControl(3);
  
  granular.beginFreeze(100);
}

void loop() {
  if (sampler.isPlaying() == false) {
    sampler.play("age.wav");
    delay(10);
  }
   
  buttonA.update();
  if(buttonA.fallingEdge()) {
    float timeValue = analogRead(timePot) / 1023.0f;
    float milliseconds = (timeValue * 250.0f) + 1.0f;
  
    granular.beginFreeze(milliseconds); 
  }

  float pitchValue = analogRead(pitchPot) / 1023.0f;
  granular.setSpeed(pitchValue * 2.0f);

  buttonB.update();
  if(buttonB.fallingEdge()) {
    ampEg.noteOn();
    modEg.noteOn();
  } else if(buttonB.risingEdge()) {
    ampEg.noteOff();
    modEg.noteOff();
  }

  float amplitude = analogRead(A1) / 1023.0f;
  control.volume(amplitude);
}
