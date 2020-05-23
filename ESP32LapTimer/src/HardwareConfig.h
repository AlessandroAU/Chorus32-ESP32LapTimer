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
#pragma once

#include <stdint.h>

/// These are all the available targets
#define BOARD_DEFAULT 1
#define BOARD_OLD 2
#define BOARD_TTGO_LORA 3
#define BOARD_WROOM 4

///Define the board used here
///For jye's PCB v2.x the value doesn't need to be changed
///If you are using v1 of jye's PCB or used the wiring diagram you'll need to change this to "BOARD_OLD"
///To define your own custom board take a look at the "targets" directory
#ifndef BOARD
#define BOARD BOARD_DEFAULT
#endif

/// If your setup doesn't use an OLED remove or comment the following line
#define OLED

// Selects the wifi mode to operate in.
// One of these must be uncommented.
//
#define WIFI_MODE_ACCESSPOINT
// For now the AP name needs to be defined regardless of mode.
#define WIFI_AP_NAME "Chorus32 LapTimer"

// When in client mode you also need to specify the
// ssid and password.
//#define WIFI_MODE_CLIENT
// For now the ssid and password needs to be defined regardless of mode
#define WIFI_SSID "testnetwork"
#define WIFI_PASSWORD "testpassword"

/// Enables Bluetooth support. Disabled by default. If you enable it you might need to change the partition scheme to "Huge APP"
//#define USE_BLUETOOTH
// For now the bluetooth name needs to be defined regardless of if it's enabled or not
#define BLUETOOTH_NAME WIFI_AP_NAME
/// Outputs all messages on the serial port. Used to use Livetime via USB
#define USE_SERIAL_OUTPUT

// Enable TCP support. Currently this needs a special version of the app: https://github.com/Smeat/Chorus-RF-Laptimer/releases/tag/tcp_support
//#define USE_TCP

// Enables the ArduinoOTA service. It allows flashing over WiFi and enters an emergency mode if a crashloop is detected.
//#define USE_ARDUINO_OTA

// BELOW ARE THE ADVANCED SETTINGS! ONLY CHANGE THEM IF YOU KNOW WHAT YOUR ARE DOING!

#define EEPROM_VERSION_NUMBER 9 // Increment when eeprom struct modified
#define MAX_NUM_RECEIVERS 6
#define VOLTAGE_UPDATE_INTERVAL_MS 1000 // interval of the battery voltage reading
#define MIN_TUNE_TIME 30000 // value in micro seconds
#define MAX_UDP_CLIENTS 5
#define MAX_TCP_CLIENTS 5
#define MAX_LAPS_NUM 100 // Maximum number of supported laps per pilot
// 800 and 2700 are about average min max raw values
#define RSSI_ADC_READING_MAX 2700
#define RSSI_ADC_READING_MIN 800
// defines the time after which the crash loop detection assumes the operation is stable
#define CRASH_COUNT_RESET_TIME_MS 300000

#include "targets/target.h" // Needs to be at the bottom

void InitHardwarePins();
extern uint8_t CS_PINS[MAX_NUM_RECEIVERS];
