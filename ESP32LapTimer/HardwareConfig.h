#pragma once
#include <stdint.h>
#include <Arduino.h>

#define BOARD_DEFAULT 1
#define BOARD_OLD 2
#define BOARD_TTGO_LORA 3

void InitHardwarePins();

///Define Pin configuration here, these are the defaults as given on github

#define BOARD BOARD_DEFAULT


// DO NOT CHANGE BELOW UNLESS USING CUSTOM HARDWARE

#define EEPROM_VERSION_NUMBER 6 // Increment when eeprom struct modified

#define MaxNumRecievers 6
extern byte NumRecievers;

#define MIN_TUNE_TIME 30000 // value in micro seconds

#define OLED //uncomment this to enable OLED support

#define MAX_UDP_CLIENTS 5


#include "targets/target.h" // Needs to be at the bottom
