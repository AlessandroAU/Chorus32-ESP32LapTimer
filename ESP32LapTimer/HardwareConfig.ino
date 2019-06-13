/////define Pins//////
#include "HardwareConfig.h"

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

  #ifdef Micro32-PCB
    pinMode(ADC_SELECT, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(STATUS_LED, OUTPUT);

    digitalWrite(ADC_SELECT, LOW);
    digitalWrite(STATUS_LED, HIGH);
  #endif
  
}
