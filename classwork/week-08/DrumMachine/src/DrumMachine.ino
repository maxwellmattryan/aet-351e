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
AudioSynthNoiseWhite     clap;         //xy=89,707
AudioSynthWaveformDc     snareDc;            //xy=91.66665649414062,449.6666564941406
AudioSynthWaveform       cowbellOsc02; //xy=109,578
AudioSynthWaveform       cowbellOsc01;      //xy=112,518
AudioSynthNoiseWhite     snareNoise;         //xy=156.66665649414062,307.66668033599854
AudioSynthWaveform       snareOsc01;      //xy=158.66665649414062,186.6666717529297
AudioSynthWaveform       snareOsc02; //xy=158.6666717529297,253.66667938232422
AudioSynthSimpleDrum     kick;          //xy=162.66665649414062,78.66666793823242
AudioEffectEnvelope      snareFilterModEg;      //xy=161.66665649414062,358.6666679382324
AudioFilterStateVariable clapFilter;        //xy=238,715
AudioMixer4              cowbellMixer;         //xy=320,526
AudioFilterStateVariable snareFilter01;        //xy=335.6666564941406,314.6666669845581
AudioMixer4              snareOscMixer;         //xy=371.6666564941406,206.6666660308838
AudioEffectEnvelope      clapAmpEg03; //xy=431,735
AudioEffectEnvelope      clapAmpEg04; //xy=432,787
AudioEffectEnvelope      clapAmpEg02; //xy=433,684
AudioEffectEnvelope      clapAmpEg01;      //xy=434,636
AudioFilterStateVariable snareFilter02; //xy=526.6666641235352,308.66668033599854
AudioEffectEnvelope      cowbellAmpEg;      //xy=541,540
AudioEffectEnvelope      snareOscAmpEg;      //xy=566,197
AudioMixer4              clapMixer;         //xy=711,710
AudioEffectEnvelope      snareFilterAmpEg;      //xy=734,308
AudioMixer4              snareMixer;         //xy=804,204
AudioMixer4              mixer;         //xy=1005.6666870117188,96.66666412353516
AudioOutputI2S           output;           //xy=1162.666732788086,97.66666603088379
AudioConnection          patchCord1(clap, 0, clapFilter, 0);
AudioConnection          patchCord2(snareDc, snareFilterModEg);
AudioConnection          patchCord3(cowbellOsc02, 0, cowbellMixer, 1);
AudioConnection          patchCord4(cowbellOsc01, 0, cowbellMixer, 0);
AudioConnection          patchCord5(snareNoise, 0, snareFilter01, 0);
AudioConnection          patchCord6(snareOsc01, 0, snareOscMixer, 0);
AudioConnection          patchCord7(snareOsc02, 0, snareOscMixer, 1);
AudioConnection          patchCord8(kick, 0, mixer, 0);
AudioConnection          patchCord9(snareFilterModEg, 0, snareFilter01, 1);
AudioConnection          patchCord10(clapFilter, 1, clapAmpEg01, 0);
AudioConnection          patchCord11(clapFilter, 1, clapAmpEg02, 0);
AudioConnection          patchCord12(clapFilter, 1, clapAmpEg03, 0);
AudioConnection          patchCord13(clapFilter, 1, clapAmpEg04, 0);
AudioConnection          patchCord14(cowbellMixer, cowbellAmpEg);
AudioConnection          patchCord15(snareFilter01, 0, snareFilter02, 0);
AudioConnection          patchCord16(snareOscMixer, snareOscAmpEg);
AudioConnection          patchCord17(clapAmpEg03, 0, clapMixer, 2);
AudioConnection          patchCord18(clapAmpEg04, 0, clapMixer, 3);
AudioConnection          patchCord19(clapAmpEg02, 0, clapMixer, 1);
AudioConnection          patchCord20(clapAmpEg01, 0, clapMixer, 0);
AudioConnection          patchCord21(snareFilter02, 2, snareFilterAmpEg, 0);
AudioConnection          patchCord22(cowbellAmpEg, 0, mixer, 2);
AudioConnection          patchCord23(snareOscAmpEg, 0, snareMixer, 0);
AudioConnection          patchCord24(clapMixer, 0, mixer, 3);
AudioConnection          patchCord25(snareFilterAmpEg, 0, snareMixer, 1);
AudioConnection          patchCord26(snareMixer, 0, mixer, 1);
AudioConnection          patchCord27(mixer, 0, output, 0);
AudioConnection          patchCord28(mixer, 0, output, 1);
AudioControlSGTL5000     control;     //xy=59.666664123535156,26.66666603088379
// GUItool: end automatically generated code

const unsigned long BOUNCE_TIME = 20;

const int MIXER_IN_KICK = 0;
const int BUTTON_PIN_KICK = 0;
Bounce kickButton = Bounce(BUTTON_PIN_KICK, BOUNCE_TIME);
const int MIXER_IN_SNARE = 1;
const int BUTTON_PIN_SNARE = 1;
Bounce snareButton = Bounce(BUTTON_PIN_SNARE, BOUNCE_TIME);
const int MIXER_IN_COWBELL = 2;
const int BUTTON_PIN_COWBELL = 2;
Bounce cowbellButton = Bounce(BUTTON_PIN_COWBELL, BOUNCE_TIME);
const int MIXER_IN_CLAP = 3;
const int BUTTON_PIN_CLAP = 3;
Bounce clapButton = Bounce(BUTTON_PIN_CLAP, BOUNCE_TIME);

