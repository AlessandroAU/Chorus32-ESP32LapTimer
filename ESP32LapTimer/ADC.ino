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
FilterBeLp2 Filter5;
FilterBeLp2 Filter6;

FilterBeLp2Slow FilterVBAT;

Adafruit_INA219 ina219; // A0+A1=GND

uint32_t ADCstartMicros;
uint32_t ADCfinishMicros;
uint16_t ADCcaptime;

uint32_t LastADCcall;


bool HTTPupdating;

uint32_t cp0_regs[18];

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

//void IRAM_ATTR ShiftArray(uint16_t data[], int length) {
//  for (int i = length; i  >= 0; i--) {
//    data[i + 1] = data[i];
//  }
//}

SemaphoreHandle_t xBinarySemaphore;

bool BeginADCReading = false;
bool ADCreadingBusy = false;
byte currentADCpin = 0;
bool ADCattached[6] = {false, false, false, false, false, false,};

void IRAM_ATTR nbADCread( void * pvParameters ) {

  Serial.println("Task created...");

  while (1) {

    xSemaphoreTake( xBinarySemaphore, portMAX_DELAY );
    uint32_t now = micros();
    //Serial.print(now - LastADCcall);
    //Serial.print(",");
    ADCstartMicros = now;
    LastADCcall = now;

    adcAttachPin(ADC1_GPIO);
    adcStart(ADC1_GPIO);
    while (adcBusy(ADC1_GPIO)) {
      NOP();
    }
    ADC1ReadingRAW = 2 * adcEnd(ADC1_GPIO); //don't know why 2x is needed here!

    adcAttachPin(ADC2_GPIO);
    adcStart(ADC2_GPIO);
    while (adcBusy(ADC2_GPIO)) {
      NOP();
    }
    ADC2ReadingRAW = 2 * adcEnd(ADC2_GPIO); //don't know why 2x is needed here!

    adcAttachPin(ADC3_GPIO);
    adcStart(ADC3_GPIO);
    while (adcBusy(ADC3_GPIO)) {
      NOP();
    }
    ADC3ReadingRAW = 2 * adcEnd(ADC3_GPIO); //don't know why 2x is needed here!


    adcAttachPin(ADC4_GPIO);
    adcStart(ADC4_GPIO);
    while (adcBusy(ADC4_GPIO)) {
      NOP();
    }
    ADC4ReadingRAW = 2 * adcEnd(ADC4_GPIO); //don't know why 2x is needed here!


    adcAttachPin(ADC5_GPIO);
    adcStart(ADC5_GPIO);
    while (adcBusy(ADC5_GPIO)) {
      NOP();
    }
    ADC5ReadingRAW = 2 * adcEnd(ADC5_GPIO); //don't know why 2x is needed here!


    adcAttachPin(ADC6_GPIO);
    adcStart(ADC6_GPIO);
    while (adcBusy(ADC6_GPIO)) {
      NOP();
    }
    ADC6ReadingRAW = 2 * adcEnd(ADC6_GPIO); //don't know why 2x is needed here!

    ADCvalues[0] = Filter1.step(ADC1ReadingRAW);
    ADCvalues[1] = Filter2.step(ADC2ReadingRAW);
    ADCvalues[2] = Filter3.step(ADC3ReadingRAW);
    ADCvalues[3] = Filter4.step(ADC4ReadingRAW);
    ADCvalues[4] = Filter5.step(ADC5ReadingRAW);
    ADCvalues[5] = Filter6.step(ADC6ReadingRAW);


#ifdef VbatADC
    VbatReadingSmooth = ADCvalues[4];
#endif

    //ADCcaptime = micros() - ADCstartMicros;
    // Serial.println(ADCcaptime);

  }
}


void StartNB_ADCread() {
  Serial.println("Starting ADC reading task on core 1");
  xTaskCreatePinnedToCore(
    nbADCread,   /* Function to implement the task */
    "ADCreader", /* Name of the task */
    10000,      /* Stack size in words */
    NULL,       /* Task input parameter */
    -1,          /* Priority of the task */
    NULL,
    0);       /* Task handle. */
  //1);  /* Core where the task should run */
}


void ReadVBAT() {
  VbatReadingFloat = ina219.getBusVoltage_V() + (ina219.getShuntVoltage_mV() / 1000);
  Serial.print("VbatReading = ");
  Serial.println(VbatReadingFloat);
  
  mAReadingFloat = ina219.getCurrent_mA();
  Serial.print("mAReadingFloat = ");
  Serial.println(mAReadingFloat);
}

void IRAM_ATTR readADCs() {

  //  if (HTTPupdating || eepromSaveRquired) {
  //    return;
  //  }
  //
  //  if (ADCreadingBusy == false) {
  //    BeginADCReading = true;
  //    Serial.print(".");
  //  }

  static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  /* un-block the interrupt processing task now */
  xSemaphoreGiveFromISR( xBinarySemaphore, &xHigherPriorityTaskWoken );
  if ( xHigherPriorityTaskWoken) {
    portYIELD_FROM_ISR(); // this wakes up sample_timer_task immediately
  }

  //  ADCstartMicros = micros();

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

  //  ADC1ReadingRAW = adc1_get_raw(ADC1);
  //  ADC2ReadingRAW = adc1_get_raw(ADC2);
  //  ADC3ReadingRAW = adc1_get_raw(ADC3);
  //  ADC4ReadingRAW = adc1_get_raw(ADC4);
  //
  //#ifdef VbatADC
  //  ADCVBATreadingRAW = adc1_get_raw(ADCVBAT);
  //  VbatReadingSmooth = FilterVBAT.step(ADCVBATreadingRAW);
  //#endif

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
  //
  //  ADCvalues[0] = Filter1.step(ADC1ReadingRAW);
  //  ADCvalues[1] = Filter2.step(ADC2ReadingRAW);
  //  ADCvalues[2] = Filter3.step(ADC3ReadingRAW);
  //  ADCvalues[3] = Filter4.step(ADC4ReadingRAW);




  if (raceMode > 0) {
    CheckRSSIthresholdExceeded();
  }
  //
  //  SampleArrayMillisOffset = millis();
  //
  //  ADCfinishMicros = micros();
  //
  //  ADCcaptime = ADCfinishMicros - ADCstartMicros;

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
  adc1_config_channel_atten(ADC5, ADC_ATTEN_6db);
  adc1_config_channel_atten(ADC6, ADC_ATTEN_6db);

  for (int i = 0; i < NumRecievers; i++) {
    RSSIthresholds[i] = EepromSettings.RSSIthresholds[i];
  }

  ina219.begin();
  ReadVBAT();

}

void InitADCtimer() {


  xBinarySemaphore = xSemaphoreCreateBinary();
  StartNB_ADCread();

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &readADCs, true);
  timerAlarmWrite(timer, 1000, true);
  timerAlarmEnable(timer);

}

void StopADCtimer() {

  //timerAttachInterrupt(timer, NULL, true); //This doesn't work, not sure why.
  HTTPupdating = true;

}
