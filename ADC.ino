////Functions to Read RSSI from ADCs//////
#include <driver/adc.h>
#include <driver/timer.h>
#include "HardwareConfig.h"
#include "Comms.h"

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
  VbatReading = adc1_get_raw(ADCvbat);
}

void IRAM_ATTR readADCs() {

  if (HTTPupdating == true) {
    return;
  }

  ADCstartMicros = micros();

  ShiftArray(ADC1readingsRAW, ADCmemLen);
  ADC1readingsRAW[0] =  adc1_get_raw(ADC1);

  ShiftArray(ADC2readingsRAW, ADCmemLen);
  ADC2readingsRAW[0] =  adc1_get_raw(ADC2);

  ShiftArray(ADC3readingsRAW, ADCmemLen);
  ADC3readingsRAW[0] =  adc1_get_raw(ADC3);

  ShiftArray(ADC4readingsRAW, ADCmemLen);
  ADC4readingsRAW[0] =  adc1_get_raw(ADC4);

  uint32_t cp_state = xthal_get_cpenable();

  if (cp_state) {   ///usually the FPU is disabled during an ISR but this makes it work again.
    // Save FPU registers
    xthal_save_cp0(cp0_regs);
  } else {
    // enable FPU
    xthal_set_cpenable(1);
  }

  SmoothValues(ADC1readingsRAW, ADC1readings, ADCmemLen, 0.80);  ///I would like to convert these floating point operations into integer math at some point
  SmoothValues(ADC2readingsRAW, ADC2readings, ADCmemLen, 0.80);
  SmoothValues(ADC3readingsRAW, ADC3readings, ADCmemLen, 0.80);
  SmoothValues(ADC4readingsRAW, ADC4readings, ADCmemLen, 0.80);

  if (cp_state) {
    // Restore FPU registers
    xthal_restore_cp0(cp0_regs);
  } else {
    // turn it back off
    xthal_set_cpenable(0);
  }

  ADCvalues[0] = ADC1readings[0];
  ADCvalues[1] = ADC2readings[0];
  ADCvalues[2] = ADC3readings[0];
  ADCvalues[3] = ADC4readings[0];


  if (raceMode > 0) {
    CheckRSSIthresholdExceeded();
  }

  SampleArrayMillisOffset = millis();

  ADCfinishMicros = micros();

  ADCcaptime = ADCfinishMicros - ADCstartMicros;

}

void SmoothValues(uint16_t data[], uint16_t dataOut[], int length, float ETAvalue) {
  uint16_t Output;
  for (int i = length; i > 0; i--) {
    Output = (uint16_t)(ETAvalue * data[i] + (1 - ETAvalue) * data[i - 1]);
    dataOut[i - 1] = Output;
  }
  //dataOut[0] = dataOut[1];
}

void ConfigureADC() {

  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1, ADC_ATTEN_6db);
  adc1_config_channel_atten(ADC2, ADC_ATTEN_6db);
  adc1_config_channel_atten(ADC3, ADC_ATTEN_6db);
  adc1_config_channel_atten(ADC4, ADC_ATTEN_6db);

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