void setup() {
    Serial.begin(9600);
    
    initAudio();
    initSdCard();
    
    delay(1000);
    
    initControls();
    initComponents();
}

void initAudio() {
    AudioMemory(16);
    
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
    pinMode(BUTTON_PIN_KICK, INPUT_PULLUP);
    pinMode(BUTTON_PIN_SNARE, INPUT_PULLUP);
    pinMode(BUTTON_PIN_COWBELL, INPUT_PULLUP);
    pinMode(BUTTON_PIN_CLAP, INPUT_PULLUP);
}

void initComponents() {
    // MIXER
    mixer.gain(MIXER_IN_KICK, 1.0f);
    mixer.gain(MIXER_IN_SNARE, 0.1f);
    mixer.gain(MIXER_IN_COWBELL, 0.1f);
    mixer.gain(MIXER_IN_CLAP, 0.2f);

    // KICK
    kick.frequency(42.0f);
    kick.length(200);
    kick.pitchMod(1.0f);

    // SNARE
    snareOsc01.begin(0.1f, 275.0f, WAVEFORM_TRIANGLE);
    snareOsc02.begin(0.2f, 164.0f, WAVEFORM_TRIANGLE);

    snareNoise.amplitude(0.5f);

    snareFilter01.frequency(3000.0f);
    snareFilter01.resonance(2.0f);
    snareFilter01.octaveControl(1.0f);

    snareFilter02.frequency(540.0f);

    snareFilterModEg.delay(5.0f);
    snareFilterModEg.attack(0.0f);
    snareFilterModEg.hold(5.0f);
    snareFilterModEg.decay(200.0f);
    snareFilterModEg.sustain(0.0f);
    snareFilterModEg.release(0.0f);

    snareFilterAmpEg.delay(5.0f);
    snareFilterAmpEg.attack(0.0f);
    snareFilterAmpEg.hold(10.0f);
    snareFilterAmpEg.decay(230.0f);
    snareFilterAmpEg.sustain(0.0f);
    snareFilterAmpEg.release(0.0f);

    snareOscAmpEg.attack(0.0f);
    snareOscAmpEg.hold(5.0f);
    snareOscAmpEg.decay(70.0f);
    snareOscAmpEg.sustain(0.0f);
    snareOscAmpEg.release(0.0f);

    snareDc.amplitude(1.0f);

    // COWBELL
    cowbellOsc01.begin(0.5f, 800.0f, WAVEFORM_SQUARE);
    cowbellOsc02.begin(0.5f, 540.0f, WAVEFORM_SQUARE);

    cowbellAmpEg.attack(0.0f);
    cowbellAmpEg.decay(320.0f);
    cowbellAmpEg.sustain(0.0f);
    cowbellAmpEg.release(0.0f);

    // CLAP
    clap.amplitude(0.5f);

    clapFilter.frequency(1000.0f);
    clapFilter.resonance(2.4f);

    clapAmpEg01.attack(0.0f);
    clapAmpEg01.decay(10.0f);
    clapAmpEg01.sustain(0.0f);
    clapAmpEg01.release(0.0f);

    clapAmpEg02.delay(10.0f);
    clapAmpEg02.attack(0.0f);
    clapAmpEg02.decay(10.0f);
    clapAmpEg02.sustain(0.0f);
    clapAmpEg02.release(0.0f);

    clapAmpEg03.delay(20.0f);
    clapAmpEg03.attack(0.0f);
    clapAmpEg03.decay(10.0f);
    clapAmpEg03.sustain(0.0f);
    clapAmpEg03.release(0.0f);

    clapAmpEg04.delay(10.0f);
    clapAmpEg04.attack(0.0f);
    clapAmpEg04.decay(240.0f);
    clapAmpEg04.sustain(0.0f);
    clapAmpEg04.release(0.0f);
}

void loop() {
    kickButton.update();
    if(kickButton.fallingEdge()) {
        kick.noteOn();
    }

    snareButton.update();
    if(snareButton.fallingEdge()) {
        snareFilterModEg.noteOff();
        snareFilterAmpEg.noteOff();
        snareOscAmpEg.noteOff();

        snareFilterModEg.noteOn();
        snareFilterAmpEg.noteOn();
        snareOscAmpEg.noteOn();
    }
    
    cowbellButton.update();
    if(cowbellButton.fallingEdge()) {
        cowbellAmpEg.noteOff();
        cowbellAmpEg.noteOn();
    }

    clapButton.update();
    if(clapButton.fallingEdge()) {
        clapAmpEg01.noteOff();
        clapAmpEg02.noteOff();
        clapAmpEg03.noteOff();
        clapAmpEg04.noteOff();

        clapAmpEg01.noteOn();
        clapAmpEg02.noteOn();
        clapAmpEg03.noteOn();
        clapAmpEg04.noteOn();
    }

    float amplitude = analogRead(A1) / 1023.0f;
    control.volume(amplitude);
}
