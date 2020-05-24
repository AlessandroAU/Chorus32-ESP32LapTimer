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
#include "TimerWebServer.h"

#include "settings_eeprom.h"
#include "ADC.h"
#include "RX5808.h"
#include "Calibration.h"
#include "Output.h"
#include "CrashDetection.h"

#include <esp_wifi.h>
#include <FS.h>
#include "TimerWebServer.h"
#include <WebServer.h>
#include "SPIFFS.h"
#include <Update.h>
#include <ESPAsyncWebServer.h>

#define WEBSOCKET_BUF_SIZE 1500
static uint8_t websocket_buffer[WEBSOCKET_BUF_SIZE];
static int websocket_buffer_pos = 0;
SemaphoreHandle_t websocket_lock;

AsyncWebServer webServer(80);
AsyncWebSocket ws("/ws");

//flag to use from web update to reboot the ESP
//static bool shouldReboot = false;
static const char NOSPIFFS[] PROGMEM = "<h1>ERROR: Could not read the SPIFFS partition</h1>This means you either forgot to upload the data files or a previous update failed.<br>To fix this problem you have a few options:<h4>1 Arduino IDE</h4> Install the <b><a href=\"https://github.com/me-no-dev/arduino-esp32fs-plugin\">following plugin</a></b>.<br> Place the plugin file here: <b>\"&lt;path to your Arduino dir&gt;/tools/ESP32FS/tool/esp32fs.jar\"</b>.<br><br> Next select <b>Tools > ESP32 Sketch Data Upload</b>.<br>NOTE: This is a seperate upload to the normal arduino upload!<h4>2 Platformio</h4>Press the Upload Filesystem button or run \"pio run -e &lt;your board&gt; -t uploadfs\"<h4>3 Use this form</h4>Upload the spiffs file from the release site here:<br><br><form method='POST' action='/update' enctype='multipart/form-data'> <input type='file' name='update'> <input type='submit' value='Update'></form>";

static bool HasSPIFFsBegun = false;
static bool isHTTPUpdating = false;


String getMacAddress() {
  byte mac[6];
  WiFi.macAddress(mac);
  String cMac = "";
  for (int i = 0; i < 6; ++i) {
    cMac += String(mac[i], HEX);
    if (i < 5)
      cMac += "-";
  }
  cMac.toUpperCase();
  Serial.print("Mac Addr:");
  Serial.println(cMac);
  return cMac;
}

String getContentType(String filename) { // convert the file extension to the MIME type
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".svg")) return "image/svg+xml";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(AsyncWebServerRequest* req, String path) { // send the right file to the client (if it exists)
  // If a folder is requested, send the index file
  String contentType = getContentType(path);            // Get the MIME type
  if (SPIFFS.exists(path)) {
    req->send(SPIFFS, path, contentType);
    return true;
  }
  return false;                                         // If the file doesn't exist, return false
}

void updateRx (int band, int channel, int rx) {
  rx = rx - 1;
  setModuleChannelBand(band, channel, rx);
  EepromSettings.RXBand[rx] = band;
  EepromSettings.RXChannel[rx] = channel;
  uint16_t index = getRXChannel(rx) + (8 * getRXBand(rx));
  EepromSettings.RXfrequencies[rx] = channelFreqTable[index];
}

void SendStatusVars(AsyncWebServerRequest* req) {
  req->send(200, "application/json", "{\"Var_VBAT\": " + String(getVbatFloat(), 2) + ", \"Var_WifiClients\": 1, \"Var_CurrMode\": \"IDLE\"}");
}

void SendStaticVars(AsyncWebServerRequest* req) {
  String sendSTR = "{\"displayTimeout\": " + String(getDisplayTimeout()) + ", \"NumRXs\": " + String(getNumReceivers() - 1) + ", \"ADCVBATmode\": " + String(getADCVBATmode()) + ", \"RXFilter\": " + String(getRXADCfilter()) + ", \"ADCcalibValue\": " + String(getVBATcalibration(), 3) + ", \"RSSIthreshold\": " + String(getRSSIThreshold(0)) + ", \"WiFiChannel\": " + String(getWiFiChannel()) + ", \"WiFiProtocol\": " + String(getWiFiProtocol());;
  sendSTR = sendSTR + ",\"Band\":{";
  for (int i = 0; i < getNumReceivers(); i++) {
    sendSTR = sendSTR + "\"" + i + "\":" + EepromSettings.RXBand[i];
    if (getNumReceivers() > 1 && getNumReceivers() - i > 1) {
      sendSTR = sendSTR + ",";
    }
  }
  sendSTR = sendSTR + "},";
  sendSTR = sendSTR + "\"Channel\":{";
  for (int i = 0; i < getNumReceivers(); i++) {
    sendSTR = sendSTR + "\"" + i + "\":" + EepromSettings.RXChannel[i];
    if (getNumReceivers() > 1 && getNumReceivers() - i > 1) {
      sendSTR = sendSTR + ",";
    }
  }
  sendSTR = sendSTR + "}";
  sendSTR = sendSTR +  "}";

  req->send(200, "application/json", sendSTR);
}

