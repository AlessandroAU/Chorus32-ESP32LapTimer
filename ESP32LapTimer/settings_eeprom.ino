#include <EEPROM.h>
#include "settings_eeprom.h"

struct EepromSettingsStruct EepromSettings;

void EepromSettingsStruct::setup() {
  EEPROM.begin(2048);
  this->load();
}

void EepromSettingsStruct::load() {
  EEPROM.get(0, *this);

  if (this->eepromVersionNumber != VERSION_NUMBER)
    this->defaults();

  for (int i = 0; i < NumRecievers; i++) {
    RXBand[i] = this->RXBand[i];
    RXChannel[i] = this->RXChannel[i];
    RXfrequencies[i] = this->RXfrequencies[i];
  }
}

void EepromSettingsStruct::save() {
  for (int i = 0; i < NumRecievers; i++) {
    this->RXBand[i] = RXBand[i];
    this->RXChannel[i] = RXChannel[i];
    this->RXfrequencies[i] = RXfrequencies[i];
  }
  
  EEPROM.put(0, *this);
  EEPROM.commit();    
}

void EepromSettingsStruct::defaults() {
  memcpy_P(this, &EepromDefaults, sizeof(EepromDefaults));
  EEPROM.put(0, *this);
  EEPROM.commit();    
}

