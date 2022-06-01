// Import required libraries
#include "ESP8266WiFi.h"
#include "ESPAsyncWebServer.h"
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>


const char* ssid = "MERCUSYS_1C33";
const char* password = "bv06baii";

//const char* ssid = "DIGI_3fe7d8_Ext";
//const char* password = "a7eec018";

//const char* ssid = "Redmi";
//const char* password = "iotproject";

const char* http_username = "admin";
const char* http_password = "iotproject";

// Set your Static IP address
IPAddress local_IP(192, 168, 1, 184);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8); // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional

const char* PARAM_INPUT = "state";

String obstacleFound = "false";

AsyncWebServer server(80);

bool keepLedOn = false;

const byte numChars = 3;
char receivedChars[numChars];   // an array to store the received data

boolean newData = false;

bool isBusy = false;

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.println("Serial begin");

  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    request->send(SPIFFS, "/controls.html", "text/html");
  });

  // Route to load style.css file
  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/styles.css", "text/css");
  });

  // Route to load controls.js file
  server.on("/controls.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/controls.js", "text/js");
  });

  server.on("/obstacleCheck", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    request->send(200, "text/plain", obstacleFound);
  });

  server.on("/inputPressed", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    String buttonId = request->getParam("button")->value();
    if (isBusy) {
      request->send(200, "text/plain", "OK");
    }
    digitalWrite(D0, HIGH);
    char charBuf[3];
    sprintf(charBuf, "%s1\n", buttonId);
    Serial.print(charBuf);

    request->send(200, "text/plain", "OK");
  });

  server.on("/inputReleased", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    String buttonId = request->getParam("button")->value();
    if (isBusy) {
      request->send(200, "text/plain", "OK");
    }
    digitalWrite(D0, LOW);
    char charBuf[3];
    sprintf(charBuf, "%s0\n", buttonId);
    Serial.print(charBuf);

    request->send(200, "text/plain", "OK");
  });

  server.on("/input", HTTP_GET, [] (AsyncWebServerRequest * request) {
    if (!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    isBusy = true;
    String preset = request->getParam("preset")->value();
    Serial.println(preset);
    isBusy = false;
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
