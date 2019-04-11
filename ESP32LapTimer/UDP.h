#pragma once


void IRAM_ATTR addToSendQueue(uint8_t item);
void IRAM_ATTR addToSendQueue(uint8_t * buff, uint8_t length);

char packetBuffer[1500];
char UDPin[1500];

uint8_t UDPoutQue[1500];
int UDPoutQuePtr = 0; //Keep track of where we are in the Que

bool MirrorToSerial = true;
void IRAM_ATTR SendUDPpacket();
