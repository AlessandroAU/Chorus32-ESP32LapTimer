/*
 * This file is part of Chorus32-ESP32LapTimer 
 * (see https://github.com/AlessandroAU/Chorus32-ESP32LapTimer).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <WiFi.h>
#include <ESPmDNS.h>

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
#include "TimerWebServer.h"
#include "Beeper.h"
#include "Calibration.h"
#include "Output.h"
#ifdef USE_BUTTONS
#include "Buttons.h"
#endif
#include "Watchdog.h"
#include "Utils.h"
#include "Laptime.h"
#include "Wireless.h"

#include "CrashDetection.h"
#ifdef USE_ARDUINO_OTA
#include <ArduinoOTA.h>
#endif

//#define BluetoothEnabled //uncomment this to use bluetooth (experimental, ble + wifi appears to cause issues)

static TaskHandle_t adc_task_handle = NULL;

void IRAM_ATTR adc_read() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  /* un-block the interrupt processing task now */
  vTaskNotifyGiveFromISR(adc_task_handle, &xHigherPriorityTaskWoken);
  if(xHigherPriorityTaskWoken) {
    portYIELD_FROM_ISR();
  }
}

void IRAM_ATTR adc_task(void* args) {
  watchdog_add_task();
  while(42) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    nbADCread(NULL);
    watchdog_feed();
  }
}

void setup() {
  init_crash_detection();

  Serial.begin(115200);
  Serial.println("Booting....");
#ifdef USE_ARDUINO_OTA
  if(is_crash_mode()) {
    log_e("Detected crashing. Starting ArduinoOTA only!");
    InitWifiAP();
    ArduinoOTA.begin();
    return;
  }
#endif
#ifdef OLED
  oledSetup();
#endif
#ifdef USE_BUTTONS
  newButtonSetup();
#endif

  EepromSettings.setup();

  delay(500);
  InitHardwarePins();
  ConfigureADC();

  InitSPI();
  //PowerDownAll(); // Powers down all RX5808's
  delay(250);

  InitWifi();

  InitWebServer();

  if (!EepromSettings.SanityCheck()) {
    EepromSettings.defaults();
    Serial.println("Detected That EEPROM corruption has occured.... \n Resetting EEPROM to Defaults....");
  }

  setRXADCfilter(EepromSettings.RXADCfilter);
  setADCVBATmode(EepromSettings.ADCVBATmode);
  setVbatCal(EepromSettings.VBATcalibration);
  commsSetup();
  init_outputs();

  for (int i = 0; i < getNumReceivers(); i++) {
    setRSSIThreshold(i, EepromSettings.RSSIthresholds[i]);
  }

  xTaskCreatePinnedToCore(adc_task, "ADCreader", 4096, NULL, 1, &adc_task_handle, 0);
  hw_timer_t* adc_task_timer = timerBegin(0, 8, true);
  timerAttachInterrupt(adc_task_timer, &adc_read, true);
  timerAlarmWrite(adc_task_timer, 1667, true); // 6khz -> 1khz per adc channel
  timerAlarmEnable(adc_task_timer);

  //SelectivePowerUp();

  // inits modules with defaults.  Loops 10 times  because some Rx modules dont initiate correctly.
  for (int i = 0; i < getNumReceivers()*10; i++) {
    setModuleChannelBand(i % getNumReceivers());
  }

  //beep();
}

void loop() {
#ifdef USE_ARDUINO_OTA
  ArduinoOTA.handle();
  if(is_crash_mode()) return;
#endif
  if(millis() > CRASH_COUNT_RESET_TIME_MS) {
    reset_crash_count();
  }
  rssiCalibrationUpdate();
  // touchMonitor(); // A function to monitor capacitive touch values, defined in buttons.ino

  //  if (shouldReboot) {  //checks if reboot is needed
  //    Serial.println("Rebooting...");
  //    delay(100);
  //    ESP.restart();
  //  }
#ifdef USE_BUTTONS
  newButtonUpdate();
#endif
#ifdef OLED
  // We need to pause the OLED during update otherwise we crash due to I2C
  if(!isUpdating()) {
    OLED_CheckIfUpdateReq();
  }
#endif
  sendNewLaps();
  update_outputs();
  SendCurrRSSIloop();

#ifdef WIFI_MODE_ACCESSPOINT
  handleDNSRequests();
#endif

  EepromSettings.save();
  beeperUpdate();
  if(UNLIKELY(!isInRaceMode())) {
    thresholdModeStep();
  }
}
