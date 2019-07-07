#pragma once

#include "HardwareConfig.h"
#include "Filter.h"

float VBATcalibration;

void ConfigureADC();
void InitADCtimer();
void IRAM_ATTR CheckRSSIthresholdExceeded();

uint16_t getRSSI(uint8_t index);
void setRSSIThreshold(uint16_t threshold);
