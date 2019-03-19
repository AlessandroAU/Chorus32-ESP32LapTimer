#pragma once


#define UDPbuffSize 1500

char packetBuffer[UDPbuffSize];
char UDPin[UDPbuffSize];

uint8_t UDPoutQue[UDPbuffSize];
int UDPoutQuePtr = 0; //Keep track of where we are in the Que

bool MirrorToSerial = true;
void IRAM_ATTR SendUDPpacket();
