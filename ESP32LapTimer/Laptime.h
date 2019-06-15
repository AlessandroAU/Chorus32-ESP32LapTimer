#ifndef __LAPTIME_H__
#define __LAPTIME_H__

#include <stdint.h>

void resetLaptimes();
void addLap(uint8_t receiver);
uint32_t getMinLapTime();
void setMinLapTime(uint32_t time);
uint32_t getLaptime(uint8_t receiver);
uint32_t getLaptime(uint8_t receiver, uint8_t lap);

/**
 * Adds a lap to the pool and returns the current lap id
 */
uint8_t addLap(uint8_t receiver, uint32_t time);
uint8_t getCurrentLap(uint8_t receiver);

#endif // __LAPTIME_H__
