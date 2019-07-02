#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include "Secrets.h"

/* Secrets.h file should contain data as below: */
#ifndef WIFI_SSID
#define WIFI_SSID "xxxxxxxxxx"
#define WIFI_PASSWORD "xxxxxxxxxx"
#endif

#define BUZZER_PIN 0
#define LED_PIN 1
#define OTA_HOSTNAME "Doorbell-Receiver"
#define SERVER_PORT 80

ESP8266WebServer server(SERVER_PORT);


void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  Serial.begin(115200);
  setupWifi();
  server.on("/", []() {
    server.send(200, "text/html", "\
      <a href=\"/ring\">/ring</a><br>\
      <a href=\"/reboot\">/reboot</a><br>\
    ");
  });
  server.on("/ring", ring);
  server.on("/reboot", []() {
    server.send(200, "text/plain", "Rebooting doorbell server");
    delay(1000);
    ESP.restart();
  });
  server.begin();
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.begin();
}


void loop() {
  server.handleClient();
  ArduinoOTA.handle();
}


void ring() {
  server.send(200, "text/plain", "Ringing");
  Serial.println("Bell rang");
  digitalWrite(BUZZER_PIN, HIGH);
  delay(2000);
  digitalWrite(BUZZER_PIN, LOW);
}


void setupWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(LED_PIN, HIGH);
    delay(50);
    digitalWrite(LED_PIN, LOW);
    delay(50);
    digitalWrite(LED_PIN, HIGH);
    delay(50);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(700);
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);
}
