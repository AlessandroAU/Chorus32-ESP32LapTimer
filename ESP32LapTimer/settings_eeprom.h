#ifndef SETTINGS_EEPROM_H
#define SETTINGS_EEPROM_H

#include "HardwareConfig.h"

enum RXADCfilter_ {LPF_10Hz, LPF_20Hz, LPF_50Hz, LPF_100Hz};
enum ADCVBATmode_ {OFF, ADC_CH5, ADC_CH6, INA219};

RXADCfilter_ RXADCfilter;
ADCVBATmode_ ADCVBATmode;

bool eepromSaveRquired = false;

struct EepromSettingsStruct {
  uint16_t eepromVersionNumber;
  uint8_t RXBand[MaxNumRecievers];
  uint8_t RXChannel[MaxNumRecievers];
  uint16_t RXfrequencies[MaxNumRecievers];
  int RSSIthresholds[MaxNumRecievers];
  RXADCfilter_ RXADCfilter;
  ADCVBATmode_ ADCVBATmode;
  float VBATcalibration;
  byte NumRecievers;


  void setup();
  void load();
  void save();
  void defaults();
};

const struct {
  uint16_t eepromVersionNumber = EEPROM_VERSION_NUMBER;

  uint8_t RXBand[MaxNumRecievers] = {0, 0, 0, 0, 0, 0};
  uint8_t RXChannel[MaxNumRecievers] = {0, 1, 2, 3, 4, 5};
  uint16_t RXfrequencies[MaxNumRecievers] = {5658, 5695, 5732, 5769, 5806, 5843};
      int RSSIthresholds[MaxNumRecievers] = {2048, 2048, 2048, 2048, 2048, 2048};
  RXADCfilter_ RXADCfilter = LPF_20Hz;
  ADCVBATmode_ ADCVBATmode = ADC_CH5;
  float VBATcalibration = 1.000;
  byte NumRecievers = 6;

} EepromDefaults;

extern EepromSettingsStruct EepromSettings;

#endif
