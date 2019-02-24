#define EEPROM_SIZE 64

#include "EEPROM.h"

///eeprom not used yet

extern uint16_t RXfrequencies[NumRecievers];
extern volatile uint8_t RXBand[NumRecievers];
extern volatile uint8_t RXChannel[NumRecievers];
//extern int RSSIthresholds;

////////////////EEPROM ADDRESS LIST/////////
#define EEPROMaddrVRXchannel 0x00
#define EEPROMaddrVRXband 0x10
#define EEPROMaddrVRXthreshold 0x20

//
//void ReadEEPROMsettings() {
//
//
//}
//
//void SaveEEPROMsettings() {
//  for (int i = 0; i < NumRecievers; i ++) {
//    EEPROM.write(EEPROMaddrVRXchannel + i, RXChannel[i]);
//    EEPROM.write(EEPROMaddrVRXband + i, RXBand[i]);
//  }
//
//  for (int i = 0; i < NumRecievers * 2; i + 2) {
//    byte lowbyte;
//    byte highbyte;
//
//    lowbyte = RSSIthresholds[i-1] & 0xFF;
//
//  }
//}
