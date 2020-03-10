#pragma once

/// These are all the available targets
#define BOARD_DEFAULT 1
#define BOARD_OLD 2
#define BOARD_TTGO_LORA 3

///Define the board used here
///For jye's PCB v2.x the value doesn't need to be changed
///If you are using v1 of jye's PCB or used the wiring diagram you'll need to change this to "BOARD_OLD"
///To define your own custom board take a look at the "targets" directory
#ifndef BOARD
#define BOARD BOARD_DEFAULT
#endif

/// If your setup doesn't use an OLED remove or comment the following line
#define OLED

///  Sets the WiFi acces point name
#define WIFI_AP_NAME "Chorus32 LapTimer"
#define BLUETOOTH_NAME WIFI_AP_NAME

/// Enables Bluetooth support. Disabled by default. If you enable it you might need to change the partition scheme to "Huge APP"
//#define USE_BLUETOOTH
/// Outputs all messages on the serial port. Used to use Livetime via USB
#define USE_SERIAL_OUTPUT

// Enable TCP support. Currently this needs a special version of the app: https://github.com/Smeat/Chorus-RF-Laptimer/releases/tag/tcp_support
//#define USE_TCP

// BELOW ARE THE ADVANCED SETTINGS! ONLY CHANGE THEM IF YOU KNOW WHAT YOUR ARE DOING!

#define EEPROM_VERSION_NUMBER 9 // Increment when eeprom struct modified
#define MaxNumReceivers 6
#define VOLTAGE_UPDATE_INTERVAL_MS 1000 // interval of the battery voltage reading
#define MIN_TUNE_TIME 30000 // value in micro seconds
#define MAX_UDP_CLIENTS 5
#define MAX_TCP_CLIENTS 5
#define MAX_LAPS_NUM 100 // Maximum number of supported laps per pilot
// 800 and 2700 are about average min max raw values
#define RSSI_ADC_READING_MAX 2700
#define RSSI_ADC_READING_MIN 800

#include "targets/target.h" // Needs to be at the bottom

void InitHardwarePins();
