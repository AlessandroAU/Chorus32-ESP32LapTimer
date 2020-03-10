////Functions to Read RSSI from ADCs//////
#include <driver/adc.h>
#include <driver/timer.h>
#include <esp_adc_cal.h>

#include <Wire.h>
#include <Adafruit_INA219.h>

#include "HardwareConfig.h"
#include "Comms.h"
#include "settings_eeprom.h"
#include "ADC.h"
#include "Timer.h"
#include "Output.h"
#include "Calibration.h"
#include "Laptime.h"
#include "Utils.h"

static Adafruit_INA219 ina219; // A0+A1=GND

static uint32_t LastADCcall;

static esp_adc_cal_characteristics_t adc_chars;

static int RSSIthresholds[MaxNumReceivers];
static uint16_t ADCReadingsRAW[MaxNumReceivers];
static unsigned int VbatReadingSmooth;
static int ADCvalues[MaxNumReceivers];
static uint16_t adcLoopCounter = 0;

static FilterBeLp2_10HZ Filter_10HZ[6] = {FilterBeLp2_10HZ(), FilterBeLp2_10HZ(), FilterBeLp2_10HZ(), FilterBeLp2_10HZ(), FilterBeLp2_10HZ(), FilterBeLp2_10HZ()};
static FilterBeLp2_20HZ Filter_20HZ[6] = {FilterBeLp2_20HZ(), FilterBeLp2_20HZ(), FilterBeLp2_20HZ(), FilterBeLp2_20HZ(), FilterBeLp2_20HZ(), FilterBeLp2_20HZ()};
static FilterBeLp2_50HZ Filter_50HZ[6] = {FilterBeLp2_50HZ(), FilterBeLp2_50HZ(), FilterBeLp2_50HZ(), FilterBeLp2_50HZ(), FilterBeLp2_50HZ(), FilterBeLp2_50HZ()};
static FilterBeLp2_100HZ Filter_100HZ[6] = {FilterBeLp2_100HZ(), FilterBeLp2_100HZ(), FilterBeLp2_100HZ(), FilterBeLp2_100HZ(), FilterBeLp2_100HZ(), FilterBeLp2_100HZ()};

static float VBATcalibration;
static float mAReadingFloat;
static float VbatReadingFloat;

static uint16_t multisample_adc1(adc1_channel_t channel, uint8_t samples) {
  uint32_t val = 0;
  for(uint8_t i = 0; i < samples; ++i) {
    val += adc1_get_raw(channel);
  }
  return val/samples;
}

void ConfigureADC() {

  adc1_config_width(ADC_WIDTH_BIT_12);

  adc1_config_channel_atten(ADC1, ADC_ATTEN_6db);
  adc1_config_channel_atten(ADC2, ADC_ATTEN_6db);
  adc1_config_channel_atten(ADC3, ADC_ATTEN_6db);
  adc1_config_channel_atten(ADC4, ADC_ATTEN_6db);
  adc1_config_channel_atten(ADC5, ADC_ATTEN_6db);
  adc1_config_channel_atten(ADC6, ADC_ATTEN_6db);

  //since the reference voltage can range from 1000mV to 1200mV we are using 1100mV as a default
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_6db, ADC_WIDTH_BIT_12, 1100, &adc_chars);

  ina219.begin();
  ReadVBAT_INA219();

}

