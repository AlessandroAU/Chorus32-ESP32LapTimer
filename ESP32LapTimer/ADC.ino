////Functions to Read RSSI from ADCs//////
#include <driver/adc.h>
#include <driver/timer.h>
#include "HardwareConfig.h"
#include "Comms.h"
#include <Wire.h>
#include <Adafruit_INA219.h>
#include "settings_eeprom.h"
#include "Filter.h"

float VBATcalibration = 0.935;

FilterBeLp2 Filter1;
FilterBeLp2 Filter2;
FilterBeLp2 Filter3;
FilterBeLp2 Filter4;

FilterBeLp2Slow FilterVBAT;

Adafruit_INA219 ina219; // A0+A1=GND

uint32_t ADCstartMicros;
uint32_t ADCfinishMicros;
uint16_t ADCcaptime;

extern bool HTTPupdating;

uint32_t cp0_regs[18];

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR ShiftArray(uint16_t data[], int length) {
  for (int i = length; i  >= 0; i--) {
    data[i + 1] = data[i];
  }
}

void ReadVBAT() {
  VbatReadingFloat = ina219.getBusVoltage_V();
  Serial.print("VbatReading = ");
  Serial.println(VbatReadingFloat);
}

void IRAM_ATTR readADCs() {

  if (HTTPupdating || eepromSaveRquired) {
    return;
  }

  ADCstartMicros = micros();

  ////    ShiftArray(ADCVBATreadingsRAW, ADCmemLen); //This is all commented out because I'm testing another filtering method atm.
  //    ADCVBATreadingsRAW[0] = adc1_get_raw(ADCVBAT);
  //
  //  //ShiftArray(ADC1readingsRAW, ADCmemLen);
  //  ADC1readingsRAW[0] =  adc1_get_raw(ADC1);
  //
  //  //ShiftArray(ADC2readingsRAW, ADCmemLen);
  //  ADC2readingsRAW[0] =  adc1_get_raw(ADC2);
  //
  //  //ShiftArray(ADC3readingsRAW, ADCmemLen);
  //  ADC3readingsRAW[0] =  adc1_get_raw(ADC3);
  //
  //  //ShiftArray(ADC4readingsRAW, ADCmemLen);
  //  ADC4readingsRAW[0] =  adc1_get_raw(ADC4);

  ADC1ReadingRAW = adc1_get_raw(ADC1);
  ADC2ReadingRAW = adc1_get_raw(ADC2);
  ADC3ReadingRAW = adc1_get_raw(ADC3);
  ADC4ReadingRAW = adc1_get_raw(ADC4);

#ifdef VbatADC
  ADCVBATreadingRAW = adc1_get_raw(ADCVBAT);
  VbatReadingSmooth = FilterVBAT.step(ADCVBATreadingRAW);
#endif

  //  uint32_t cp_state = xthal_get_cpenable();
  //
  //  if (cp_state) {   ///usually the FPU is disabled during an ISR but this makes it work again.
  //    // Save FPU registers
  //    xthal_save_cp0(cp0_regs);
  //  } else {
  //    // enable FPU
  //    xthal_set_cpenable(1);
  //  }
  //
  //  SmoothValues(ADC1readingsRAW, ADC1readings, ADCmemLen, 0.50);  ///I would like to convert these floating point operations into integer math at some point
  //  SmoothValues(ADC2readingsRAW, ADC2readings, ADCmemLen, 0.50);
  //  SmoothValues(ADC3readingsRAW, ADC3readings, ADCmemLen, 0.50);
  //  SmoothValues(ADC4readingsRAW, ADC4readings, ADCmemLen, 0.50);
  //  SmoothValues(ADCVBATreadingsRAW, ADCVBATreadings, ADCmemLen, 0.01);
  //
  //  if (cp_state) {
  //    // Restore FPU registers
  //    xthal_restore_cp0(cp0_regs);
  //  } else {
  //    // turn it back off
  //    xthal_set_cpenable(0);
  //  }

  //    ADCvalues[0] = ADC1readings[0];
  //    ADCvalues[1] = ADC2readings[0];
  //    ADCvalues[2] = ADC3readings[0];
  //    ADCvalues[3] = ADC4readings[0];
  //
  //  ADCvalues[0] = ADC1readingsRAW[0];
  //  ADCvalues[1] = ADC2readingsRAW[0];
  //  ADCvalues[2] = ADC3readingsRAW[0];
  //  ADCvalues[3] = ADC4readingsRAW[0];

  ADCvalues[0] = Filter1.step(ADC1ReadingRAW);
  ADCvalues[1] = Filter2.step(ADC2ReadingRAW);
  ADCvalues[2] = Filter3.step(ADC3ReadingRAW);
  ADCvalues[3] = Filter4.step(ADC4ReadingRAW);




  if (raceMode > 0) {
    CheckRSSIthresholdExceeded();
  }

  SampleArrayMillisOffset = millis();

  ADCfinishMicros = micros();

  ADCcaptime = ADCfinishMicros - ADCstartMicros;

}

void IRAM_ATTR CheckRSSIthresholdExceeded() {
  uint32_t CurrTime = millis();

  for (uint8_t i = 0; i < NumRecievers; i++) {
    if ( ADCvalues[i] > RSSIthresholds[i]) {

      if (CurrTime > (MinLapTime + LapTimes[i][LapTimePtr[i]])) {

        LapTimePtr[i] = LapTimePtr[i] + 1;
        LapTimes[i][LapTimePtr[i]] = CurrTime;

        sendLap(LapTimePtr[i], i);
      }
    }
  }
}

//void SmoothValues(uint16_t data[], uint16_t dataOut[], int length, float ETAvalue) {
//  uint16_t Output;
//  for (int i = length; i > 0; i--) {
//    Output = (uint16_t)(ETAvalue * data[i] + (1 - ETAvalue) * data[i - 1]);
//    dataOut[i - 1] = Output;
//  }
//  //dataOut[0] = dataOut[1];
//}

void ConfigureADC() {

  adc1_config_width(ADC_WIDTH_BIT_12);

  adc1_config_channel_atten(ADC1, ADC_ATTEN_6db);
  adc1_config_channel_atten(ADC2, ADC_ATTEN_6db);
  adc1_config_channel_atten(ADC3, ADC_ATTEN_6db);
  adc1_config_channel_atten(ADC4, ADC_ATTEN_6db);

  adc1_config_channel_atten(ADCVBAT, ADC_ATTEN_6db);

  for (int i = 0; i < NumRecievers; i++) {
    RSSIthresholds[i] = EepromSettings.RSSIthresholds[i];
  }
  
  ina219.begin();
  ina219.setCalibration_16V_400mA();
  ReadVBAT();

}

void InitADCtimer() {

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &readADCs, true);
  timerAlarmWrite(timer, 1000, true);
  timerAlarmEnable(timer);

}

void StopADCtimer() {

  //timerAttachInterrupt(timer, NULL, true); //This doesn't work, not sure why.
  HTTPupdating = true;

}

