#ifndef SETTINGS_EEPROM_H
#define SETTINGS_EEPROM_H

#include "HardwareConfig.h"

enum RXADCfilter_ {LPF_10Hz, LPF_20Hz, LPF_50Hz, LPF_100Hz};



bool eepromSaveRquired = false;

struct EepromSettingsStruct {
  uint16_t eepromVersionNumber;

  uint8_t RXBand[MaxNumRecievers];
  uint8_t RXChannel[MaxNumRecievers];
  uint16_t RXfrequencies[MaxNumRecievers];
  int RSSIthresholds[MaxNumRecievers];
  RXADCfilter_ RXADCfilter;

  void setup();
  void load();
  void save();
  void defaults();
};

const struct {
  uint16_t eepromVersionNumber = EEPROM_VERSION_NUMBER;

  uint8_t RXBand[MaxNumRecievers] = {4, 4, 4, 4, 0, 0};
  uint8_t RXChannel[MaxNumRecievers] = {0, 2, 4, 6, 0, 8};
  uint16_t RXfrequencies[MaxNumRecievers] = {5740, 5780, 5820, 5860, 5658, 5917};
  int RSSIthresholds[MaxNumRecievers] = {3500, 3500, 3500, 3500, 3500, 3500};
  RXADCfilter_ RXADCfilter = LPF_20Hz;

} EepromDefaults;

extern EepromSettingsStruct EepromSettings;

#endif
