#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include "Comms.h"
#include "ADC.h"
#include "HardwareConfig.h"
#include "RX5808.h"
#include "Bluetooth.h"
#include "settings_eeprom.h"
#include "OLED.h"
#include "WebServer.h"
#include "Beeper.h"
#include "Calibration.h"

//#define BluetoothEnabled //uncomment this to use bluetooth (experimental, ble + wifi appears to cause issues)

WiFiUDP UDPserver;

WiFiUDP UDPserverDatalogger; //datalogging server 
//
#define MAX_SRV_CLIENTS 5
WiFiClient serverClients[MAX_SRV_CLIENTS];

volatile uint32_t LapTimes[MaxNumRecievers][100];
volatile int LapTimePtr[MaxNumRecievers] = {0, 0, 0, 0, 0, 0}; //Keep track of what lap we are up too

uint32_t MinLapTime = 5000;  //this is in millis

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

#ifdef BluetoothEnabled
  SerialBT.begin("Chorus Laptimer SPP");
#endif
  InitWifiAP();

  InitWebServer();

  if (!EepromSettings.SanityCheck()) {
    EepromSettings.defaults();
    Serial.println("Detected That EEPROM corruption has occured.... \n Resetting EEPROM to Defaults....");
  }

  RXADCfilter = EepromSettings.RXADCfilter;
  ADCVBATmode = EepromSettings.ADCVBATmode;
  VBATcalibration = EepromSettings.VBATcalibration;
  NumRecievers = EepromSettings.NumRecievers;
  commsSetup();

  for (int i = 0; i < NumRecievers; i++) {
    setRSSIThreshold(i, EepromSettings.RSSIthresholds[i]);
  }
  UDPserver.begin(9000);

  InitADCtimer();

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
  HandleSerialRead();
  HandleServerUDP();
  SendCurrRSSIloop();
  dnsServer.processNextRequest();

  //if (raceMode == 0) {
  //if (client.connected()) {
  webServer.handleClient();
  //}
  // }

#ifdef BluetoothEnabled
  HandleBluetooth();
#endif
  EepromSettings.save();

  if (ADCVBATmode == INA219) {
    ReadVBAT_INA219();
  }
  beeperUpdate();
  if(!raceMode) {
    thresholdModeStep();
  }
}
