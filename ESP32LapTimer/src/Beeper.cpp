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
#include "Beeper.h"

#include "Timer.h"
#include "HardwareConfig.h"

#include <Arduino.h>

static Timer beeperTimer = Timer(50);

void beep() {
  digitalWrite(BEEPER, HIGH);
  delay(50);
  digitalWrite(BEEPER, LOW);
  beeperTimer.reset();
}

void doubleBeep() {
  int i=0;
  for (i=0; i<=1; i++) {
    digitalWrite(BEEPER, HIGH);
    delay(50);
    digitalWrite(BEEPER, LOW);
    delay(50);
  }
}

void chirps() {
  int i = 0;
  for (i = 0; i <=5; i++) {
    digitalWrite(BEEPER, HIGH);
    delay(10);
    digitalWrite(BEEPER, LOW);
    delay(10);
  }
}

void fiveBeep() {
  int i=0;
  for (i=0; i<=4; i++) {
    digitalWrite(BEEPER, HIGH);
    delay(100);
    digitalWrite(BEEPER, LOW);
    delay(50);
  }
}

void beeperUpdate() {
  if (beeperTimer.hasTicked()) {
    digitalWrite(BEEPER, LOW);
  }
}
