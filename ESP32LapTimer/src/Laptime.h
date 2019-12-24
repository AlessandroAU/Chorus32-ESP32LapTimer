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
