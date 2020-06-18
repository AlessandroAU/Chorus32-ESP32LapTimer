/*
 * This file is part of Chorus32-ESP32LapTimer 
 * (see https://github.com/AlessandroAU/Chorus32-ESP32LapTimer).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "Laptime.h"

#include <stdint.h>
#include <Arduino.h>

#include "HardwareConfig.h"
#include "settings_eeprom.h"
#include "Comms.h"

static uint32_t LapTimes[MAX_NUM_PILOTS][MAX_LAPS_NUM];
static uint8_t lap_counter[MAX_NUM_PILOTS]; //Keep track of what lap we are up too
static int last_lap_sent[MAX_NUM_PILOTS];

static uint32_t MinLapTime = 5000;  //this is in millis
static uint32_t start_time = 0;

void resetLaptimes() {
  memset(LapTimes, 0, MAX_NUM_PILOTS * MAX_LAPS_NUM * sizeof(LapTimes[0][0]));
  memset(lap_counter, 0, MAX_NUM_PILOTS * sizeof(lap_counter[0]));
  memset(last_lap_sent, 0, MAX_NUM_PILOTS * sizeof(last_lap_sent[0]));
}

void sendNewLaps() {
  for (int i = 0; i < MAX_NUM_PILOTS; ++i) {
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
  if(receiver < MAX_NUM_PILOTS && lap < MAX_LAPS_NUM) {
    return LapTimes[receiver][lap];
  }
  return 0;
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
