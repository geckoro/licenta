// Import required libraries
#include "ESP8266WiFi.h"
#include "ESPAsyncWebServer.h"
#include "HTML_PAGE.h"

//const char* ssid = "DIGI_3fe7d8";
//const char* password = "a7eec018";

#define WORK_FROM_LAPTOP 1

#ifdef WORK_FROM_LAPTOP
const char* ssid = "MERCUSYS_1C33";
const char* password = "bv06baii";
#else
const char* ssid = "Redmi";
const char* password = "iotproject";
#endif

const char* http_username = "admin";
const char* http_password = "iotproject";

const char* PARAM_INPUT = "state";

String obstacleFound = "false";

AsyncWebServer server(80);

const char* index_html PROGMEM = RAW_HTML;
bool keepLedOn = false;

const byte numChars = 3;
char receivedChars[numChars];   // an array to store the received data

boolean newData = false;

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.println("Serial begin");
  
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
    String buttonId = request->getParam("button")->value();
    digitalWrite(D0, HIGH);
    char charBuf[3];
    sprintf(charBuf, "%s1\n", buttonId);
    Serial.print(charBuf);
    request->send(200, "text/plain", "OK");
  });

  server.on("/inputReleased", HTTP_GET, [](AsyncWebServerRequest *request){
    if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    String buttonId = request->getParam("button")->value();
    digitalWrite(D0, LOW);
    char charBuf[3];
    sprintf(charBuf, "%s0\n", buttonId);
    Serial.print(charBuf);
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
  recvWithEndMarker();
  processNewData();
}

void recvWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();
    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    }
    else {
      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
    }
  }
}

void processNewData() {
  if (newData == true) {
    if (receivedChars[numChars - 2] == '1') {
      obstacleFound = "true";
    } else if (receivedChars[numChars - 2] == '0') {
      obstacleFound = "false";
    }

    newData = false;
  }
}
