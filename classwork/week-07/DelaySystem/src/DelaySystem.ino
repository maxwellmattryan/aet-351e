#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

#include <Bounce.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

AudioInputI2S            input;           //xy=55,327.5156373977661
AudioEffectDelay         delayL;         //xy=238.51562881469727,240.51565551757812
AudioEffectDelay         delayR; //xy=238.515625,573.5156517028809
AudioMixer4              mixerL;         //xy=240.5156135559082,340.51562881469727
AudioMixer4              mixerR; //xy=239.51563262939453,672.515682220459
AudioSynthWaveform       oscL01; //xy=420.515625,126.51562213897705
AudioEffectMultiply      multiplyL01;      //xy=426.51562881469727,189.51565289497375
AudioSynthWaveform       oscR01;      //xy=430.51562118530273,471.5156526565552
AudioEffectMultiply      multiplyR01; //xy=439.51561737060547,522.5156841278076
AudioOutputI2S           output;           //xy=452.5156669616699,347.51562309265137
AudioSynthWaveform       oscR02; //xy=615.9999961853027,435.9999692440033
AudioEffectMultiply      multiplyR02; //xy=622,499
AudioSynthWaveform       oscL02; //xy=630.5156211853027,88.5155942440033
AudioEffectMultiply      multiplyL02; //xy=636.515625,151.515625
AudioConnection          patchCord1(input, 0, mixerL, 0);
AudioConnection          patchCord2(input, 1, mixerR, 0);
AudioConnection          patchCord3(delayL, 0, multiplyL01, 1);
AudioConnection          patchCord4(delayL, 1, multiplyL02, 1);
AudioConnection          patchCord5(delayR, 0, multiplyR01, 1);
AudioConnection          patchCord6(delayR, 1, multiplyR02, 1);
AudioConnection          patchCord7(mixerL, delayL);
AudioConnection          patchCord8(mixerL, 0, output, 0);
AudioConnection          patchCord9(mixerR, delayR);
AudioConnection          patchCord10(mixerR, 0, output, 1);
AudioConnection          patchCord11(oscL01, 0, multiplyL01, 0);
AudioConnection          patchCord12(multiplyL01, 0, mixerL, 1);
AudioConnection          patchCord13(oscR01, 0, multiplyR01, 0);
AudioConnection          patchCord14(multiplyR01, 0, mixerR, 1);
AudioConnection          patchCord15(oscR02, 0, multiplyR02, 0);
AudioConnection          patchCord16(multiplyR02, 0, mixerL, 2);
AudioConnection          patchCord17(oscL02, 0, multiplyL02, 0);
AudioConnection          patchCord18(multiplyL02, 0, mixerR, 2);
AudioControlSGTL5000     control;     //xy=59.666664123535156,26.66666603088379

const int AUDIO_MEMORY = 192;

const int FEEDBACK_POT = A3;
const int OSC_FREQUENCY_POT = A2;

const int MIXER_MIC_IN = 0;
const int MIXER_MULTIPLY_LEFT_IN = 1;
const int MIXER_MULTIPLY_RIGHT_IN = 2;

const int DELAY_TAP_01 = 0;
const int DELAY_TAP_02 = 1;

const float OSC_MAX_FREQUENCY = 1800.0f;
const float OSC_OFFSET_MULTIPLIER = 0.25f;

void setup() {
    Serial.begin(9600);
    
    initAudio();
    initSdCard();
    
    delay(1000);
    
    initControls();
    initComponents();
}

void initAudio() {
    AudioMemory(AUDIO_MEMORY);
    
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

    mixerL.gain(MIXER_MIC_IN, 0.5f);
    mixerR.gain(MIXER_MIC_IN, 0.5f);

    delayL.delay(DELAY_TAP_01, 90.0f);
    delayL.delay(DELAY_TAP_02, 30.0f);
    delayR.delay(DELAY_TAP_01, 120.0f);
    delayR.delay(DELAY_TAP_02, 60.0f);

    oscL01.begin(1.0f, 240.0f, WAVEFORM_SINE);
    oscL02.begin(1.0f, 240.0f, WAVEFORM_SINE);
    oscR01.begin(1.0f, 320.0f, WAVEFORM_SINE);
    oscR02.begin(1.0f, 320.0f, WAVEFORM_SINE);
}

void loop() {
    float feedbackIn = analogRead(FEEDBACK_POT) / 1023.0f;
    mixerL.gain(MIXER_MULTIPLY_LEFT_IN, feedbackIn);
    mixerL.gain(MIXER_MULTIPLY_RIGHT_IN, feedbackIn);
    mixerR.gain(MIXER_MULTIPLY_LEFT_IN, feedbackIn);
    mixerR.gain(MIXER_MULTIPLY_RIGHT_IN, feedbackIn);

    float frequencyIn = OSC_MAX_FREQUENCY * analogRead(OSC_FREQUENCY_POT) / 1023.0f;
    oscL01.frequency(frequencyIn);
    oscL02.frequency(frequencyIn);
    oscR01.frequency(frequencyIn * OSC_OFFSET_MULTIPLIER);
    oscR02.frequency(frequencyIn * OSC_OFFSET_MULTIPLIER);

    float amplitude = analogRead(A1) / 1023.0f;
    control.volume(amplitude);
}
