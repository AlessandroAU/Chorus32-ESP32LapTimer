#include <EEPROM.h>
#include "settings_eeprom.h"
#include "Comms.h"
#include "RX5808.h"
#include "crc.h"

struct EepromSettingsStruct EepromSettings;

static bool eepromSaveRequired = false;

void EepromSettingsStruct::setup() {
  EEPROM.begin(512);
  this->load();
}

void EepromSettingsStruct::load() {
  EEPROM.get(0, *this);
  Serial.println("EEPROM LOADED");

  Serial.println(EepromSettings.NumReceivers);
  Serial.println(NumReceivers);

  if (this->eepromVersionNumber != EEPROM_VERSION_NUMBER) {
    this->defaults();
    Serial.println("EEPROM DEFAULTS LOADED");
  }
}

bool EepromSettingsStruct::SanityCheck() {

  bool IsGoodEEPROM = true;

  if (EepromSettings.NumReceivers > MaxNumReceivers) {
    IsGoodEEPROM = false;
    Serial.print("Error: Corrupted EEPROM value NumRecievers: ");
    Serial.println(EepromSettings.NumReceivers);
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

  for (int i = 0; i < EepromSettings.NumReceivers; i++) {
    if (EepromSettings.RXBand[i] > MaxBand) {
      IsGoodEEPROM = false;
      Serial.print("Error: Corrupted EEPROM NODE: ");
      Serial.print(i);
      Serial.print(" value MaxBand: ");
      Serial.println(EepromSettings.RXBand[i]);
      return IsGoodEEPROM;
    }

  }

  for (int i = 0; i < EepromSettings.NumReceivers; i++) {
    if (EepromSettings.RXChannel[i] > MaxChannel) {
      IsGoodEEPROM = false;
      Serial.print("Error: Corrupted EEPROM NODE: ");
      Serial.print(i);
      Serial.print(" value RXChannel: ");
      Serial.println(EepromSettings.RXChannel[i]);
      return IsGoodEEPROM;
    }
  }

  for (int i = 0; i < EepromSettings.NumReceivers; i++) {
    if ((EepromSettings.RXfrequencies[i] > MaxFreq) or (EepromSettings.RXfrequencies[i] < MinFreq)) {
      IsGoodEEPROM = false;
      Serial.print("Error: Corrupted EEPROM NODE: ");
      Serial.print(i);
      Serial.print(" value RXfrequencies: ");
      Serial.println(EepromSettings.RXfrequencies[i]);
      return IsGoodEEPROM;
    }
  }

  for (int i = 0; i < EepromSettings.NumReceivers; i++) {
    if (EepromSettings.RSSIthresholds[i] > MaxThreshold) {
      IsGoodEEPROM = false;
      Serial.print("Error: Corrupted EEPROM NODE: ");
      Serial.print(i);
      Serial.print(" value RSSIthresholds: ");
      Serial.println(EepromSettings.RSSIthresholds[i]);
      return IsGoodEEPROM;
    }
  }
  return IsGoodEEPROM && this->validateCRC();
}

void EepromSettingsStruct::save() {
  if (eepromSaveRequired) {
    this->updateCRC();
    EEPROM.put(0, *this);
    EEPROM.commit();
    eepromSaveRequired = false;
    Serial.println("EEPROM SAVED");
  }
}

void EepromSettingsStruct::defaults() {
  // We are using a temporary struct since we might have invalid values during setup and core 0 might use them
  EepromSettingsStruct settings;
  // by setting everything to 0 we guarantee that every variable is initialized
  memset(&settings, 0, sizeof(EepromSettingsStruct));
  for(uint8_t i = 0; i < MaxNumReceivers; ++i){
    settings.RxCalibrationMax[i] = RSSI_ADC_READING_MAX;
    settings.RxCalibrationMin[i] = RSSI_ADC_READING_MIN;
    settings.RSSIthresholds[i] = 2048;
    settings.RXBand[i] = 0;
    settings.RXChannel[i] = i % 8;
    settings.RXfrequencies[i] = getFrequencyFromBandChannel(settings.RXBand[i], settings.RXChannel[i]);
  }

  settings.eepromVersionNumber = EEPROM_VERSION_NUMBER;
  settings.ADCVBATmode = INA219;
  settings.RXADCfilter = LPF_20Hz;
  settings.VBATcalibration = 1;
  settings.NumReceivers = 6;
  settings.WiFiProtocol = 1;
  settings.WiFiChannel = 1;

  settings.updateCRC();

  *this = settings;

  EEPROM.put(0, *this);
  EEPROM.commit();
}

crc_t EepromSettingsStruct::calcCRC() {
  crc_t crc = crc_init();
  crc = crc_update(crc, this, sizeof(*this) - sizeof(this->crc));
  crc = crc_finalize(crc);
  return crc;
}

void EepromSettingsStruct::updateCRC() {
  this->crc = this->calcCRC();
}

bool EepromSettingsStruct::validateCRC(){
  return this->crc == this->calcCRC();
}

RXADCfilter_ getRXADCfilter() {
  return EepromSettings.RXADCfilter;
}

ADCVBATmode_ getADCVBATmode() {
  return EepromSettings.ADCVBATmode;
}

void setRXADCfilter(RXADCfilter_ filter) {
  EepromSettings.RXADCfilter = filter;
}

void setADCVBATmode(ADCVBATmode_ mode) {
  EepromSettings.ADCVBATmode = mode;
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

uint8_t getNumReceivers() {
  return EepromSettings.NumReceivers;
}

uint32_t getDisplayTimeout() {
  return EepromSettings.display_timeout_ms;
}
