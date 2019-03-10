////Functions to Read RSSI from ADCs//////
#include <driver/adc.h>
#include <driver/timer.h>
#include "HardwareConfig.h"
#include "Comms.h"
#include <Wire.h>
#include <Adafruit_INA219.h>
#include "settings_eeprom.h"
#include "ADC.h"
#include "Timer.h"
#include "UDP.h"

Timer ina219Timer = Timer(1000);

extern RXADCfilter_ RXADCfilter;
extern ADCVBATmode_ ADCVBATmode;
extern byte NumRecievers;


FilterBeLp2Slow FilterVBAT; //slow filter for VBAT readings

extern RXADCfilter_ RXADCfilter; //variable to hold which filter we use.

Adafruit_INA219 ina219; // A0+A1=GND

uint32_t ADCstartMicros;
uint32_t ADCfinishMicros;
uint16_t ADCcaptime;

uint32_t LastADCcall;


bool HTTPupdating;

uint32_t cp0_regs[18];

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

SemaphoreHandle_t xBinarySemaphore;

bool BeginADCReading = false;
bool ADCreadingBusy = false;
byte currentADCpin = 0;

void ConfigureADC() {

  adc1_config_width(ADC_WIDTH_BIT_12);

  adc1_config_channel_atten(ADC1, ADC_ATTEN_6db);
  adc1_config_channel_atten(ADC2, ADC_ATTEN_6db);
  adc1_config_channel_atten(ADC3, ADC_ATTEN_6db);
  adc1_config_channel_atten(ADC4, ADC_ATTEN_6db);
  adc1_config_channel_atten(ADC5, ADC_ATTEN_6db);
  adc1_config_channel_atten(ADC6, ADC_ATTEN_6db);

  ina219.begin();
  ReadVBAT_INA219();

}

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
    ADCReadingsRAW[0] = 2 * adcEnd(ADC1_GPIO); //don't know why 2x is needed here!

    adcAttachPin(ADC2_GPIO);
    adcStart(ADC2_GPIO);
    while (adcBusy(ADC2_GPIO)) {
      NOP();
    }
    ADCReadingsRAW[1] = 2 * adcEnd(ADC2_GPIO); //don't know why 2x is needed here!

    adcAttachPin(ADC3_GPIO);
    adcStart(ADC3_GPIO);
    while (adcBusy(ADC3_GPIO)) {
      NOP();
    }
    ADCReadingsRAW[2] = 2 * adcEnd(ADC3_GPIO); //don't know why 2x is needed here!


    adcAttachPin(ADC4_GPIO);
    adcStart(ADC4_GPIO);
    while (adcBusy(ADC4_GPIO)) {
      NOP();
    }
    ADCReadingsRAW[3] = 2 * adcEnd(ADC4_GPIO); //don't know why 2x is needed here!


    adcAttachPin(ADC5_GPIO);
    adcStart(ADC5_GPIO);
    while (adcBusy(ADC5_GPIO)) {
      NOP();
    }
    ADCReadingsRAW[4] = 2 * adcEnd(ADC5_GPIO); //don't know why 2x is needed here!


    adcAttachPin(ADC6_GPIO);
    adcStart(ADC6_GPIO);
    while (adcBusy(ADC6_GPIO)) {
      NOP();
    }
    ADCReadingsRAW[5] = 2 * adcEnd(ADC6_GPIO); //don't know why 2x is needed here!

    switch (RXADCfilter) {

      case LPF_10Hz:
        for (int i = 0; i < 6; i++) {
          ADCvalues[i] = Filter_10HZ[i].step(ADCReadingsRAW[i]);
        }
        break;

      case LPF_20Hz:
        for (int i = 0; i < 6; i++) {
          ADCvalues[i] = Filter_20HZ[i].step(ADCReadingsRAW[i]);
        }
        break;

      case LPF_50Hz:
        for (int i = 0; i < 6; i++) {
          ADCvalues[i] = Filter_50HZ[i].step(ADCReadingsRAW[i]);
        }
        break;

      case LPF_100Hz:
        for (int i = 0; i < 6; i++) {
          ADCvalues[i] = Filter_100HZ[i].step(ADCReadingsRAW[i]);
        }
        break;
    }

    switch (ADCVBATmode) {
      case ADC_CH5:
        VbatReadingSmooth = ADCvalues[4];
        break;
      case ADC_CH6:
        VbatReadingSmooth = ADCvalues[5];
        break;
    }

    if (raceMode > 0) {
      CheckRSSIthresholdExceeded();
    }


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


void ReadVBAT_INA219() {
  if (ina219Timer.hasTicked()) {
    VbatReadingFloat = ina219.getBusVoltage_V() + (ina219.getShuntVoltage_mV() / 1000);
//    Serial.print("VbatReading = ");
//    Serial.println(VbatReadingFloat);

    mAReadingFloat = ina219.getCurrent_mA();
//    Serial.print("mAReadingFloat = ");
//    Serial.println(mAReadingFloat);

    ina219Timer.reset();
  }
}

void IRAM_ATTR readADCs() {

  static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  /* un-block the interrupt processing task now */
  xSemaphoreGiveFromISR( xBinarySemaphore, &xHigherPriorityTaskWoken );
  if ( xHigherPriorityTaskWoken) {
    portYIELD_FROM_ISR(); // this wakes up sample_timer_task immediately
  }
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
}
