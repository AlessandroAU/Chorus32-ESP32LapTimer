#include "TimerWebServer.h"

#include "settings_eeprom.h"
#include "ADC.h"
#include "RX5808.h"
#include "Calibration.h"

#include <esp_wifi.h>
#include <FS.h>
#include "TimerWebServer.h"
#include <WebServer.h>
#include "SPIFFS.h"
#include <Update.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer webServer(80);

//flag to use from web update to reboot the ESP
//static bool shouldReboot = false;
static const char NOSPIFFS[] PROGMEM = "You have not uploaded the SPIFFs filesystem!!!, Please install the <b><a href=\"https://github.com/me-no-dev/arduino-esp32fs-plugin\">following plugin</a></b>.<br> Place the plugin file here: <b>\"<path to your Arduino dir>/tools/ESP32FS/tool/esp32fs.jar\"</b>.<br><br> Next select <b>Tools > ESP32 Sketch Data Upload</b>.<br>NOTE: This is a seperate upload to the normal arduino upload!!!<br><br> The web interface will not work until you do this.";

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
  setRXBand(rx, band);
  EepromSettings.RXChannel[rx] = channel;
  setRXChannel(rx, channel);
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

  if (getNumReceivers() >= 1) {
    String Band2 = req->arg("band2");
    String Channel2 = req->arg("channel2");
    int band2 = (byte)Band2.toInt();
    int channel2 = (byte)Channel2.toInt();
    updateRx(band2, channel2, 2);
  }
  if (getNumReceivers() >= 2) {
    String Band3 = req->arg("band3");
    String Channel3 = req->arg("channel3");
    int band3 = (byte)Band3.toInt();
    int channel3 = (byte)Channel3.toInt();
    updateRx(band3, channel3, 3);
  }
  if (getNumReceivers() >= 3) {
    String Band4 = req->arg("band4");
    String Channel4 = req->arg("channel4");
    int band4 = (byte)Band4.toInt();
    int channel4 = (byte)Channel4.toInt();
    updateRx(band4, channel4, 4);
  }
  if (getNumReceivers() >= 4) {
    String Band5 = req->arg("band5");
    String Channel5 = req->arg("channel5");
    int band5 = (byte)Band5.toInt();
    int channel5 = (byte)Channel5.toInt();
    updateRx(band5, channel5, 5);
  }

  if (getNumReceivers() >= 5) {
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

void InitWebServer() {
  HasSPIFFsBegun = SPIFFS.begin();
  //delay(1000);

  if (!SPIFFS.exists("/index.html")) {
    Serial.println("SPIFFS filesystem was not found");
    webServer.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
      req->send(200, "text/html", NOSPIFFS);
    });

    webServer.onNotFound([](AsyncWebServerRequest* req) {
      req->send(404, "text/plain", "404: Not Found");
    });

    webServer.begin();                           // Actually start the server
    Serial.println("HTTP server started");
    //client.setNoDelay(true);
    return;
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

  webServer.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
    req->send(SPIFFS, "/index.html");
  });

  webServer.on("/update", HTTP_POST, [](AsyncWebServerRequest* req) {
    AsyncWebServerResponse *response = req->beginResponse((Update.hasError()) ? 400 : 200, "text/plain", (Update.hasError()) ? "FAIL" : "OK, module rebooting");
    response->addHeader("Connection", "close");
    req->send(response);
    Serial.println("off-updating");
    ESP.restart();
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
