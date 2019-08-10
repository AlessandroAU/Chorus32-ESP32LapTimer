#pragma once
#include <stdint.h>
#include <Arduino.h>

#define BOARD_DEFAULT 1
#define BOARD_OLD 2
#define BOARD_TTGO_LORA 3

void InitHardwarePins();

///Define the board used here
///For jye's PCB v2.x the value doesn't need to be changed
///If you are using v1 of jye's PCB or used the wiring diagram you'll need to change this to "BOARD_OLD"
///To define your own custom board take a look at the "targets" directory

#define BOARD BOARD_DEFAULT


// DO NOT CHANGE BELOW UNLESS USING CUSTOM HARDWARE

#define EEPROM_VERSION_NUMBER 9 // Increment when eeprom struct modified

#define WIFI_AP_NAME "Chorus32 LapTimer"
#define BLUETOOTH_NAME WIFI_AP_NAME

#define MaxNumReceivers 6

#define VOLTAGE_UPDATE_INTERVAL_MS 1000

#define MIN_TUNE_TIME 30000 // value in micro seconds

#define OLED //uncomment this to enable OLED support

#define MAX_UDP_CLIENTS 5
#define MAX_LAPS_NUM 100 // Maximum number of supported laps per pilot

//#define USE_BLUETOOTH // Disabled by default. If you enable it you might need to change the partition scheme to "Huge APP"
#define USE_SERIAL_OUTPUT

// 800 and 2700 are about average min max raw values
#define RSSI_ADC_READING_MAX 2700
#define RSSI_ADC_READING_MIN 800

#include "targets/target.h" // Needs to be at the bottom

#if defined(BUTTON1) && defined(BUTTON2)
#define USE_BUTTONS
#endif

// Define unconfigured pins
#ifndef SCK
#define SCK -1
#endif
#ifndef MOSI
#define MOSI -1
#endif
#ifndef MISO
#define MISO -1
#endif
