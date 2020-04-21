/*
 * This file is part of Chorus32-ESP32LapTimer 
 * (see https://github.com/AlessandroAU/Chorus32-ESP32LapTimer).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
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

static void WiFiEvent(WiFiEvent_t event) {
  switch(event) {
    case SYSTEM_EVENT_AP_START:
      log_i("Setting system hostname");
      WiFi.softAPsetHostname("chorus32");
      break;
    case SYSTEM_EVENT_STA_START:
      log_i("Setting system hostname");
      WiFi.setHostname("chorus32");
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      log_i("WiFi network disconnected, retrying...");
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    default:
      break;
  }
}

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

  MDNS.begin("chorus32");

  log_i("mDNS responder started");

  return true;
}

void InitWifi() {
  WiFi.onEvent(WiFiEvent);

  #if defined(WIFI_MODE_ACCESSPOINT)
    InitWifiAP();
  #elif defined(WIFI_MODE_CLIENT)
    if(!InitWifiClient()) {
      log_i("Failed to connect to WiFi Network");
      log_i("Starting up in AP mode instead!");
      InitWifiAP();
    }
  #endif
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
