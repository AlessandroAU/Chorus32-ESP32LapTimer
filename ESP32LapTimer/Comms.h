#ifndef COMMS_H
#define COMMS_H

#include "HardwareConfig.h"

void HandleSerialRead();
void HandleServerUDP();
void SendCurrRSSIloop();
void IRAM_ATTR sendLap(uint8_t Lap, uint8_t NodeAddr);
void commsSetup();
void thresholdModeStep();

#endif
