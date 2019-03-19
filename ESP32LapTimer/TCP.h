#pragma once

WiFiServer wifiServer(9000);
void HandleServerTCP();
void SendTCPpacket();
void ProcessTCPCommand();
void TCPinit();
void CheckForConnections();

#define TCPbuffSize 1500

uint8_t TCPoutQue[TCPbuffSize];
char TCPBuffIn[100];
int TCPoutQuePtr = 0; //Keep track of where we are in the Que
