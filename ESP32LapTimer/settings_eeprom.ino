#include <EEPROM.h>
#include "settings_eeprom.h"
#include "Comms.h"

struct EepromSettingsStruct EepromSettings;

void EepromSettingsStruct::setup() {
  EEPROM.begin(512);
  this->load();
}

void EepromSettingsStruct::load() {
  EEPROM.get(0, *this);

  if (this->eepromVersionNumber != EEPROM_VERSION_NUMBER) {
    this->defaults();
  }
}

void EepromSettingsStruct::save() {
  if (eepromSaveRquired) {
    EEPROM.put(0, *this);
    EEPROM.commit();   
    eepromSaveRquired = false;
  }
}

void EepromSettingsStruct::defaults() {
  memcpy_P(this, &EepromDefaults, sizeof(EepromDefaults));
  EEPROM.put(0, *this);
  EEPROM.commit();    
}
