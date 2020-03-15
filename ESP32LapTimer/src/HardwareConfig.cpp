/////define Pins//////
#include "HardwareConfig.h"
#include "pp_helpers.h"

#include <Arduino.h>

int SELECT_PINS[PP_NARG(CS_PINS)] = { CS_PINS };
adc1_channel_t ANALOG_PINS[PP_NARG(ADC_PINS)] = { ADC_PINS };

void InitHardwarePins() {

  pinMode(SCK, OUTPUT);
  pinMode(MOSI, OUTPUT);

  for(int i = 0; i < PP_NARG(CS_PINS); i++) {
    pinMode(SELECT_PINS[i], OUTPUT);
  }

  //pinMode(MISO, INPUT);
  pinMode(BEEPER, OUTPUT);
  digitalWrite(BEEPER, LOW);
}
