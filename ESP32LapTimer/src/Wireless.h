#ifndef __WIRELESS_H__
#define __WIRELESS_H__

void InitWifiAP();
void handleDNSRequests();
void dnsHandleRequests();
void airplaneModeOn();
void airplaneModeOff();
void toggleAirplaneMode();
bool isAirplaneModeOn();
bool InitWifiClient();

#endif // __WIRELESS_H__
