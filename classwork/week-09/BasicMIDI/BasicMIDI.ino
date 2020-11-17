#include <Bounce.h>

const int ENV_BUTTON_PIN = 0;
Bounce envButton = Bounce(ENV_BUTTON_PIN, 20);
const int OTHER_BUTTON_PIN = 1;
Bounce otherButton = Bounce(OTHER_BUTTON_PIN, 20);

const int MIDI_CHANNEL = 1;

const int NOTE_01 = 36;
const int NOTE_02 = 48;
const int VELOCITY = 100;

void setup() {
    pinMode(ENV_BUTTON_PIN, INPUT_PULLUP);
    pinMode(OTHER_BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
    envButton.update();
    if(envButton.fallingEdge()) {
        usbMIDI.sendNoteOn(NOTE_01, VELOCITY, MIDI_CHANNEL);
    } else if(envButton.risingEdge()) {
        usbMIDI.sendNoteOff(NOTE_01, 0.0f, MIDI_CHANNEL);
    }

    otherButton.update();
    if(otherButton.fallingEdge()) {
        usbMIDI.sendNoteOn(NOTE_02, VELOCITY, MIDI_CHANNEL);
    } else if(otherButton.risingEdge()) {
        usbMIDI.sendNoteOff(NOTE_02, 0.0f, MIDI_CHANNEL);
    }

    while(usbMIDI.read()) {
        // DO NOTHING...
    }
}
