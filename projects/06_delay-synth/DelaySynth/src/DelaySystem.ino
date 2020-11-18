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
AudioSynthWaveform       osc;      //xy=95,345
AudioFilterStateVariable filter;        //xy=267,351
AudioEffectEnvelope      ampEg;      //xy=461,353
AudioEffectDelay         delayL;         //xy=683,259
AudioEffectDelay         delayR;         //xy=683,592
AudioMixer4              mixerL;         //xy=685,359
AudioMixer4              mixerR;         //xy=684,691
AudioSynthWaveform       oscL01;         //xy=865,145
AudioEffectMultiply      multiplyL01;    //xy=871,208
AudioSynthWaveform       oscR01;         //xy=875,490
AudioEffectMultiply      multiplyR01;    //xy=884,541
AudioOutputI2S           output;         //xy=897,366
AudioSynthWaveform       oscR02;         //xy=1060,454
AudioEffectMultiply      multiplyR02;    //xy=1067,518
AudioSynthWaveform       oscL02;         //xy=1075,107
AudioEffectMultiply      multiplyL02;    //xy=1081,170
AudioConnection          patchCord1(osc, 0, filter, 0);
AudioConnection          patchCord2(filter, 0, ampEg, 0);
AudioConnection          patchCord3(ampEg, 0, mixerL, 0);
AudioConnection          patchCord4(ampEg, 0, mixerR, 0);
AudioConnection          patchCord5(delayL, 0, multiplyL01, 1);
AudioConnection          patchCord6(delayL, 1, multiplyL02, 1);
AudioConnection          patchCord7(delayR, 0, multiplyR01, 1);
AudioConnection          patchCord8(delayR, 1, multiplyR02, 1);
AudioConnection          patchCord9(mixerL, delayL);
AudioConnection          patchCord10(mixerL, 0, output, 0);
AudioConnection          patchCord11(mixerR, delayR);
AudioConnection          patchCord12(mixerR, 0, output, 1);
AudioConnection          patchCord13(oscL01, 0, multiplyL01, 0);
AudioConnection          patchCord14(multiplyL01, 0, mixerL, 1);
AudioConnection          patchCord15(oscR01, 0, multiplyR01, 0);
AudioConnection          patchCord16(multiplyR01, 0, mixerR, 1);
AudioConnection          patchCord17(oscR02, 0, multiplyR02, 0);
AudioConnection          patchCord18(multiplyR02, 0, mixerL, 2);
AudioConnection          patchCord19(oscL02, 0, multiplyL02, 0);
AudioConnection          patchCord20(multiplyL02, 0, mixerR, 2);
AudioControlSGTL5000     control;        //xy=83,48
// GUItool: end automatically generated code

const int AUDIO_MEMORY = 192;

const int AMP_EG_BUTTON_PIN = 0;
Bounce ampEgButton = Bounce(AMP_EG_BUTTON_PIN, 20);

const int OSC_FREQUENCY_POT = A6;
const int FILTER_CUTOFF_POT = A7;
const int FEEDBACK_POT = A3;
const int FEEDBACK_OSC_FREQUENCY_POT = A2;

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
    //initSdCard();
    
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
    pinMode(AMP_EG_BUTTON_PIN, INPUT_PULLUP);
}

void initComponents() {
    osc.begin(1.0f, 100.0f, WAVEFORM_SAWTOOTH);
    

    filter.frequency(240.0f);
    filter.resonance(1.4f);

    ampEg.attack(10.0f);
    ampEg.decay(140.0f);
    ampEg.sustain(0.4f);
    ampEg.release(240.0f);

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
    float oscFrequencyIn = analogRead(OSC_FREQUENCY_POT) / 1023.0f * 3200.0f;
    osc.frequency(oscFrequencyIn);

    float filterCutoffIn = analogRead(FILTER_CUTOFF_POT) / 1023.0f * 6400.0f;
    filter.frequency(filterCutoffIn);

    float feedbackIn = analogRead(FEEDBACK_POT) / 1023.0f;
    mixerL.gain(MIXER_MULTIPLY_LEFT_IN, feedbackIn);
    mixerL.gain(MIXER_MULTIPLY_RIGHT_IN, feedbackIn);
    mixerR.gain(MIXER_MULTIPLY_LEFT_IN, feedbackIn);
    mixerR.gain(MIXER_MULTIPLY_RIGHT_IN, feedbackIn);

    float frequencyIn = OSC_MAX_FREQUENCY * analogRead(FEEDBACK_OSC_FREQUENCY_POT) / 1023.0f;
    oscL01.frequency(frequencyIn);
    oscL02.frequency(frequencyIn);
    oscR01.frequency(frequencyIn * OSC_OFFSET_MULTIPLIER);
    oscR02.frequency(frequencyIn * OSC_OFFSET_MULTIPLIER);

    ampEgButton.update();
    if(ampEgButton.fallingEdge()) {
        Serial.println("NOTE ON");
        ampEg.noteOn();
    } else if(ampEgButton.risingEdge()) {
        Serial.println("NOTE OFF");
        ampEg.noteOff();
    }

    float amplitude = analogRead(A1) / 1023.0f;
    control.volume(amplitude);
}
