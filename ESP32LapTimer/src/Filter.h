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
#ifndef __FILTER_H__
#define __FILTER_H__

#include <stdint.h>

// see https://en.wikipedia.org/wiki/Low-pass_filter#Simple_infinite_impulse_response_filter for reference
typedef struct lowpass_filter_s {
  float state;
  float RC;
  float alpha;
  uint32_t last_call;
  
} lowpass_filter_t; 

void filter_init(lowpass_filter_t* filter, float cutoff, float dt);
void filter_adjust_dt(lowpass_filter_t* filter, float dt);
float filter_add_value(lowpass_filter_t* filter, float value);


#endif // __FILTER_H__