void IRAM_ATTR nbADCread( void * pvParameters ) {
  static uint8_t current_adc = 0;
  if(current_adc == 0) ++adcLoopCounter; // only count adc number 0 to get the frequency per module

  uint32_t now = micros();
  LastADCcall = now;
  adc1_channel_t channel = ADC1;
  switch (current_adc) {
    case 0:
      channel = ADC1;
      break;
    case 1:
      channel = ADC2;
      break;
    case 2:
      channel = ADC3;
      break;
    case 3:
      channel = ADC4;
      break;
    case 4:
      channel = ADC5;
      break;
    case 5:
      channel = ADC6;
      break;
  }

  if(LIKELY(isInRaceMode())) {
    ADCReadingsRAW[current_adc] = adc1_get_raw(channel);
  } else {
    // multisample when not in race mode (for threshold calibration etc)
    ADCReadingsRAW[current_adc] = multisample_adc1(channel, 10);
  }

  // Applying calibration
  if (LIKELY(!isCalibrating())) {
    uint16_t rawRSSI = constrain(ADCReadingsRAW[current_adc], EepromSettings.RxCalibrationMin[current_adc], EepromSettings.RxCalibrationMax[current_adc]);
    ADCReadingsRAW[current_adc] = map(rawRSSI, EepromSettings.RxCalibrationMin[current_adc], EepromSettings.RxCalibrationMax[current_adc], RSSI_ADC_READING_MIN, RSSI_ADC_READING_MAX);
  }

  switch (getRXADCfilter()) {
    case LPF_10Hz:
      ADCvalues[current_adc] = Filter_10HZ[current_adc].step(ADCReadingsRAW[current_adc]);
      break;
    case LPF_20Hz:
      ADCvalues[current_adc] = Filter_20HZ[current_adc].step(ADCReadingsRAW[current_adc]);
      break;
    case LPF_50Hz:
      ADCvalues[current_adc] = Filter_50HZ[current_adc].step(ADCReadingsRAW[current_adc]);
      break;
    case LPF_100Hz:
      ADCvalues[current_adc] = Filter_100HZ[current_adc].step(ADCReadingsRAW[current_adc]);
      break;
  }

  if (LIKELY(isInRaceMode() > 0)) {
    CheckRSSIthresholdExceeded(current_adc);
  }
  current_adc = (current_adc + 1) % MaxNumReceivers;
}


void ReadVBAT_INA219() {
  setVbatFloat(ina219.getBusVoltage_V() + (ina219.getShuntVoltage_mV() / 1000));
  mAReadingFloat = ina219.getCurrent_mA();
}

void IRAM_ATTR CheckRSSIthresholdExceeded(uint8_t node) {
  uint32_t CurrTime = millis();
  if ( ADCvalues[node] > RSSIthresholds[node]) {
    if (CurrTime > (getMinLapTime() + getLaptime(node))) {
      addLap(node, CurrTime);
    }
  }
}

uint16_t getRSSI(uint8_t index) {
  if(index < MaxNumReceivers) {
    return ADCvalues[index];
  }
  return 0;
}

void setRSSIThreshold(uint8_t node, uint16_t threshold) {
  if(node < MaxNumReceivers) {
    RSSIthresholds[node] = threshold;
  }
}

uint16_t getRSSIThreshold(uint8_t node){
  return RSSIthresholds[node];
}

uint16_t getADCLoopCount() {
  return adcLoopCounter;
}

void setADCLoopCount(uint16_t count) {
  adcLoopCounter = count;
}

void setVbatCal(float calibration) {
  VBATcalibration = calibration;
}

float getMaFloat() {
  return mAReadingFloat;
}

float getVbatFloat(bool force_read){
  static uint32_t last_voltage_update = 0;
  if((millis() - last_voltage_update) > VOLTAGE_UPDATE_INTERVAL_MS || force_read) {
    switch (getADCVBATmode()) {
      case ADC_CH5:
        VbatReadingSmooth = esp_adc_cal_raw_to_voltage(adc1_get_raw(ADC5), &adc_chars);
        setVbatFloat(VbatReadingSmooth / 1000.0 * VBATcalibration);
        break;
      case ADC_CH6:
        VbatReadingSmooth = esp_adc_cal_raw_to_voltage(adc1_get_raw(ADC6), &adc_chars);
        setVbatFloat(VbatReadingSmooth / 1000.0 * VBATcalibration);
        break;
      case INA219:
        ReadVBAT_INA219();
      default:
        break;
    }
    last_voltage_update = millis();
  }
  return VbatReadingFloat;
}

void setVbatFloat(float val){
  VbatReadingFloat = val;
}

void setVBATcalibration(float val) {
  VBATcalibration = val;
}

float getVBATcalibration() {
  return VBATcalibration;
}
