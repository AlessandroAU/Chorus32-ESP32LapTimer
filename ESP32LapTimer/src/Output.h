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
#ifndef __OUTPUT_H__
#define __OUTPUT_H__

#include <stdint.h>
#include <Arduino.h>


typedef struct output_s {
  void* data;
  void (*init)(void* output);
  void (*sendPacket)(void* output, uint8_t* buf, uint32_t size);
  void (*update)(void* output);
  void (*handle_input_callback)(uint8_t* buf, uint32_t size);
} output_t;

/**
 * \return true if the item has been added to the queue
 */
bool IRAM_ATTR addToSendQueue(uint8_t item);

/**
 * \return the number of items added to the queue
 */
uint8_t IRAM_ATTR addToSendQueue(uint8_t * buff, uint32_t length);

void update_outputs();
void init_outputs();


void output_input_callback(uint8_t* buf, uint32_t size);


#endif // __OUTPUT_H__
