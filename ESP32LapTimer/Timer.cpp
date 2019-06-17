#include <Arduino.h>
#include "Timer.h"


Timer::Timer(uint16_t delay) {
    this->delay = delay;
    this->nextTick = millis() + this->delay;
    this->ticked = false;
}

bool Timer::hasTicked() {
    if (this->ticked)
        return true;

    if (millis() >= this->nextTick) {
        this->ticked = true;
        return true;
    }

    return false;
}

void Timer::reset() {
    this->nextTick = millis() + this->delay;
    this->ticked = false;
}
