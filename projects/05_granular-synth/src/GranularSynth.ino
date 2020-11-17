#include <Audio.h>
#include <Wire.h>
#include <Bounce.h>

#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

AudioPlaySdWav           sampler;     
AudioEffectGranular      granular01;      
AudioEffectGranular      granular02;
AudioEffectGranular      granular03;
AudioEffectGranular      granular04; 
AudioEffectGranular      granular05;
AudioEffectGranular      granular06; 
AudioMixer4              mixerL;        
AudioMixer4              mixerR; 
AudioEffectDigitalCombine combineL;       
AudioEffectDigitalCombine combineR; 
AudioSynthWaveformDc     dc;           
AudioEffectEnvelope      ampEgL;     
AudioEffectEnvelope      ampEgR;
AudioEffectEnvelope      modEg; 
AudioFilterStateVariable filterL;       
AudioFilterStateVariable filterR; 
AudioOutputI2S           output;          
AudioConnection          patchCord1(sampler, 0, granular01, 0);
AudioConnection          patchCord2(sampler, 0, granular02, 0);
AudioConnection          patchCord3(sampler, 0, granular03, 0);
AudioConnection          patchCord4(sampler, 1, granular04, 0);
AudioConnection          patchCord5(sampler, 1, granular05, 0);
AudioConnection          patchCord6(sampler, 1, granular06, 0);
AudioConnection          patchCord7(granular01, 0, mixerL, 0);
AudioConnection          patchCord8(granular03, 0, mixerL, 2);
AudioConnection          patchCord9(granular02, 0, mixerL, 1);
AudioConnection          patchCord10(granular04, 0, mixerR, 0);
AudioConnection          patchCord11(granular06, 0, mixerR, 2);
AudioConnection          patchCord12(granular05, 0, mixerR, 1);
AudioConnection          patchCord13(mixerL, 0, combineL, 0);
AudioConnection          patchCord14(mixerL, 0, combineR, 0);
AudioConnection          patchCord15(mixerR, 0, combineL, 1);
AudioConnection          patchCord16(mixerR, 0, combineR, 1);
AudioConnection          patchCord17(combineL, ampEgL);
AudioConnection          patchCord18(combineR, ampEgR);
AudioConnection          patchCord19(dc, modEg);
AudioConnection          patchCord20(ampEgL, 0, filterL, 0);
AudioConnection          patchCord21(ampEgR, 0, filterR, 0);
AudioConnection          patchCord22(modEg, 0, filterL, 1);
AudioConnection          patchCord23(modEg, 0, filterR, 1);
AudioConnection          patchCord24(filterL, 0, output, 0);
AudioConnection          patchCord25(filterR, 0, output, 1);
AudioControlSGTL5000     control;

const int TEMPO_POT = A2;
const int TEMPO_MAX = 520;
const int TEMPO_MIN = 30;
const int SEQUENCER_BUTTON = 0;
Bounce sequencerStartButton = Bounce(SEQUENCER_BUTTON, 20);
const int SEQUENCER_SWITCH_BUTTON = 1;
Bounce sequencerSwitchButton = Bounce(SEQUENCER_SWITCH_BUTTON, 20);
bool isSequencerButtonDown = false;
bool isInSequencerMode = true;
bool isSequencerActive = false;
int sequencerStartTime = 0;

const char* GRAIN_SAMPLE = "age.wav";
const int GRAIN_PITCH_POT = A6;
const int GRAIN_SIZE_POT = A3;
const int GRAIN_RESET_BUTTON = 3;
Bounce grainResetButton = Bounce(GRAIN_RESET_BUTTON, 20);
const int BASE_GRAIN_SIZE = 6;
const int NUM_SAMPLES = 14700;
int16_t granularMemory[NUM_SAMPLES];

const int FILTER_CUTOFF_POT = A7;
const float FILTER_CUTOFF_MIN_FREQUENCY = 18.0f;
const float FILTER_CUTOFF_MAX_FREQUENCY = 600.0f;

int combineModeIndex = (int) AudioEffectDigitalCombine::AND;
const int COMBINE_MODE_BUTTON = 2;
Bounce combineModeButton = Bounce(COMBINE_MODE_BUTTON, 20);

void setup() {
    Serial.begin(9600);
    
    initAudio();
    initSdCard();
    
    delay(1000);
    
    initControls();
    initComponents();
}

void initAudio() {
    granular01.begin(granularMemory, NUM_SAMPLES);
    granular02.begin(granularMemory, NUM_SAMPLES);
    granular03.begin(granularMemory, NUM_SAMPLES);
    granular04.begin(granularMemory, NUM_SAMPLES);
    granular05.begin(granularMemory, NUM_SAMPLES);
    granular06.begin(granularMemory, NUM_SAMPLES);
       
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
    pinMode(SEQUENCER_BUTTON, INPUT_PULLUP);
    pinMode(SEQUENCER_SWITCH_BUTTON, INPUT_PULLUP);
    pinMode(GRAIN_RESET_BUTTON, INPUT_PULLUP);
    pinMode(COMBINE_MODE_BUTTON, INPUT_PULLUP);
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

    combineL.setCombineMode(combineModeIndex);
    combineR.setCombineMode(combineModeIndex);
    
    ampEgL.attack(4.0f);
    ampEgL.decay(60.0f);
    ampEgL.sustain(0.4f);
    ampEgL.release(540.0f);
    
    ampEgR.attack(4.0f);
    ampEgR.decay(60.0f);
    ampEgR.sustain(0.4f);
    ampEgR.release(540.0f);

    dc.amplitude(1.0f);

    modEg.attack(6.0f);
    modEg.decay(70.0f);
    modEg.sustain(0.4f);
    modEg.release(540.0f);

    filterL.frequency(500.0f);
    filterL.resonance(1.8f);
    filterL.octaveControl(5);
    
    filterR.frequency(500.0f);
    filterR.resonance(1.8f);
    filterR.octaveControl(5);
}

