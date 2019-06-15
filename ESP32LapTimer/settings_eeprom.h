#pragma once 

#include "HardwareConfig.h"

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
  uint8_t RXBand[MaxNumRecievers];
  uint8_t RXChannel[MaxNumRecievers];
  uint16_t RXfrequencies[MaxNumRecievers];
  int RSSIthresholds[MaxNumRecievers];
  RXADCfilter_ RXADCfilter;
  ADCVBATmode_ ADCVBATmode;
  float VBATcalibration;
  uint8_t NumRecievers;
  uint16_t RxCalibrationMin[MaxNumRecievers];
  uint16_t RxCalibrationMax[MaxNumRecievers];


  void setup();
  void load();
  void save();
  void defaults();
  bool SanityCheck();
};

const struct {
  uint16_t eepromVersionNumber = EEPROM_VERSION_NUMBER;

  uint8_t RXBand[MaxNumRecievers] = {0, 0, 0, 0, 0, 0};
  uint8_t RXChannel[MaxNumRecievers] = {0, 1, 2, 3, 4, 5};
  uint16_t RXfrequencies[MaxNumRecievers] = {5658, 5695, 5732, 5769, 5806, 5843};
  int RSSIthresholds[MaxNumRecievers] = {2048, 2048, 2048, 2048, 2048, 2048};
  RXADCfilter_ RXADCfilter = LPF_20Hz;
  ADCVBATmode_ ADCVBATmode = INA219;
  float VBATcalibration = 1.000;
  uint8_t NumRecievers = 6;
  uint16_t RxCalibrationMin[MaxNumRecievers] = {800, 800, 800, 800, 800, 800};
  uint16_t RxCalibrationMax[MaxNumRecievers] = {2700, 2700, 2700, 2700, 2700, 2700};

} EepromDefaults;

extern EepromSettingsStruct EepromSettings;

RXADCfilter_ getRXADCfilter();
ADCVBATmode_ getADCVBATmode();

void setRXADCfilter(RXADCfilter_ filter);
void setADCVBATmode(ADCVBATmode_ mode);

void setSaveRequired();
