#include "WebServer.h"

bool airplaneMode = false;

// Toggle Airplane mode on and off based on current state
void toggleAirplaneMode() {
  if (!airplaneMode) {
    airplaneModeOn();
  } else {
    airplaneModeOff();
  }
}

void airplaneModeOn() {
  // Enable Airplane Mode (WiFi Off)
  Serial.println("Airplane Mode On");
  WiFi.mode(WIFI_OFF);
  airplaneMode = true;
}

void airplaneModeOff() {
  // Disable Airplane Mode (WiFi On)
  Serial.println("Airplane Mode OFF");
  InitWifiAP();
  InitWebServer();
  airplaneMode = false;
}
