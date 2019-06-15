
bool firstRedirect = true;
bool HasSPIFFsBegun = false;

bool HTTPupdating = false;

///////////Extern Variable we need acces too///////////////////////

extern RXADCfilter_ RXADCfilter;
extern ADCVBATmode_ ADCVBATmode;

extern byte NumRecievers;
extern float VBATcalibration;

//////////////////////////////////////////////////////////////////

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

void HandleWebServer( void * parameter ) {
  while (1) {
    webServer.handleClient();
    vTaskDelay(50);
  }

}

void HandleDNSServer( void * parameter ) {
  while (1) {
    dnsServer.processNextRequest();
    //vTaskDelay(100);
  }
}


bool handleFileRead(String path) { // send the right file to the client (if it exists)
  HTTPupdating = true;
  //Serial.println("off");
  // If a folder is requested, send the index file
  String contentType = getContentType(path);            // Get the MIME type
  if (SPIFFS.exists(path)) {
    //Serial.println(path);// If the file exists
    File file = SPIFFS.open(path, "r");                 // Open it
    size_t sent = webServer.streamFile(file, contentType); // And send it to the client
    file.close();
    HTTPupdating = false;
    //Serial.println("on");
    return true;
  }
  //Serial.print("\tFile Not Found: ");
  //Serial.println(path);
  HTTPupdating = false;
  //Serial.println("on");
  return false;                                         // If the file doesn't exist, return false
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

void InitWifiAP() {
  HTTPupdating = true;
  Serial.println("off");
  //ESP_ERROR_CHECK( esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11N) );
  //esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N | WIFI_PROTOCOL_LR)
  // WiFi.mode(WIFI_AP);




  WiFi.begin();
  delay( 500 ); // If not used, somethimes following command fails
  WiFi.mode( WIFI_AP );
  //ESP_ERROR_CHECK(esp_wifi_set_protocol(WIFI_IF_AP, WIFI_PROTOCOL_11N));
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  //WiFi.setSleep(false);
  WiFi.softAP("Chorus32 LapTimer");


  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);
  HTTPupdating = false;
  Serial.println("on");

}

bool DoesSPIFFsExist() {

}


