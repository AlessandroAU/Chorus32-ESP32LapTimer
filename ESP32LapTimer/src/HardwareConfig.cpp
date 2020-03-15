/////define Pins//////
#include "HardwareConfig.h"

#include <Arduino.h>

int SELECT_PINS[NUM_RECEIVERS] = { CS_PINS };

void InitHardwarePins() {

  pinMode(SCK, OUTPUT);
  pinMode(MOSI, OUTPUT);

  for(int i = 0; i < NUM_RECEIVERS; i++) {
    pinMode(SELECT_PINS[i], OUTPUT);
  }

  //pinMode(MISO, INPUT);
  pinMode(BEEPER, OUTPUT);
  digitalWrite(BEEPER, LOW);
}
