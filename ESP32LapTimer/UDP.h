#pragma once

#include <esp32-hal.h>

void IRAM_ATTR addToSendQueue(uint8_t item);
void IRAM_ATTR addToSendQueue(uint8_t * buff, uint8_t length);
void IRAM_ATTR SendUDPpacket();

void UDPinit();
