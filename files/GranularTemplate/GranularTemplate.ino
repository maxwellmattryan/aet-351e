#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

///////////////////////////////////
/// COPY DESIGN TOOL CODE HERE ////
///////////////////////////////////

int numSamples = 14700;
int16_t granularMemory[numSamples]; // define granular memory

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

}

void initComponents() {
  
}

void loop() {
    if (sampler.isPlaying() == false) {
        sampler.play("SAMPLE.wav");
        delay(10);
    }

    float amplitude = analogRead(A1) / 1023.0f;
    control.volume(amplitude);
}
