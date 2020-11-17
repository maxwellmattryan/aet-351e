const int LED_PIN_01 = 0;
const int LED_PIN_02 = 1;
const int LED_PIN_03 = 2;

const int MIDI_CHANNEL = 1;

void setup() {
    pinMode(LED_PIN_01, OUTPUT);
    pinMode(LED_PIN_02, OUTPUT);
    pinMode(LED_PIN_03, OUTPUT);

    usbMIDI.setHandleNoteOn(onNote);
    usbMIDI.setHandleNoteOff(offNote);
}

void loop() {
    usbMIDI.read(MIDI_CHANNEL);
}

void onNote(byte channel, byte note, byte velocity) {
    if(note == 60) {
        digitalWrite(LED_PIN_01, HIGH);
    }
}

void offNote(byte channel, byte note, byte velocity) {
    
}
