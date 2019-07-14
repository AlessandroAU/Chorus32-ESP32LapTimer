#include <EEPROM.h>
#include "settings_eeprom.h"
#include "Comms.h"

struct EepromSettingsStruct EepromSettings;


///////////Extern Variable we need acces too///////////////////////

static RXADCfilter_ RXADCfilter;
static ADCVBATmode_ ADCVBATmode;
static bool eepromSaveRequired = false;

uint8_t NumRecievers = 6;
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

bool EepromSettingsStruct::SanityCheck() {

  bool IsGoodEEPROM = true;

  if (EepromSettings.NumRecievers > MaxNumRecievers) {
    IsGoodEEPROM = false;
    Serial.print("Error: Corrupted EEPROM value NumRecievers: ");
    Serial.println(EepromSettings.NumRecievers);
    return IsGoodEEPROM;
  }


  if (EepromSettings.RXADCfilter > MaxADCFilter) {
    IsGoodEEPROM = false;
    Serial.print("Error: Corrupted EEPROM value RXADCfilter: ");
    Serial.println(EepromSettings.RXADCfilter);
    return IsGoodEEPROM;
  }

  if (EepromSettings.ADCVBATmode > MaxVbatMode) {
    IsGoodEEPROM = false;
    Serial.print("Error: Corrupted EEPROM value ADCVBATmode: ");
    Serial.println(EepromSettings.ADCVBATmode);
    return IsGoodEEPROM;
  }

  if (EepromSettings.VBATcalibration > MaxVBATCalibration) {
    IsGoodEEPROM = false;
    Serial.print("Error: Corrupted EEPROM value VBATcalibration: ");
    Serial.println(EepromSettings.VBATcalibration);
    return IsGoodEEPROM;
  }

  for (int i = 0; i < EepromSettings.NumRecievers; i++) {
    if (EepromSettings.RXBand[i] > MaxBand) {
      IsGoodEEPROM = false;
      Serial.print("Error: Corrupted EEPROM NODE: ");
      Serial.print(i);
      Serial.print(" value MaxBand: ");
      Serial.println(EepromSettings.RXBand[i]);
      return IsGoodEEPROM;
    }

  }

  for (int i = 0; i < EepromSettings.NumRecievers; i++) {
    if (EepromSettings.RXChannel[i] > MaxChannel) {
      IsGoodEEPROM = false;
      Serial.print("Error: Corrupted EEPROM NODE: ");
      Serial.print(i);
      Serial.print(" value RXChannel: ");
      Serial.println(EepromSettings.RXChannel[i]);
      return IsGoodEEPROM;
    }
  }

  for (int i = 0; i < EepromSettings.NumRecievers; i++) {
    if ((EepromSettings.RXfrequencies[i] > MaxFreq) or (EepromSettings.RXfrequencies[i] < MinFreq)) {
      IsGoodEEPROM = false;
      Serial.print("Error: Corrupted EEPROM NODE: ");
      Serial.print(i);
      Serial.print(" value RXfrequencies: ");
      Serial.println(EepromSettings.RXfrequencies[i]);
      return IsGoodEEPROM;
    }
  }

  for (int i = 0; i < EepromSettings.NumRecievers; i++) {
    if (EepromSettings.RSSIthresholds[i] > MaxThreshold) {
      IsGoodEEPROM = false;
      Serial.print("Error: Corrupted EEPROM NODE: ");
      Serial.print(i);
      Serial.print(" value RSSIthresholds: ");
      Serial.println(EepromSettings.RSSIthresholds[i]);
      return IsGoodEEPROM;
    }
  }
  return IsGoodEEPROM;
}

void EepromSettingsStruct::save() {
  if (eepromSaveRequired) {
    EEPROM.put(0, *this);
    EEPROM.commit();
    eepromSaveRequired = false;
    Serial.println("EEPROM SAVED");
  }
}

void EepromSettingsStruct::defaults() {
  memcpy_P(this, &EepromDefaults, sizeof(EepromDefaults));
  EEPROM.put(0, *this);
  EEPROM.commit();
}


RXADCfilter_ getRXADCfilter() {
  return RXADCfilter;
}

ADCVBATmode_ getADCVBATmode() {
  return ADCVBATmode;
}

void setRXADCfilter(RXADCfilter_ filter) {
  RXADCfilter = filter;
}

void setADCVBATmode(ADCVBATmode_ mode) {
  ADCVBATmode = mode;
}

void setSaveRequired() {
  eepromSaveRequired = true;
}

int getWiFiChannel(){
  return EepromSettings.WiFiChannel;
}
int getWiFiProtocol(){
  return EepromSettings.WiFiProtocol;
}
