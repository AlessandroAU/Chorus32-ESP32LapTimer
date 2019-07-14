/////define Pins//////
#include "HardwareConfig.h"

#include <Arduino.h>

void InitHardwarePins() {

  pinMode(SCK, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(CS1, OUTPUT);
  pinMode(CS2, OUTPUT);
  pinMode(CS3, OUTPUT);
  pinMode(CS4, OUTPUT);
  pinMode(CS5, OUTPUT);
  pinMode(CS6, OUTPUT);
  //pinMode(MISO, INPUT);
  pinMode(BEEPER, OUTPUT);
  digitalWrite(BEEPER, LOW);
}
