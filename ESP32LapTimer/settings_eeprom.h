#ifndef SETTINGS_EEPROM_H
#define SETTINGS_EEPROM_H

#include "HardwareConfig.h"

enum RXADCfilter_ {LPF_10Hz, LPF_20Hz, LPF_50Hz, LPF_100Hz};



bool eepromSaveRquired = false;

struct EepromSettingsStruct {
  uint16_t eepromVersionNumber;

  uint8_t RXBand[NumRecievers];
  uint8_t RXChannel[NumRecievers];
  uint16_t RXfrequencies[NumRecievers];
  int RSSIthresholds[NumRecievers];
  RXADCfilter_ RXADCfilter;
  
  void setup();
  void load();
  void save();
  void defaults();
};

const struct {
  uint16_t eepromVersionNumber = VERSION_NUMBER;
  
  uint8_t RXBand[NumRecievers] = {4, 4, 4, 4};
  uint8_t RXChannel[NumRecievers] = {0, 2, 4, 6};
  uint16_t RXfrequencies[NumRecievers] = {5740, 5780, 5820, 5860};
  int RSSIthresholds[NumRecievers] = {3500, 3500, 3500, 3500};
  RXADCfilter_ RXADCfilter = LPF_20Hz;
  
} EepromDefaults;

extern EepromSettingsStruct EepromSettings;

#endif
