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
  uint8_t RXBand[MaxNumReceivers];
  uint8_t RXChannel[MaxNumReceivers];
  uint16_t RXfrequencies[MaxNumReceivers];
  int RSSIthresholds[MaxNumReceivers];
  RXADCfilter_ RXADCfilter;
  ADCVBATmode_ ADCVBATmode;
  float VBATcalibration;
  uint8_t NumReceivers;
  uint16_t RxCalibrationMin[MaxNumReceivers];
  uint16_t RxCalibrationMax[MaxNumReceivers];
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
