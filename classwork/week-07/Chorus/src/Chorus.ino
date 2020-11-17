#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

#include <Bounce.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

AudioInputI2S            input;           //xy=57.515625,86.51562309265137
AudioEffectChorus        chorus;        //xy=220.51562118530273,239.51562309265137
AudioMixer4              mixer;         //xy=221.5156135559082,99.51563835144043
AudioOutputI2S           output;           //xy=411.51562118530273,105.51562213897705
AudioConnection          patchCord1(input, 0, mixer, 0);
AudioConnection          patchCord2(chorus, 0, mixer, 1);
AudioConnection          patchCord3(mixer, chorus);
AudioConnection          patchCord4(mixer, 0, output, 0);
AudioConnection          patchCord5(mixer, 0, output, 1);
AudioControlSGTL5000     control;     //xy=59.666664123535156,26.66666603088379

// Configuring delay for the chorus in samples
#define CHORUS_DELAY 3000

// Declaring array of 16-bit integers
short delayBuffer[CHORUS_DELAY];

const int FEEDBACK_POT = A3;
const int POT_B = A2;

const int MIXER_SAMPLER_IN = 0;
const int MIXER_CHORUS_IN = 1;

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

}

void initComponents() {
    control.inputSelect(AUDIO_INPUT_MIC);
    control.micGain(48);

    chorus.begin(delayBuffer, CHORUS_DELAY, 5);

    mixer.gain(MIXER_SAMPLER_IN, 0.5f);
    mixer.gain(MIXER_CHORUS_IN, 0.5f);
}

void loop() {
    float feedbackIn = analogRead(FEEDBACK_POT) / 1023.0f;
    mixer.gain(MIXER_CHORUS_IN, feedbackIn);

    float amplitude = analogRead(A1) / 1023.0f;
    control.volume(amplitude);
}
