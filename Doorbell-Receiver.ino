#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <time.h>
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
const int TIMEZONE = 5;
const int DST = 0;
String logTime;
String logMsg;

ESP8266WebServer server(SERVER_PORT);


void setup() {
  log("I/system: startup");
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  Serial.begin(115200);
  setupWifi();
  setupTime();
  server.on("/", []() {
    server.send(200, "text/html", "\
      <a href=\"/log\">/log</a><br>\
      <a href=\"/ring\">/ring</a><br>\
      <a href=\"/reboot\">/reboot</a><br>\
    ");
    log("I/server: served / to " + server.client().remoteIP().toString());
  });
  server.on("/log", []() {
    server.send(200, "text/plain", logMsg);
    log("I/server: served /log to " + server.client().remoteIP().toString());
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


void log(String msg) {
  time_t now = time(0);
  logTime = ctime(&now);
  logTime.trim();
  logMsg = logMsg + "[" + logTime + "] ";
  logMsg = logMsg + msg + "\n";
}


void ring() {
  digitalWrite(BUZZER_PIN, HIGH);
  server.send(200, "text/plain", "Ringing");
  log("I/ringer: ringing bell on request from " + server.client().remoteIP().toString());
  Serial.println("Bell rang");
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
  log("I/system: Wifi connected.");
  delay(700);
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);
}


void setupTime() {
  configTime(TIMEZONE * 3600, DST, "pool.ntp.org", "time.nist.gov");
  Serial.println("Waiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  delay(100);
  time_t now = time(0);
  Serial.println(ctime(&now));
}
