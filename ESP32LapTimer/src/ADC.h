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
#pragma once

#include <esp_attr.h>
#include <stdint.h>
#include <driver/adc.h>

#include "HardwareConfig.h"
#include "Filter.h"

void ConfigureADC();
void IRAM_ATTR CheckRSSIthresholdExceeded(uint8_t node);
void ReadVBAT_INA219();
void IRAM_ATTR nbADCread( void * pvParameters );
adc1_channel_t IRAM_ATTR getADCChannel(uint8_t adc_num);
uint16_t multisample_adc1(adc1_channel_t channel, uint8_t samples);

uint16_t getRSSI(uint8_t index);
void setRSSIThreshold(uint8_t node, uint16_t threshold);
uint16_t getRSSIThreshold(uint8_t node);
uint16_t getADCLoopCount();
void setADCLoopCount(uint16_t count);

void setVbatCal(float calibration);
float getMaFloat();

float getVbatFloat(bool force_read = false);
void setVbatFloat(float val);

float getVBATcalibration();
void setVBATcalibration(float val);

// TODO: these don't belong here!
uint8_t getActivePilots();
bool isPilotActive(uint8_t pilot);
void setPilotActive(uint8_t pilot, bool active);
void setPilotFilters(uint16_t cutoff);

void setPilotBand(uint8_t pilot, uint8_t band);
void setPilotChannel(uint8_t pilot, uint8_t channel);
void setPilotFrequency(uint8_t pilot, uint16_t frequency);

uint8_t getPilotBand(uint8_t pilot);
uint8_t getPilotChannel(uint8_t pilot);
uint16_t getPilotFrequency(uint8_t pilot);
