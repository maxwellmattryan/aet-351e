#include <NewPing.h>
#include <movingAvg.h>

#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       modOsc;      //xy=69.66666030883789,82.6666669845581
AudioSynthWaveformModulated carrierOsc;   //xy=251.66667938232422,88.66666603088379
AudioEffectEnvelope      ampEg;      //xy=438.6666679382324,89.66666603088379
AudioEffectDelay         delay1;         //xy=578.6667022705078,217.66667556762695
AudioOutputI2S           output;           //xy=722.6667022705078,96.66666603088379
AudioConnection          patchCord1(modOsc, 0, carrierOsc, 0);
AudioConnection          patchCord2(carrierOsc, ampEg);
AudioConnection          patchCord3(ampEg, 0, output, 0);
AudioConnection          patchCord4(ampEg, delay1);
AudioConnection          patchCord5(delay1, 0, output, 1);
AudioControlSGTL5000     control;     //xy=59.666664123535156,26.66666603088379
// GUItool: end automatically generated code

const int TRIGGER_PIN = 0;
const int ECHO_PIN = 1;
const int MAX_PING_DISTANCE = 250;

NewPing ping(TRIGGER_PIN, ECHO_PIN, MAX_PING_DISTANCE);

unsigned long currentTime;
unsigned long previousTime;
const unsigned long TIME_THRESHOLD = 48;

const int AVERAGING_RANGE_MIN = 150;
const int AVERAGING_RANGE_MAX = 2500;
const int AVERAGING_TIME = 32;
movingAvg averager(AVERAGING_TIME);

bool canPlayNote = true;
const int MIN_FREQUENCY = 50;
const int MAX_FREQUENCY = 400;

const float DELAY_TIME = 24.0f;

void setup() {
    Serial.begin(9600);
    initAudio();
    initSdCard();
    
    delay(1000);
    
    initControls();
    initComponents();
}

void initAudio() {
    AudioMemory(32);
    
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
    averager.begin();
}

void initComponents() {
    modOsc.begin(0.5f, 100.0f, WAVEFORM_SINE);
    carrierOsc.begin(0.5f, 200.0f, WAVEFORM_SINE);

    ampEg.attack(100.0f);
    ampEg.decay(100.0f);
    ampEg.sustain(0.4f);
    ampEg.release(1000.0f);

    delay1.delay(0, DELAY_TIME);
}

void loop() {
    currentTime = millis();
    if(currentTime - previousTime >= TIME_THRESHOLD) {
        previousTime = currentTime;

        int pingValue = ping.ping();
        int fixedValue = constrain(pingValue, AVERAGING_RANGE_MIN, AVERAGING_RANGE_MAX);
        int mappedValue = map(fixedValue, AVERAGING_RANGE_MIN, AVERAGING_RANGE_MAX, MIN_FREQUENCY, MAX_FREQUENCY);
        int averagedValue = averager.reading(mappedValue);
        Serial.println(averagedValue);

        if(!canPlayNote) {
            modOsc.frequency(averagedValue);
        }

        if(pingValue <= AVERAGING_RANGE_MAX && canPlayNote) {
            ampEg.noteOn();
            canPlayNote = false;
        } else if(pingValue > AVERAGING_RANGE_MAX && !canPlayNote) {
            ampEg.noteOff();
            canPlayNote = true;
        }
    }

    float amplitude = analogRead(A1) / 1023.0f;
    control.volume(amplitude);
}