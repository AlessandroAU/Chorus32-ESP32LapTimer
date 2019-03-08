
#include "Timer.h"

Timer beeperTimer = Timer(100);

void beep() {
  digitalWrite(BEEPER, HIGH);
  beeperTimer.reset();
}

void beeperUpdate() {
  if (beeperTimer.hasTicked()) {
    digitalWrite(BEEPER, LOW);
  }
}
