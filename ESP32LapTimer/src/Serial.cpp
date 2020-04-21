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
#include "Serial.h"

#include "Output.h"

#include <Arduino.h>
#include <stdint.h>

#define SERIAL_INPUT_BUF 50

static uint8_t SerialBuffIn[SERIAL_INPUT_BUF];
static uint8_t current_pos = 0;

void serial_update(void* output) {
  while (Serial.available() > 0) {
    char rc = Serial.read();
    SerialBuffIn[current_pos] = rc;
    ++current_pos;
    // Found packet, so handle it
    if(rc == '\n') {
      output_t* out = (output_t*)output;
      out->handle_input_callback(SerialBuffIn, current_pos);
      current_pos = 0;
    } else if(current_pos >= SERIAL_INPUT_BUF) {
      // we are overflowing, so we just discard the current buffer which should just be garbage
      current_pos = 0;
    }
  }
}

void serial_init(void* output) {
  (void)output;
}

void serial_send_packet(void* output, uint8_t* buf, uint32_t size) {
  (void) output;
  Serial.write(buf, size);
}
