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

#include "HardwareConfig.h"
#include "crc.h"

enum RXADCfilter_ {LPF_10Hz, LPF_20Hz, LPF_50Hz, LPF_100Hz};
enum ADCVBATmode_ {OFF, ADC_CH5, ADC_CH6, INA219};

#define MaxChannel 7
#define MaxBand 7

#define MaxFreq 5945
#define MinFreq 5180

#define MaxADCFilter 3
#define MaxVbatMode 3
#define MaxVBATCalibration 100.00
#define MaxThreshold 4095

struct EepromSettingsStruct {
  uint16_t eepromVersionNumber;
  uint8_t RXBand[MAX_NUM_PILOTS];
  uint8_t RXChannel[MAX_NUM_PILOTS];
  uint16_t RSSIthresholds[MAX_NUM_PILOTS];
  RXADCfilter_ RXADCfilter;
  ADCVBATmode_ ADCVBATmode;
  float VBATcalibration;
  uint8_t NumReceivers;
  uint16_t RxCalibrationMin[MAX_NUM_RECEIVERS];
  uint16_t RxCalibrationMax[MAX_NUM_RECEIVERS];
  uint8_t WiFiProtocol; // 0 is b only, 1 is bgn
  uint8_t WiFiChannel;
  uint32_t display_timeout_ms;
  crc_t crc; // This MUST be the last variable!


  void setup();
  void load();
  void save();
  void defaults();
  bool SanityCheck();
  void updateCRC();
  bool validateCRC();
  crc_t calcCRC();
};

extern EepromSettingsStruct EepromSettings;

RXADCfilter_ getRXADCfilter();
ADCVBATmode_ getADCVBATmode();

void setRXADCfilter(RXADCfilter_ filter);
void setADCVBATmode(ADCVBATmode_ mode);

int getWiFiChannel();
int getWiFiProtocol();

uint8_t getNumReceivers();
uint32_t getDisplayTimeout();

void setSaveRequired();