void loop() {
    if (!sampler.isPlaying()) {
        sampler.play(GRAIN_SAMPLE);
        delay(10);
    }

    updateGrains();
    updateCombiner();
    updateFilter();
    updateAmplitude();
    updateSequencer();
}

void updateGrains() {
    float pitch = (analogRead(GRAIN_PITCH_POT) / 1023.0f) * 3.0f;
    granular01.setSpeed(pitch);
    granular02.setSpeed(pitch * 0.5f);
    granular03.setSpeed(pitch * 2.0f);
    granular04.setSpeed(pitch);
    granular06.setSpeed(pitch * 2.0f);
    
    grainResetButton.update();
    if(grainResetButton.fallingEdge()) {
        float grainSizeIn = analogRead(GRAIN_SIZE_POT) / 1023.0f;

        granular01.beginFreeze(grainSizeIn * (BASE_GRAIN_SIZE * 1.0f));
        granular02.beginFreeze(grainSizeIn * (BASE_GRAIN_SIZE * 2.0f));
        granular03.beginFreeze(grainSizeIn * (BASE_GRAIN_SIZE * 4.0f));
        granular04.beginFreeze(grainSizeIn * (BASE_GRAIN_SIZE * 8.0f));
        granular05.beginFreeze(grainSizeIn * (BASE_GRAIN_SIZE * 16.0f));
        granular06.beginFreeze(grainSizeIn * (BASE_GRAIN_SIZE * 32.0f));
    }
}

void updateCombiner() {
    combineModeButton.update();
    if(combineModeButton.fallingEdge()) {
        combineModeIndex += 1;
        combineModeIndex %= 4;
        
        combineL.setCombineMode(combineModeIndex);
        combineR.setCombineMode(combineModeIndex);
    }
}

void updateFilter() {
    float filterCutoffFrequency = calculateFilterCutoffFrequency();
    filterL.frequency(filterCutoffFrequency);
    filterR.frequency(filterCutoffFrequency);
}

float calculateFilterCutoffFrequency() {
    float pot = analogRead(FILTER_CUTOFF_POT) / 1023.0f;
    dc.amplitude((pot * 0.25f) + 0.75f);
    return pot * (FILTER_CUTOFF_MAX_FREQUENCY - FILTER_CUTOFF_MIN_FREQUENCY) + FILTER_CUTOFF_MIN_FREQUENCY;
}

void updateAmplitude() {
    float amplitude = analogRead(A1) / 1023.0f;
    control.volume(amplitude);
}

void updateSequencer() {
    updateSequencerButtons();
    updateSequencerSwitch();
    if(isInSequencerMode) {
        updateSequencerTiming();
    } else {
        updateEnvelopes();
    }
}

void updateSequencerSwitch() {
    sequencerSwitchButton.update();
    if(sequencerSwitchButton.fallingEdge()) {
        ampEgL.noteOff();
        ampEgR.noteOff();
        modEg.noteOff();

        isSequencerActive = false;
        isInSequencerMode = !isInSequencerMode;
    }
}

void updateSequencerButtons() {
    sequencerStartButton.update();
    if(sequencerStartButton.fallingEdge()) { 
        isSequencerActive = !isSequencerActive;
        if(isSequencerActive) {
            ampEgL.noteOn();
            ampEgR.noteOn();
            modEg.noteOn();
            
            sequencerStartTime = millis();
        } else {
            ampEgL.noteOff();
            ampEgR.noteOff();
            modEg.noteOff();
        }
    }
}

void updateSequencerTiming() {
    if(isSequencerActive) {
        float sequenceTime = calculateSequenceTime();
        ampEgL.release(sequenceTime);
        ampEgR.release(sequenceTime);
        
        if(millis() - sequencerStartTime > sequenceTime) {
            ampEgL.noteOff();
            ampEgR.noteOff();
            modEg.noteOff();
            
            sequencerStartTime = millis();

            ampEgL.noteOn();
            ampEgR.noteOn();
            modEg.noteOn();
        }
    }
}

void updateEnvelopes() {
    if(sequencerStartButton.fallingEdge()) {
        ampEgL.noteOn();
        ampEgR.noteOn();
        modEg.noteOn();
    } else if(sequencerStartButton.risingEdge()) {
        ampEgL.noteOff();
        ampEgR.noteOff();
        modEg.noteOff();
    }
}

float calculateSequenceTime() {
    int tempo = (analogRead(TEMPO_POT) / 1023.0f) * (TEMPO_MAX - TEMPO_MIN) + TEMPO_MIN;
    return (float) (60000 / (tempo * 4));
}
