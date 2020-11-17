#include <NewPing.h>
#include <movingAvg.h>

const int TRIGGER_PIN = 0;
const int ECHO_PIN = 1;
const int MAX_PING_DISTANCE = 250;

NewPing ping(TRIGGER_PIN, ECHO_PIN, MAX_PING_DISTANCE);

unsigned long currentTime;
unsigned long previousTime;
const unsigned long TIME_THRESHOLD = 32;

const int AVERAGING_RANGE_MIN = 150;
const int AVERAGING_RANGE_MAX = 2500;
const int AVERAGING_TIME = 16;
movingAvg averager(AVERAGING_TIME);

void setup() {
    Serial.begin(9600);

    averager.begin();
}

void loop() {
    currentTime = millis();
    if(currentTime - previousTime >= TIME_THRESHOLD) {
        int pingValue = ping.ping();
        int fixedValue = constrain(pingValue, AVERAGING_RANGE_MIN, AVERAGING_RANGE_MAX);
        int mappedValue = map(fixedValue, AVERAGING_RANGE_MIN, AVERAGING_RANGE_MAX, 0, 1000);
        int averagedValue = averager.reading(mappedValue);

        previousTime = currentTime;
    }
}
