// Import required libraries
#include "ESP8266WiFi.h"
#include "ESPAsyncWebServer.h"
#include <SoftwareSerial.h>
#include "HTML_PAGE.h"
#define WORK_FROM_FAGARAS 1

#ifdef WORK_FROM_FAGARAS
const char* ssid = "DIGI_3fe7d8";
const char* password = "a7eec018";
#else
const char* ssid = "MERCUSYS_1C33";
const char* password = "bv06baii";
#endif

const char* http_username = "admin";
const char* http_password = "iotproject";

const char* PARAM_INPUT = "state";

String obstacleFound = "false";

AsyncWebServer server(80);

const char* index_html PROGMEM = RAW_HTML;
bool keepLedOn = false;

void setup(){  
  pinMode(D0, OUTPUT);
  
  // Serial port for debugging purposes
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    request->send(200, "text/html", index_html);
  });
  
  server.on("/obstacleCheck", HTTP_GET, [](AsyncWebServerRequest *request){
    if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    request->send(200, "text/plain", obstacleFound);
  });

  server.on("/inputPressed", HTTP_GET, [](AsyncWebServerRequest *request){
    if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    digitalWrite(D0, HIGH);
    request->send(200, "text/plain", "OK");
  });

  server.on("/inputReleased", HTTP_GET, [](AsyncWebServerRequest *request){
    if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    digitalWrite(D0, LOW);
    request->send(200, "text/plain", "OK");
  });

  server.on("/input", HTTP_GET, [] (AsyncWebServerRequest *request) {
    if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    String buttonId = request->getParam("button")->value();
    
    Serial.println(buttonId);
    request->send(200, "text/plain", "OK");
  });
  // Start server
  server.begin();
}

void loop() {
  if (Serial.available()) {
    int received = Serial.parseInt();
    if (received == 3) {
      obstacleFound = "false";
    }
    else if (received == 4) {
      obstacleFound = "true";
    }
  }
}
