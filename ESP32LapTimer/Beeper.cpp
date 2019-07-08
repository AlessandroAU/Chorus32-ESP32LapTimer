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
