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
void summary_page_init(void* data);
void summary_page_input(void* data, uint8_t index, uint8_t type);

void adc_page_update(void* data);
void adc_page_init(void* data);

void calib_page_update(void* data);
void calib_page_input(void* data, uint8_t index, uint8_t type);

void airplane_page_update(void* data);
void airplane_page_input(void* data, uint8_t index, uint8_t type);

void oledInjectInput(uint8_t index, uint8_t type);
void next_page_input(void* data, uint8_t index, uint8_t type);


#endif // __OLED_H__
