#include "HTTPserver.h"

const char* host = "esp32-webupdate";
//const char* ssid = "........";
//const char* password = "........";

const char* serverIndex = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";

void InitHTTPserver(void) {

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("Chorus LapTimer");

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);

  //delay(2000);

  MDNS.begin(host);
  HTTPserver.on("/", HTTP_GET, []() {
    HTTPserver.sendHeader("Connection", "close");
    HTTPserver.send(200, "text/html", serverIndex);
  });
  HTTPserver.on("/update", HTTP_POST, []() {
    StopADCtimer();
    HTTPserver.sendHeader("Connection", "close");
    HTTPserver.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    StopADCtimer();
    HTTPUpload& upload = HTTPserver.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.setDebugOutput(true);
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin()) { //start with max available size
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
  });
  HTTPserver.begin();
  MDNS.addService("http", "tcp", 80);

  Serial.printf("Ready! Open http://%s.local in your browser\n", host);

}
