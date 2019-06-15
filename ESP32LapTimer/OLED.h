#ifndef __OLED_H__
#define __OLED_H__

#include <stdint.h>

void oledSetup();
void oledUpdate();
void OLED_CheckIfUpdateReq();
void setDisplayScreenNumber(uint16_t num);
uint16_t getDisplayScreenNumber();
void incrementRxFrequency();
void incrementRxBand();

void setNumberOfOledScreens(uint8_t num);
uint8_t getNumberOfOledScreens();

void setNumberOfBaseScreens(uint8_t num);
uint8_t getNumberOfBaseScreens();


#endif // __OLED_H__
