#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>

extern byte DNS_PORT;
extern IPAddress apIP;
extern DNSServer dnsServer;

void InitWifiAP();
void handleDNSRequests();
void dnsHandleRequests();
void airplaneModeOn();
void airplaneModeOff();
void toggleAirplaneMode();
bool isAirplaneModeOn();
bool InitWifiClient();
