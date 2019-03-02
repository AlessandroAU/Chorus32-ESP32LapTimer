#pragma once

#include <DNSServer.h>

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;

//flag to use from web update to reboot the ESP
bool shouldReboot = false;
const char NOSPIFFS[] PROGMEM = "You have not uploaded the SPIFFs filesystem, Please install the <b><a href=\"https://github.com/me-no-dev/arduino-esp32fs-plugin\">following plugin</a></b> then select <b>Tools > ESP32 Sketch Data Upload</b>.<br>At the moment only <b>v0.1</b> of the tool appears to work so install that one. of the  The web interface will not work until you do this.";
