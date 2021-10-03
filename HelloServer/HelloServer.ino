#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>

#define DDNSadr "http://192.168.1.10:8000/update"

#ifndef STASSID
#define STASSID "ap"
#define STAPSK  "pass"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

const int led = 13;

#define ZOOMER 5
#define HERC 4

#include "fn.h"

int counter = 0;

ICACHE_RAM_ATTR void detectsMovement() {
  counter++;
  tone(ZOOMER,(int)random(500,10000),(unsigned long )1000);
}


void setup(void) {
  pinMode(led, OUTPUT);
  pinMode(ZOOMER, OUTPUT);
  pinMode(HERC, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(HERC), detectsMovement, RISING);

  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("HTTP server started");

  Serial.println();
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", []() {
    server.send(200, "text/plain", String(counter));
  });

  server.onNotFound(handleNotFound);
  server.begin();
}

void loop(void) {
  server.handleClient();
  MDNS.update();
  updateDDNS();

}
