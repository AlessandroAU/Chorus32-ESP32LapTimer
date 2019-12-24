#include "Laptime.h"

#include <stdint.h>
#include <Arduino.h>

#include "HardwareConfig.h"
#include "settings_eeprom.h"
#include "Comms.h"

static volatile uint32_t LapTimes[MaxNumReceivers][MAX_LAPS_NUM];
static volatile int lap_counter[MaxNumReceivers] = {0, 0, 0, 0, 0, 0}; //Keep track of what lap we are up too
static int last_lap_sent[MaxNumReceivers];

static uint32_t MinLapTime = 5000;  //this is in millis
static uint32_t start_time = 0;

void resetLaptimes() {
  for (int i = 0; i < getNumReceivers(); ++i) {
    lap_counter[i] = 0;
    last_lap_sent[i] = 0;
  }
}

void sendNewLaps() {
  for (int i = 0; i < getNumReceivers(); ++i) {
    int laps_to_send = lap_counter[i] - last_lap_sent[i];
    if(laps_to_send > 0) {
      for(int j = 0; j < laps_to_send; ++j) {
        sendLap(lap_counter[i] - j, i);
      }
      last_lap_sent[i] += laps_to_send;
    }
  }
}

uint32_t getLaptime(uint8_t receiver, uint8_t lap) {
  return LapTimes[receiver][lap];
}

uint32_t getLaptime(uint8_t receiver) {
  return getLaptime(receiver, lap_counter[receiver]);
}

uint32_t getLaptimeRel(uint8_t receiver, uint8_t lap) {
  if(lap == 1) {
    return getLaptime(receiver, lap) - start_time;
  } else if(lap == 0) {
    return 0;
  }
  return getLaptime(receiver, lap) - getLaptime(receiver, lap - 1);
}

uint32_t getLaptimeRelToStart(uint8_t receiver, uint8_t lap) {
  return getLaptime(receiver, lap) - start_time;
}

uint32_t getLaptimeRel(uint8_t receiver) {
  return getLaptimeRel(receiver, lap_counter[receiver]);
}

uint8_t addLap(uint8_t receiver, uint32_t time) {
  lap_counter[receiver] = lap_counter[receiver] + 1;
  LapTimes[receiver][lap_counter[receiver]] = time;
  return lap_counter[receiver];
}

uint32_t getMinLapTime() {
  return MinLapTime;
}

void setMinLapTime(uint32_t time) {
  MinLapTime = time;
}

uint8_t getCurrentLap(uint8_t receiver) {
  return lap_counter[receiver];
}

void startRaceLap() {
  resetLaptimes();
  start_time = millis();
}