void InitWebServer() {


  HasSPIFFsBegun = SPIFFS.begin();
  //delay(1000);

  if (!SPIFFS.exists("/index.html")) {
    Serial.println("SPIFFS filesystem was not found");
    webServer.on("/", HTTP_GET, []() {
      webServer.send(200, "text/html", NOSPIFFS);
    });

    webServer.onNotFound([]() {
      webServer.send(404, "text/plain", "404: Not Found");
    });

    webServer.begin();                           // Actually start the server
    Serial.println("HTTP server started");
    //client.setNoDelay(true);
    return;
  }


  webServer.onNotFound([]() {

    if (
      (webServer.uri() == "/generate_204") ||
      (webServer.uri() == "/gen_204") ||
      (webServer.uri() == "/library/test/success.html") ||
      (webServer.uri() == "/hotspot-detect.html") ||
      (webServer.uri() == "/connectivity-check.html")  ||
      (webServer.uri() == "/check_network_status.txt")  ||
      (webServer.uri() == "/ncsi.txt")
    ) {
      webServer.sendHeader("Location", "/", true);  //Redirect to our html web page
      if (firstRedirect) {
        webServer.sendHeader("Location", "/", true);  //Redirect to our html web page
        webServer.send(301, "text/plain", "");
      } else {
        webServer.send(404, "text/plain", "");
      }
    } else {

      // If the client requests any URI
      if (!handleFileRead(webServer.uri()))                  // send it if it exists
        webServer.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
    }
  });

  webServer.on("/StatusVars", SendStatusVars);
  webServer.on("/StaticVars", SendStaticVars);

  webServer.on("/updateGeneral", ProcessGeneralSettingsUpdate);
  webServer.on("/updateFilters", ProcessADCRXFilterUpdate);
  webServer.on("/ADCVBATsettings", ProcessVBATModeUpdate);
  webServer.on("/calibrateRSSI",calibrateRSSI);
  webServer.on("/eepromReset",eepromReset);

  webServer.on("/", HTTP_GET, []() {
    firstRedirect = false; //wait for it to hit the index page one time
    HTTPupdating = true;
    //Serial.println("off");
    webServer.sendHeader("Connection", "close");
    File file = SPIFFS.open("/index.html", "r");
    // Open it
    size_t sent = webServer.streamFile(file, "text/html"); // And send it to the client
    file.close();
    HTTPupdating = false;
    //Serial.println("on");
  });


  webServer.on("/update", HTTP_POST, []() {
    HTTPupdating = true;
    Serial.println("off-updating");
    webServer.sendHeader("Connection", "close");
    webServer.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK, module rebooting");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = webServer.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.setDebugOutput(true);
      Serial.printf("Update: %s\n", upload.filename.c_str());
      uint32_t maxSketchSpace = 0x140000;

      if (!Update.begin(maxSketchSpace)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
      Serial.setDebugOutput(false);
    }
    yield();
  });


  webServer.begin();                           // Actually start the server
  Serial.println("HTTP server started");
  client.setNoDelay(1);
  delay(1000);


  /////////////////////////////////////Not Using FreeRTOS tasks at the moment/////////////////////
  //  xTaskCreate(
  //    HandleWebServer,          /* Task function. */
  //    "HandleWebServer",        /* String with name of task. */
  //    10000,            /* Stack size in bytes. */
  //    NULL,             /* Parameter passed as input of the task */
  //    2,                /* Priority of the task. */
  //    NULL);            /* Task handle. */

  //  xTaskCreate(
  //    HandleDNSServer,          /* Task function. */
  //    "HandleDNSServer",        /* String with name of task. */
  //    4096,            /* Stack size in bytes. */
  //    NULL,             /* Parameter passed as input of the task */
  //    1,                /* Priority of the task. */
  //    NULL);            /* Task handle. */
  ////////////////////////////////////////////////////////////////////////////////////////////////


}


void SendStatusVars() {
  webServer.send(200, "application/json", "{\"Var_VBAT\": " + String(getVbatFloat(), 2) + ", \"Var_WifiClients\": 1, \"Var_CurrMode\": \"IDLE\"}");

}


