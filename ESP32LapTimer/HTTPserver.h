#ifndef HTTPserver_H
#define HTTPserver_H

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <DNSServer.h>

bool HTTPupdating = false;

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
WebServer HTTPserver(80);

#endif