void ProcessGeneralSettingsUpdate(AsyncWebServerRequest* req) {
  String NumRXs = req->arg("NumRXs");
  EepromSettings.NumReceivers = (byte)NumRXs.toInt();

  // getNumReceivers() is always >= 0
  // TODO: why does getNumReceivers() == 0 equals to 1 rx?
  String Band1 = req->arg("band1");
  String Channel1 = req->arg("channel1");
  int band1 = (byte)Band1.toInt();
  int channel1 = (byte)Channel1.toInt();
  updateRx(band1, channel1, 1);

  if (getNumReceivers() > 1) {
    String Band2 = req->arg("band2");
    String Channel2 = req->arg("channel2");
    int band2 = (byte)Band2.toInt();
    int channel2 = (byte)Channel2.toInt();
    updateRx(band2, channel2, 2);
  }
  if (getNumReceivers() > 2) {
    String Band3 = req->arg("band3");
    String Channel3 = req->arg("channel3");
    int band3 = (byte)Band3.toInt();
    int channel3 = (byte)Channel3.toInt();
    updateRx(band3, channel3, 3);
  }
  if (getNumReceivers() > 3) {
    String Band4 = req->arg("band4");
    String Channel4 = req->arg("channel4");
    int band4 = (byte)Band4.toInt();
    int channel4 = (byte)Channel4.toInt();
    updateRx(band4, channel4, 4);
  }
  if (getNumReceivers() > 4) {
    String Band5 = req->arg("band5");
    String Channel5 = req->arg("channel5");
    int band5 = (byte)Band5.toInt();
    int channel5 = (byte)Channel5.toInt();
    updateRx(band5, channel5, 5);
  }

  if (getNumReceivers() > 5) {
    String Band6 = req->arg("band6");
    String Channel6 = req->arg("channel6");
    int band6 = (byte)Band6.toInt();
    int channel6 = (byte)Channel6.toInt();
    updateRx(band6, channel6, 6);
  }

  String Rssi = req->arg("RSSIthreshold");
  int rssi = (byte)Rssi.toInt();
  int value = rssi * 12;
  for (int i = 0 ; i < MAX_NUM_RECEIVERS; i++) {
    EepromSettings.RSSIthresholds[i] = value;
    setRSSIThreshold(i, value);
  }

  req->redirect("/redirect.html");
  setSaveRequired();
}

void calibrateRSSI(AsyncWebServerRequest* req) {
  rssiCalibration();
  req->redirect("/redirect.html");
}

void eepromReset(AsyncWebServerRequest* req){
  EepromSettings.defaults();
  req->redirect("/redirect.html");
}

void ProcessVBATModeUpdate(AsyncWebServerRequest* req) {
  String inADCVBATmode = req->arg("ADCVBATmode");
  String inADCcalibValue = req->arg("ADCcalibValue");

  setADCVBATmode((ADCVBATmode_)(byte)inADCVBATmode.toInt());
  setVBATcalibration(inADCcalibValue.toFloat());

  EepromSettings.ADCVBATmode = getADCVBATmode();
  EepromSettings.VBATcalibration = getVBATcalibration();
  setSaveRequired();

  req->redirect("/redirect.html");
  setSaveRequired();
}

void ProcessADCRXFilterUpdate(AsyncWebServerRequest* req) {
  String inRXFilter = req->arg("RXFilter");
  setRXADCfilter((RXADCfilter_)(byte)inRXFilter.toInt());
  EepromSettings.RXADCfilter = getRXADCfilter();

  req->redirect("/redirect.html");
  setSaveRequired();

}

void ProcessWifiSettings(AsyncWebServerRequest* req) {
  String inWiFiChannel = req->arg("WiFiChannel");
  String inWiFiProtocol = req->arg("WiFiProtocol");

  EepromSettings.WiFiProtocol = inWiFiProtocol.toInt();
  EepromSettings.WiFiChannel = inWiFiChannel.toInt();

  req->redirect("/redirect.html");
  setSaveRequired();
  airplaneModeOn();
  airplaneModeOff();
}

void ProcessDisplaySettingsUpdate(AsyncWebServerRequest* req) {
  EepromSettings.display_timeout_ms = req->arg("displayTimeout").toInt() * 1000;
  req->redirect("/redirect.html");
  setSaveRequired();
}

void onWebsocketEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(isHTTPUpdating) return; // ignore all incoming messages during update
  if(xSemaphoreTake(websocket_lock, portMAX_DELAY)){
    //Handle WebSocket event
    if(type == WS_EVT_DATA){
      //data packet
      AwsFrameInfo * info = (AwsFrameInfo*)arg;
      if(info->final && info->index == 0 && info->len == len){
        //the whole message is in a single frame and we got all of it's data
        // we'll ignore fragmented messages for now
        if(websocket_buffer_pos + len < WEBSOCKET_BUF_SIZE) {
          memcpy(websocket_buffer + websocket_buffer_pos, data, len);
          websocket_buffer_pos += len;
        }
      }
    }
    xSemaphoreGive(websocket_lock);
  }
}

void read_websocket(void* output) {
  if(xSemaphoreTake(websocket_lock, 1)){
    if(websocket_buffer_pos > 0) {
      output_t* out = (output_t*)output;
      out->handle_input_callback(websocket_buffer, websocket_buffer_pos);
      websocket_buffer_pos = 0;
    }
    xSemaphoreGive(websocket_lock);
  }
}

void send_websocket(void* output, uint8_t* data, size_t len) {
  ws.textAll(data, len);
}

void InitWebServer() {
  HasSPIFFsBegun = SPIFFS.begin();
  // attach AsyncWebSocket
  ws.onEvent(onWebsocketEvent);
  webServer.addHandler(&ws);
  websocket_lock = xSemaphoreCreateMutex();

  webServer.on("/recovery.html", HTTP_GET, [](AsyncWebServerRequest* req) {
      req->send(200, "text/html", NOSPIFFS);
    });

  if (!SPIFFS.exists("/index.html")) {
    Serial.println("SPIFFS filesystem was not found");
    webServer.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
      req->redirect("/recovery.html");
    });
  } else {
    webServer.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
      req->send(SPIFFS, "/index.html");
    });
  }


  webServer.onNotFound([](AsyncWebServerRequest* req) {

    if (
      (req->url() == "/generate_204") ||
      (req->url() == "/gen_204") ||
      (req->url() == "/library/test/success.html") ||
      (req->url() == "/hotspot-detect.html") ||
      (req->url() == "/connectivity-check.html")  ||
      (req->url() == "/check_network_status.txt")  ||
      (req->url() == "/ncsi.txt")
    ) {
      req->redirect("/");
    } else {
      // If the client requests any URI
      if (!handleFileRead(req, req->url()))                  // send it if it exists
        req->send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
    }
  });

  webServer.on("/StatusVars", SendStatusVars);
  webServer.on("/StaticVars", SendStaticVars);

  webServer.on("/updateGeneral", ProcessGeneralSettingsUpdate);
  webServer.on("/updateFilters", ProcessADCRXFilterUpdate);
  webServer.on("/ADCVBATsettings", ProcessVBATModeUpdate);
  webServer.on("/displaySettings", ProcessDisplaySettingsUpdate);
  webServer.on("/calibrateRSSI",calibrateRSSI);
  webServer.on("/eepromReset",eepromReset);

  webServer.on("/WiFisettings", ProcessWifiSettings);


  webServer.on("/update", HTTP_POST, [](AsyncWebServerRequest* req) {
    AsyncWebServerResponse *response = req->beginResponse((Update.hasError()) ? 400 : 200, "text/plain", (Update.hasError()) ? "FAIL" : "OK, module rebooting");
    response->addHeader("Connection", "close");
    req->send(response);
    Serial.println("off-updating");
    restart_esp();
  }, [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    isHTTPUpdating = true;
    if(!index) {
      int partition = data[0] == 0xE9 ? U_FLASH : U_SPIFFS;
      if(partition == U_SPIFFS) {
        // Since we don't have a magic number, we are checking the filename for "spiffs"
        if(strstr(filename.c_str(), "spiffs") == NULL) {
          partition = -1; // set partition to an invalid value
        }
      }
      log_i("Update Start: %s on partition %d\n", filename.c_str(), partition);
      if (!Update.begin(UPDATE_SIZE_UNKNOWN, partition)) { //start with max available size
        log_e("%s\n", Update.errorString());
        isHTTPUpdating = false;
      }
    }
    if(!Update.hasError()){
      if(Update.write(data, len) != len){
        log_e("%s\n", Update.errorString());
        isHTTPUpdating = false;
      }
    }
    if(final){
      if(Update.end(true)){
        Serial.printf("Update Success: %uB\n", index+len);
      } else {
        log_e("%s\n", Update.errorString());
      }
      isHTTPUpdating = false;
    }
  });

  webServer.begin();                           // Actually start the server
  Serial.println("HTTP server started");
  delay(1000);
}

bool isUpdating() {
  return isHTTPUpdating;
}
