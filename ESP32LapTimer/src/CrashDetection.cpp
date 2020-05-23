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
#include "CrashDetection.h"

#include <rom/rtc.h>
#include <Arduino.h>

// positive values indicate a crashing system. negative values a manual reboot loop
RTC_NOINIT_ATTR static int crash_count = 0;

bool is_crash_mode() {
  return (crash_count > MAX_CRASH_COUNT) || (crash_count < -MAX_CRASH_COUNT);
}

void init_crash_detection() {
  // crash reason is not sw reset, so not a crash!
  if(rtc_get_reset_reason(0) != 12) {
    crash_count = 0;
  } else {
    ++crash_count;
  }
}

int get_crash_count() {
  return crash_count;
}

void reset_crash_count() {
  crash_count = 0;
}

void restart_esp() {
  --crash_count;
  ESP.restart();
}
