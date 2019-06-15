#include "Laptime.h"

#include <stdint.h>

#include "HardwareConfig.h"

static volatile uint32_t LapTimes[MaxNumRecievers][100];
static volatile int LapTimePtr[MaxNumRecievers] = {0, 0, 0, 0, 0, 0}; //Keep track of what lap we are up too

static uint32_t MinLapTime = 5000;  //this is in millis

void resetLaptimes() {
  for (int i = 0; i < NumRecievers; ++i) {
    LapTimePtr[i] = 0;
  }
}

uint32_t getLaptime(uint8_t receiver, uint8_t lap) {
  return LapTimes[receiver][lap];
}

uint32_t getLaptime(uint8_t receiver) {
  return getLaptime(receiver, LapTimePtr[receiver]);
}

uint8_t addLap(uint8_t receiver, uint32_t time) {
  LapTimePtr[receiver] = LapTimePtr[receiver] + 1;
  LapTimes[receiver][LapTimePtr[receiver]] = time;
  return LapTimePtr[receiver];
}

uint32_t getMinLapTime() {
  return MinLapTime;
}

void setMinLapTime(uint32_t time) {
  MinLapTime = time;
}

uint8_t getCurrentLap(uint8_t receiver) {
  return LapTimePtr[receiver];
}
