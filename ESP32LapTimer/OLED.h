#ifndef __OLED_H__
#define __OLED_H__

#include <stdint.h>

enum button_press {
  BUTTON_SHORT,
  BUTTON_LONG
};

void oledSetup();
void OLED_CheckIfUpdateReq();
void setDisplayScreenNumber(uint16_t num);
uint16_t getDisplayScreenNumber();
void incrementRxFrequency(uint8_t currentRXNumber);
void incrementRxBand(uint8_t currentRXNumber);

void setNumberOfOledScreens(uint8_t num);
uint8_t getNumberOfOledScreens();

void setNumberOfBaseScreens(uint8_t num);
uint8_t getNumberOfBaseScreens();

void rx_page_update(void* data);
void rx_page_input(void* data, uint8_t index, uint8_t type);
void rx_page_init(void* data);

void summary_page_update(void* data);
void adc_page_update(void* data);
void calib_page_update(void* data);
void calib_page_input(void* data, uint8_t index, uint8_t type);

void airplane_page_update(void* data);
void airplane_page_input(void* data, uint8_t index, uint8_t type);

void oledInjectInput(uint8_t index, uint8_t type);
void next_page_input(void* data, uint8_t index, uint8_t type);


#endif // __OLED_H__
