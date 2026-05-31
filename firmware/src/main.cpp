#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "wifi.h"
#include "sensor.h"
#include "web.h"
#include "led.h"

void setup() {
  Serial.begin(115200);
  Serial.println("\nBOOT OK");
  delay(2000);

  ledBegin();
  sensorBegin();

  Serial.println("\nAvvio WiFi...");

  wifiBegin();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connesso!");
  Serial.print("IP assegnato: ");
  Serial.println(WiFi.localIP());

  webBegin();

  Serial.println("Web avviato");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnesso!");
    delay(2000);
    return;
  }

  float t = getTemp();
  float h = getHum();

  webLoop();

  delay(2000);
}