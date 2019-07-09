#pragma once

#include <esp_attr.h>

#include "HardwareConfig.h"
#include "Filter.h"

void ConfigureADC();
void IRAM_ATTR CheckRSSIthresholdExceeded();
void ReadVBAT_INA219();
void IRAM_ATTR nbADCread( void * pvParameters );

uint16_t getRSSI(uint8_t index);
void setRSSIThreshold(uint8_t node, uint16_t threshold);
uint16_t getRSSIThreshold(uint8_t node);
uint16_t getADCLoopCount();
void setADCLoopCount(uint16_t count);

void setVbatCal(float calibration);
float getMaFloat();

float getVbatFloat();
void setVbatFloat(float val);

float getVBATcalibration();
void setVBATcalibration(float val);

