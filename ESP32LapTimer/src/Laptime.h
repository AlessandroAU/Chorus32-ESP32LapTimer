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
#ifndef __LAPTIME_H__
#define __LAPTIME_H__

#include <stdint.h>

void resetLaptimes();
void addLap(uint8_t receiver);
uint32_t getMinLapTime();
void setMinLapTime(uint32_t time);
uint32_t getLaptime(uint8_t receiver);
uint32_t getLaptime(uint8_t receiver, uint8_t lap);
uint32_t getLaptimeRel(uint8_t receiver, uint8_t lap);
uint32_t getLaptimeRelToStart(uint8_t receiver, uint8_t lap);
uint32_t getLaptimeRel(uint8_t receiver);
void startRaceLap();


/**
 * Adds a lap to the pool and returns the current lap id
 */
uint8_t addLap(uint8_t receiver, uint32_t time);
/// Laps begin at 1. lap 0 is always 0
uint8_t getCurrentLap(uint8_t receiver);

/// Sends unsent laps to the output queue
void sendNewLaps();

#endif // __LAPTIME_H__
