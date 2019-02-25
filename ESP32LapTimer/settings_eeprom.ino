#include <EEPROM.h>
#include "settings_eeprom.h"

struct EepromSettingsStruct EepromSettings;

void EepromSettingsStruct::setup() {
  EEPROM.begin(512);
  this->load();
}

void EepromSettingsStruct::load() {
  EEPROM.get(0, *this);

  if (this->eepromVersionNumber != VERSION_NUMBER)
    this->defaults();
}

void EepromSettingsStruct::save() {
  if (eepromSaveRquired) {
    for (int i = 0; i < NumRecievers; i++) {
      this->RXBand[i] = RXBand[i];
      this->RXChannel[i] = RXChannel[i];
      this->RXfrequencies[i] = RXfrequencies[i];
      this->RSSIthresholds[i] = RSSIthresholds[i];
    }
    
    EEPROM.put(0, *this);
    EEPROM.commit();    
  }
}

void EepromSettingsStruct::defaults() {
  memcpy_P(this, &EepromDefaults, sizeof(EepromDefaults));
  EEPROM.put(0, *this);
  EEPROM.commit();    
}