void SendStaticVars() {

  String sendSTR = "{\"NumRXs\": " + String(NumRecievers - 1) + ", \"ADCVBATmode\": " + String(ADCVBATmode) + ", \"RXFilter\": " + String(RXADCfilter) + ", \"ADCcalibValue\": " + String(VBATcalibration, 3) + ", \"RSSIthreshold\": " + String(getRSSIThreshold(0));
  sendSTR = sendSTR + ",\"Band\":{";
  for (int i = 0; i < NumRecievers; i++) {
    sendSTR = sendSTR + "\"" + i + "\":" + EepromSettings.RXBand[i];
    if (NumRecievers > 1 && NumRecievers - i > 1) {
      sendSTR = sendSTR + ",";
    }
  }
  sendSTR = sendSTR + "},";
  sendSTR = sendSTR + "\"Channel\":{";
  for (int i = 0; i < NumRecievers; i++) {
    sendSTR = sendSTR + "\"" + i + "\":" + EepromSettings.RXChannel[i];
    if (NumRecievers > 1 && NumRecievers - i > 1) {
      sendSTR = sendSTR + ",";
    }
  }
  sendSTR = sendSTR + "}";
  sendSTR = sendSTR +  "}";

  webServer.send(200, "application/json", sendSTR);
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

void ProcessGeneralSettingsUpdate() {
  String NumRXs = webServer.arg("NumRXs");
  NumRecievers = (byte)NumRXs.toInt();

  if (NumRecievers >= 0) {
    String Band1 = webServer.arg("band1");
    String Channel1 = webServer.arg("channel1");
    int band1 = (byte)Band1.toInt();
    int channel1 = (byte)Channel1.toInt();
    updateRx(band1, channel1, 1);
  }
  if (NumRecievers >= 1) {
    String Band2 = webServer.arg("band2");
    String Channel2 = webServer.arg("channel2");
    int band2 = (byte)Band2.toInt();
    int channel2 = (byte)Channel2.toInt();
    updateRx(band2, channel2, 2);
  }
  if (NumRecievers >= 2) {
    String Band3 = webServer.arg("band3");
    String Channel3 = webServer.arg("channel3");
    int band3 = (byte)Band3.toInt();
    int channel3 = (byte)Channel3.toInt();
    updateRx(band3, channel3, 3);
  }
  if (NumRecievers >= 3) {
    String Band4 = webServer.arg("band4");
    String Channel4 = webServer.arg("channel4");
    int band4 = (byte)Band4.toInt();
    int channel4 = (byte)Channel4.toInt();
    updateRx(band4, channel4, 4);
  }
  if (NumRecievers >= 4) {
    String Band5 = webServer.arg("band5");
    String Channel5 = webServer.arg("channel5");
    int band5 = (byte)Band5.toInt();
    int channel5 = (byte)Channel5.toInt();
    updateRx(band5, channel5, 5);
  }

  if (NumRecievers >= 5) {
    String Band6 = webServer.arg("band6");
    String Channel6 = webServer.arg("channel6");
    int band6 = (byte)Band6.toInt();
    int channel6 = (byte)Channel6.toInt();
    updateRx(band6, channel6, 6);
  }

  EepromSettings.NumRecievers = NumRecievers;

  String Rssi = webServer.arg("RSSIthreshold");
  int rssi = (byte)Rssi.toInt();
  int value = rssi * 12;
  for (int i = 0 ; i < MaxNumRecievers; i++) {
    EepromSettings.RSSIthresholds[i] = value;
    setRSSIThreshold(i, value);
  }

  webServer.sendHeader("Connection", "close");
  File file = SPIFFS.open("/redirect.html", "r");                 // Open it
  size_t sent = webServer.streamFile(file, "text/html"); // And send it to the client
  file.close();
  setSaveRequired();
#ifdef OLED
  oledUpdate();
#endif

//  PowerDownAll();
//  SelectivePowerUp();
//  for (int i = 0; i < NumRecievers; i++) {
//    setModuleChannelBand(i);
//    delay(10);
//  }
//  //TODO, clean up above code so we don't need to set freqs twice.

}

void ProcessVBATModeUpdate() {
  String inADCVBATmode = webServer.arg("ADCVBATmode");
  String inADCcalibValue = webServer.arg("ADCcalibValue");

  ADCVBATmode = (ADCVBATmode_)(byte)inADCVBATmode.toInt();
  VBATcalibration =  inADCcalibValue.toFloat();

  EepromSettings.ADCVBATmode = ADCVBATmode;
  EepromSettings.VBATcalibration = VBATcalibration;
  setSaveRequired();

  webServer.sendHeader("Connection", "close");
  File file = SPIFFS.open("/redirect.html", "r");                 // Open it
  size_t sent = webServer.streamFile(file, "text/html"); // And send it to the client
  file.close();
  setSaveRequired();
}

void ProcessADCRXFilterUpdate() {
  String inRXFilter = webServer.arg("RXFilter");
  RXADCfilter = (RXADCfilter_)(byte)inRXFilter.toInt();

  EepromSettings.RXADCfilter = RXADCfilter;



  webServer.sendHeader("Connection", "close");
  File file = SPIFFS.open("/redirect.html", "r");                 // Open it
  size_t sent = webServer.streamFile(file, "text/html"); // And send it to the client
  file.close();
  setSaveRequired();

}

void calibrateRSSI() {
    rssiCalibration();
}
void eepromReset(){
    EepromSettings.defaults();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///Below is code for ASYNC webserver which would be much faster, but it appears that at the moment it is not stable for ESP32 platforms.

//#include <Arduino.h>
//#include "WebServer.h"
////#include <Hash.h>
////#include <ESP8266WiFi.h>
//#include <WiFi.h>
////#include <ESP8266mDNS.h>
//#include <ESPmDNS.h>
//#include <ArduinoOTA.h>
//#include <FS.h>
//#include <DNSServer.h>
//
////#include <ESPAsyncTCP.h>
//#include <AsyncTCP.h>
//
//#include <SPIFFS.h>
////#include <SPIFFSEditor.h>
////#include <AsyncWebSocket.h>
//#include <ESPAsyncWebServer.h>
//
//// SKETCH BEGIN
//AsyncWebServer server(80);
//AsyncWebSocket ws("/ws");
//AsyncEventSource events("/events");

//void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
//  if(type == WS_EVT_CONNECT){
//    Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
//    client->printf("Hello Client %u :)", client->id());
//    client->ping();
//  } else if(type == WS_EVT_DISCONNECT){
//    Serial.printf("ws[%s][%u] disconnect: %u\n", server->url(), client->id());
//  } else if(type == WS_EVT_ERROR){
//    Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
//  } else if(type == WS_EVT_PONG){
//    Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");
//  } else if(type == WS_EVT_DATA){
//    AwsFrameInfo * info = (AwsFrameInfo*)arg;
//    String msg = "";
//    if(info->final && info->index == 0 && info->len == len){
//      //the whole message is in a single frame and we got all of it's data
//      Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);
//
//      if(info->opcode == WS_TEXT){
//        for(size_t i=0; i < info->len; i++) {
//          msg += (char) data[i];
//        }
//      } else {
//        char buff[3];
//        for(size_t i=0; i < info->len; i++) {
//          sprintf(buff, "%02x ", (uint8_t) data[i]);
//          msg += buff ;
//        }
//      }
//      Serial.printf("%s\n",msg.c_str());
//
//      if(info->opcode == WS_TEXT)
//        client->text("I got your text message");
//      else
//        client->binary("I got your binary message");
//    } else {
//      //message is comprised of multiple frames or the frame is split into multiple packets
//      if(info->index == 0){
//        if(info->num == 0)
//          Serial.printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
//        Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
//      }
//
//      Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);
//
//      if(info->opcode == WS_TEXT){
//        for(size_t i=0; i < info->len; i++) {
//          msg += (char) data[i];
//        }
//      } else {
//        char buff[3];
//        for(size_t i=0; i < info->len; i++) {
//          sprintf(buff, "%02x ", (uint8_t) data[i]);
//          msg += buff ;
//        }
//      }
//      Serial.printf("%s\n",msg.c_str());
//
//      if((info->index + len) == info->len){
//        Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
//        if(info->final){
//          Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
//          if(info->message_opcode == WS_TEXT)
//            client->text("I got your text message");
//          else
//            client->binary("I got your binary message");
//        }
//      }
//    }
//  }
//}

//const char* http_username = "admin";
//const char* http_password = "admin";
//
//
//
//void InitWebServer() {
//
//  WiFi.mode(WIFI_AP);
//  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
//  WiFi.softAP("Chorus32 LapTimer");
//  WiFi.setSleep(false);
//
//  // if DNSServer is started with "*" for domain name, it will reply with
//  // provided IP to all DNS request
//  dnsServer.start(DNS_PORT, "*", apIP);
//
//
//  //  Serial.begin(115200);
//  //  Serial.setDebugOutput(true);
//  //  WiFi.Hostname(hostName);
//  //  WiFi.mode(WIFI_AP_STA);
//  //  WiFi.softAP(hostName);
//  //  WiFi.begin(ssid, password);
//  //  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
//  //    Serial.printf("STA: Failed!\n");
//  //    WiFi.disconnect(false);
//  //    delay(1000);
//  //    WiFi.begin(ssid, password);
//  //  }
//
//  //Send OTA events to the browser
//  //  ArduinoOTA.onStart([]() { events.send("Update Start", "ota"); });
//  //  ArduinoOTA.onEnd([]() { events.send("Update End", "ota"); });
//  //  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
//  //    char p[32];
//  //    sprintf(p, "Progress: %u%%\n", (progress/(total/100)));
//  //    events.send(p, "ota");
//  //  });
//  //  ArduinoOTA.onError([](ota_error_t error) {
//  //    if(error == OTA_AUTH_ERROR) events.send("Auth Failed", "ota");
//  //    else if(error == OTA_BEGIN_ERROR) events.send("Begin Failed", "ota");
//  //    else if(error == OTA_CONNECT_ERROR) events.send("Connect Failed", "ota");
//  //    else if(error == OTA_RECEIVE_ERROR) events.send("Recieve Failed", "ota");
//  //    else if(error == OTA_END_ERROR) events.send("End Failed", "ota");
//  //  });
//  //  ArduinoOTA.setHostname(hostName);
//  //  ArduinoOTA.begin();
//
//  MDNS.addService("http", "tcp", 80);
//  MDNS.begin("Chorus32");
//
//  SPIFFS.begin();
//
//  // ws.onEvent(onWsEvent);
//  server.addHandler(&ws);
//
//  //  events.onConnect([](AsyncEventSourceClient * client) {
//  //    client->send("hello!", NULL, millis(), 1000);
//  //  });
//
//  server.addHandler(&events);
//
//  //server.addHandler(new SPIFFSEditor(http_username,http_password));
//
//
//
//  //  if (!SPIFFS.exists("/index.html")) {  //we test to see of the index.html file exists if it does not it means the user has not yet uploaded the SPIFFS image to the board.
//  //    Serial.println("SPIFFS filesystem was not found");
//  //
//  //    server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
//  //      request->send(200, "text/html", NOSPIFFS);
//  //    });
//  //    server.begin();
//  //    return; // we exist without defining any more behaviour
//  //  };
//
//  server.on("/heap", HTTP_GET, [](AsyncWebServerRequest * request) {
//    request->send(200, "text/plain", String(ESP.getFreeHeap()));
//  });
//
//  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
//
//  server.onNotFound([](AsyncWebServerRequest * request) {
//
//    if (
//      (request->url() == "/generate_204") ||
//      (request->url() == "/gen_204") ||
//      (request->url() == "/library/test/success.html") ||
//      (request->url() == "/hotspot-detect.html") ||
//      (request->url() == "/connectivity-check.html")  ||
//      (request->url() == "/check_network_status.txt")  ||
//      (request->url() == "/ncsi.txt")
//    ) {
//      //webServer.sendHeader("Location", "/", true);  //Redirect to our html web page
//      if (firstRedirect) {
//        request->redirect("/index.html");
//        //webServer.sendHeader("Location", "/", true);  //Redirect to our html web page
//        //request->send(302, "text/plain", "");
//      } else {
//        request->send(204, "text/plain", "");
//      }
//
//    }
//  });
//
//
//  //  server.onNotFound([](AsyncWebServerRequest * request) {
//  //    Serial.printf("NOT_FOUND: ");
//  //    if (request->method() == HTTP_GET)
//  //      Serial.printf("GET");
//  //    else if (request->method() == HTTP_POST)
//  //      Serial.printf("POST");
//  //    else if (request->method() == HTTP_DELETE)
//  //      Serial.printf("DELETE");
//  //    else if (request->method() == HTTP_PUT)
//  //      Serial.printf("PUT");
//  //    else if (request->method() == HTTP_PATCH)
//  //      Serial.printf("PATCH");
//  //    else if (request->method() == HTTP_HEAD)
//  //      Serial.printf("HEAD");
//  //    else if (request->method() == HTTP_OPTIONS)
//  //      Serial.printf("OPTIONS");
//  //    else
//  //      Serial.printf("UNKNOWN");
//  //    Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());
//  //
//  //    if (request->contentLength()) {
//  //      Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
//  //      Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
//  //    }
//  //
//  //    int headers = request->headers();
//  //    int i;
//  //    for (i = 0; i < headers; i++) {
//  //      AsyncWebHeader* h = request->getHeader(i);
//  //      Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
//  //    }
//  //
//  //    int params = request->params();
//  //    for (i = 0; i < params; i++) {
//  //      AsyncWebParameter* p = request->getParam(i);
//  //      if (p->isFile()) {
//  //        Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
//  //      } else if (p->isPost()) {
//  //        Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
//  //      } else {
//  //        Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
//  //      }
//  //    }
//  //
//  //    request->send(404);
//  //  });
//
//  // upload a file to /upload
//
//
//  server.on("/update", HTTP_POST, [](AsyncWebServerRequest * request) {
//    shouldReboot = !Update.hasError();
//    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", shouldReboot ? "OK" : "FAIL");
//    response->addHeader("Connection", "close");
//    request->send(response);
//  }, [](AsyncWebServerRequest * request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
//    if (!index) {
//      Serial.printf("Update Start: %s\n", filename.c_str());
//      //Update.runAsync(true); //only for esp8266
//      uint32_t maxSketchSpace = 0x140000;
//      if (!Update.begin(maxSketchSpace)) {
//        Update.printError(Serial);
//      }
//    }
//    if (!Update.hasError()) {
//      if (Update.write(data, len) != len) {
//        Update.printError(Serial);
//      }
//    }
//    if (final) {
//      if (Update.end(true)) {
//        Serial.printf("Update Success: %uB\n", index + len);
//      } else {
//        Update.printError(Serial);
//      }
//    }
//  });
//
//  //  server.onFileUpload([](AsyncWebServerRequest * request, const String & filename, size_t index, uint8_t *data, size_t len, bool final) {
//  //    if (!index)
//  //      Serial.printf("UploadStart: %s\n", filename.c_str());
//  //    Serial.printf("%s", (const char*)data);
//  //    if (final)
//  //      Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index + len);
//  //  });
//  //  server.onRequestBody([](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
//  //    if (!index)
//  //      Serial.printf("BodyStart: %u\n", total);
//  //    Serial.printf("%s", (const char*)data);
//  //    if (index + len == total)
//  //      Serial.printf("BodyEnd: %u\n", total);
//  //  });
//
//  server.begin();
//}


//const char* ap_name = "Chorus32";
//const char* pass = "";
//
//esp_err_t event_handler(void* ctx, system_event_t* event)
//{
//  return ESP_OK;
//}
//
//void init_wifi(wifi_mode_t mode)
//{
//  const uint8_t protocol = WIFI_PROTOCOL_LR;
//  tcpip_adapter_init();
//  ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
//  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//  ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
//  ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
//  ESP_ERROR_CHECK( esp_wifi_set_mode(mode) );
//  wifi_event_group = xEventGroupCreate();
//
//  if (mode == WIFI_MODE_STA) {
//    ESP_ERROR_CHECK( esp_wifi_set_protocol(WIFI_IF_STA, protocol) );
//    wifi_config_t config = {
//      .sta = {
//        .ssid = ap_name,
//        .password = pass,
//        .bssid_set = false
//      }
//    };
//    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &config) );
//    ESP_ERROR_CHECK( esp_wifi_start() );
//    ESP_ERROR_CHECK( esp_wifi_connect() );
//
//    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
//                        false, true, portMAX_DELAY);
//    ESP_LOGI(TAG, "Connected to AP");
//  } else {
//    ESP_ERROR_CHECK( esp_wifi_set_protocol(WIFI_IF_AP, protocol) );
//    wifi_config_t config = {
//      .ap = {
//        .ssid = ap_name,
//        .password = pass,
//        .ssid_len = 0,
//        .authmode = WIFI_AUTH_WPA_WPA2_PSK,
//        .ssid_hidden = false,
//        .max_connection = 3,
//        .beacon_interval = 100,
//      }
//    };
//    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_AP, &config) );
//    ESP_ERROR_CHECK( esp_wifi_start() );
//  }
//}
