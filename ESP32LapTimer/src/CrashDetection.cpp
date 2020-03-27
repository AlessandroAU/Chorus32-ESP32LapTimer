#include "CrashDetection.h"

#include <rom/rtc.h>
#include <Arduino.h>

RTC_NOINIT_ATTR static int crash_count = 0;

bool is_crash_mode() {
  return crash_count > MAX_CRASH_COUNT;
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


void restart_esp() {
  --crash_count;
  ESP.restart();
}
