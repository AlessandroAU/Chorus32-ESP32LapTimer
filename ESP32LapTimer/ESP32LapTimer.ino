#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>

#include <esp_task_wdt.h>

#include "Comms.h"
#include "ADC.h"
#include "HardwareConfig.h"
#include "RX5808.h"
#include "Bluetooth.h"
#include "settings_eeprom.h"
#ifdef OLED
#include "OLED.h"
#endif
#include "WebServer.h"
#include "Beeper.h"
#include "Calibration.h"
#include "UDP.h"
#include "Buttons.h"
#include "WebServer.h"
#include "Watchdog.h"

//#define BluetoothEnabled //uncomment this to use bluetooth (experimental, ble + wifi appears to cause issues)

//
#define MAX_SRV_CLIENTS 5
WiFiClient serverClients[MAX_SRV_CLIENTS];

SemaphoreHandle_t adc_semaphore;

void IRAM_ATTR adc_read() {
  static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  /* un-block the interrupt processing task now */
  xSemaphoreGiveFromISR( adc_semaphore, &xHigherPriorityTaskWoken );
  if ( xHigherPriorityTaskWoken) {
    portYIELD_FROM_ISR(); // this wakes up sample_timer_task immediately
  }
}

void IRAM_ATTR adc_task(void* args) {
  watchdog_add_task();
  while(42) {
    xSemaphoreTake( adc_semaphore, portMAX_DELAY );
    nbADCread(NULL);
    watchdog_feed();
  }
}


void setup() {

#ifdef OLED
  oledSetup();
#endif

  Serial.begin(115200);
  Serial.println("Booting....");
  newButtonSetup();

  EepromSettings.setup();

  delay(500);
  InitHardwarePins();
  ConfigureADC();

  InitSPI();
  //PowerDownAll(); // Powers down all RX5808's
  delay(250);

  InitWifiAP();

  InitWebServer();

  if (!EepromSettings.SanityCheck()) {
    EepromSettings.defaults();
    Serial.println("Detected That EEPROM corruption has occured.... \n Resetting EEPROM to Defaults....");
  }

  setRXADCfilter(EepromSettings.RXADCfilter);
  setADCVBATmode(EepromSettings.ADCVBATmode);
  setVbatCal(EepromSettings.VBATcalibration);
  NumRecievers = EepromSettings.NumRecievers;
  commsSetup();
  init_outputs();

  for (int i = 0; i < NumRecievers; i++) {
    setRSSIThreshold(i, EepromSettings.RSSIthresholds[i]);
  }

  adc_semaphore = xSemaphoreCreateBinary();
  hw_timer_t* adc_task_timer = timerBegin(0, 8, true);
  timerAttachInterrupt(adc_task_timer, &adc_read, true);
  timerAlarmWrite(adc_task_timer, 1667, true); // 6khz -> 1khz per adc channel
  timerAlarmEnable(adc_task_timer);
  xTaskCreatePinnedToCore(adc_task, "ADCreader", 4096, NULL, 1, NULL, 0);

  //SelectivePowerUp();

  // inits modules with defaults.  Loops 10 times  because some Rx modules dont initiate correctly.
  for (int i = 0; i < NumRecievers*10; i++) {
    setModuleChannelBand(i % NumRecievers);
  }

  //beep();
}

void loop() {
  rssiCalibrationUpdate();
  // touchMonitor(); // A function to monitor capacitive touch values, defined in buttons.ino

  //  if (shouldReboot) {  //checks if reboot is needed
  //    Serial.println("Rebooting...");
  //    delay(100);
  //    ESP.restart();
  //  }
  newButtonUpdate();
#ifdef OLED
  OLED_CheckIfUpdateReq();
#endif
  update_outputs();
  SendCurrRSSIloop();
  updateWifi();

  EepromSettings.save();
  if (getADCVBATmode() == INA219) {
    ReadVBAT_INA219();
  }
  beeperUpdate();
  if(!isInRaceMode()) {
    thresholdModeStep();
  }
}
