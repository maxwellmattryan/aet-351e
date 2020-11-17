#include<Wire.h>
#include<movingAvg.h>

const int MPU_ADDRESS = 0x68;

int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ; 

const int CC_X_CONTROL_NUM = 20;
const int CC_Y_CONTROL_NUM = 21;
const int CC_MIDI_CHANNEL = 1;

const int BUTTON_01_PIN = 0;
const int BUTTON_02_PIN = 1;

movingAvg xAvg(8);
movingAvg yAvg(8);

int xOldAvg = -1;
int yOldAvg = -1;

const int MSEC_THRESHOLD = 30;
elapsedMillis msec = 0;

void setup() {
  Wire.begin();
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  
  Serial.begin(9600);

  pinMode(BUTTON_01_PIN, INPUT_PULLUP);
  pinMode(BUTTON_02_PIN, INPUT_PULLUP);

  xAvg.begin();
  yAvg.begin();
}

void loop() {
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDRESS, 14, true);

  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();

  if(msec >= MSEC_THRESHOLD) {
    msec = 0;

    bool isButton01Pressed = digitalRead(BUTTON_01_PIN);
    bool isButton02Pressed = digitalRead(BUTTON_02_PIN);

    // CAUTION: MIDI values HAVE to be constrained
    int xConstrained = constrain(GyX, -17000, 17000);
    int xMapped = map(xConstrained, -17000, 17000, 0, 127);
    int xAveraged = xAvg.reading(xMapped);

    if(isButton01Pressed == HIGH && xOldAvg != xAveraged) {
        usbMIDI.sendControlChange(CC_X_CONTROL_NUM, xAveraged, CC_MIDI_CHANNEL);

        xOldAvg = xAveraged;
    }

    int yConstrained = constrain(GyY, -17000, 17000);
    int yMapped = map(yConstrained, -17000, 17000, 0, 127);
    int yAveraged = yAvg.reading(yMapped);

    if(isButton02Pressed == HIGH && yOldAvg != yAveraged) {
        usbMIDI.sendControlChange(CC_Y_CONTROL_NUM, yAveraged, CC_MIDI_CHANNEL);

        yOldAvg = yAveraged;
    }

    //Serial.print("GyX = "); Serial.print(GyX);
    //Serial.print(" | GyY = "); Serial.println(GyY);
  }
}
