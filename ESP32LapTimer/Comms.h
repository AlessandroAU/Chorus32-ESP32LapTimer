#ifndef __COMMS_H__
#define __COMMS_H__

#include "HardwareConfig.h"

void HandleSerialRead();
void HandleServerUDP();
void SendCurrRSSIloop();
void IRAM_ATTR sendLap(uint8_t Lap, uint8_t NodeAddr);
void commsSetup();
void thresholdModeStep();
void handleSerialControlInput(char *controlData, uint8_t  ControlByte, uint8_t NodeAddr, uint8_t length);

#endif // __COMMS_H__
