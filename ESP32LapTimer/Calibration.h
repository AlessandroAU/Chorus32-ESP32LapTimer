#include "ADC.h"
#include "HardwareConfig.h"
#include "RX5808.h"
#include "settings_eeprom.h"

int calibrationFreqIndex = 0;
bool isCurrentlyCalibrating = false;

void rssiCalibration() {

  for (uint8_t i = 0; i < NumRecievers; i++) {
    EepromSettings.RxCalibrationMin[i] = 5000;
    EepromSettings.RxCalibrationMax[i] = 0;
  }
  
  isCurrentlyCalibrating = true;
  calibrationFreqIndex = 0;
  setModuleFrequencyAll(channelFreqTable[calibrationFreqIndex]);
  hasRSSIReadingRefreshed = false;
  
}

void rssiCalibrationUpdate() {

  if (isCurrentlyCalibrating && hasRSSIReadingRefreshed) {
    
    for (uint8_t i = 0; i < NumRecievers; i++) {
      if (ADCReadingsRAW[i] < EepromSettings.RxCalibrationMin[i])
        EepromSettings.RxCalibrationMin[i] = ADCReadingsRAW[i];
        
      if (ADCReadingsRAW[i] > EepromSettings.RxCalibrationMax[i])
        EepromSettings.RxCalibrationMax[i] = ADCReadingsRAW[i];
    }
  
    calibrationFreqIndex++;
    
    if (calibrationFreqIndex < 8*8) { // 8*8 = 8 bands * 8 channels = total number of freq in channelFreqTable.
      
      setModuleFrequencyAll(channelFreqTable[calibrationFreqIndex]);
      hasRSSIReadingRefreshed = false;
      
    } else {
      
      for (int i = 0; i < NumRecievers; i++) {
        setModuleChannelBand(i);
      }
      
      isCurrentlyCalibrating = false;
      eepromSaveRquired = true;
      displayScreenNumber = 0;
      
    }  
                 
  }
  
}
