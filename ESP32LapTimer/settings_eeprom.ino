#include <EEPROM.h>
#include "settings_eeprom.h"
#include "Comms.h"

struct EepromSettingsStruct EepromSettings;


///////////Extern Variable we need acces too///////////////////////

extern RXADCfilter_ RXADCfilter;
extern ADCVBATmode_ ADCVBATmode;

extern byte NumRecievers;
extern float VBATcalibration;

//////////////////////////////////////////////////////////////////

void EepromSettingsStruct::setup() {
  EEPROM.begin(512);
  this->load();
}

void EepromSettingsStruct::load() {
  EEPROM.get(0, *this);
  Serial.println("EEPROM LOADED");

  Serial.println(EepromSettings.NumRecievers);
  Serial.println(NumRecievers);
  
  if (this->eepromVersionNumber != EEPROM_VERSION_NUMBER) {
    this->defaults();
    Serial.println("EEPROM DEFAULTS LOADED");
  }
}

void EepromSettingsStruct::save() {
  if (eepromSaveRquired) {
    EEPROM.put(0, *this);
    EEPROM.commit();
    eepromSaveRquired = false;
    Serial.println("EEPROM SAVED");
  }
}

void EepromSettingsStruct::defaults() {
  memcpy_P(this, &EepromDefaults, sizeof(EepromDefaults));
  EEPROM.put(0, *this);
  EEPROM.commit();
}
