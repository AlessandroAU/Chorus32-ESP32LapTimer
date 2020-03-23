#include <WiFi.h>
#include <Arduino.h>
#include <ESPmDNS.h>
#include <esp_wifi.h>
#include <DNSServer.h>

#include "TimerWebServer.h"
#include "settings_eeprom.h"
#include "targets/target.h"

static const uint16_t DNS_PORT = 53;
static IPAddress apIP(192, 168, 4, 1);
static DNSServer dnsServer;
static bool airplaneMode = false;
static uint32_t delayTime = 500; // milliseconds
static uint32_t maxConnectionTime = 60000; // 2 minutes

void InitWifiAP() {
  WiFi.begin();
  delay( 500 ); // If not used, somethimes following command fails
  WiFi.mode( WIFI_AP );
  uint8_t protocol = getWiFiProtocol() ? (WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N) : (WIFI_PROTOCOL_11B);
  ESP_ERROR_CHECK(esp_wifi_set_protocol(WIFI_IF_AP, protocol));
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  uint8_t channel = getWiFiChannel();
  if(channel < 1 || channel > 13) {
    channel = 1;
  }
  log_i("Starting wifi %s on channel %i in mode %s", WIFI_AP_NAME, channel, protocol ? "bgn" : "b");
  WiFi.softAP(WIFI_AP_NAME, NULL, channel);
  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);
}

bool InitWifiClient() {
  uint32_t timeWaited = 0;

  WiFi.mode(WIFI_MODE_STA);

  log_i("Connecting to: %s", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while(WiFi.status() != WL_CONNECTED) {
    if(timeWaited >= maxConnectionTime) {
      log_e("Connection timeout");
      return false;
    }

    delay(delayTime);
    timeWaited = timeWaited + delayTime;
  }

  log_i("WiFi connected");
  log_i("IP address: %s", WiFi.localIP().toString().c_str());

  if(!MDNS.begin("chorus32")) {
    log_e("Error settings up mDNS responder");
    while(1) {
      delay(1000);
    }
  }

  log_i("mDNS responder started");

  return true;
}

void handleDNSRequests() {
  dnsServer.processNextRequest();
}

void airplaneModeOn() {
  // Enable Airplane Mode (WiFi Off)
  log_i("Airplane Mode On");
  WiFi.mode(WIFI_OFF);
  airplaneMode = true;
}

void airplaneModeOff() {
  // Disable Airplane Mode (WiFi On)
  log_i("Airplane Mode OFF");
  InitWifiAP();
  InitWebServer();
  airplaneMode = false;
}

// Toggle Airplane mode on and off based on current state
void toggleAirplaneMode() {
  if (!airplaneMode) {
    airplaneModeOn();
  } else {
    airplaneModeOff();
  }
}

bool isAirplaneModeOn() {
  return airplaneMode;
}
