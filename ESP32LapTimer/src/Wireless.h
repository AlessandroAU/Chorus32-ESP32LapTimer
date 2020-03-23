#ifndef __WIRELESS_H__
#define __WIRELESS_H__

#include <WiFi.h>

void InitWifiAP();
void handleDNSRequests();
void dnsHandleRequests();
void airplaneModeOn();
void airplaneModeOff();
void toggleAirplaneMode();
bool isAirplaneModeOn();
bool InitWifiClient();
void InitWifi();
void WiFiEvent(WiFiEvent_t event);

#endif // __WIRELESS_H__
