#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "config.h"
#include "wifi.h"
#include "sensor.h"
#include "led.h"
#include "supabase.h"

void goToSleep() {
  Serial.println("Deep sleep...");
  Serial.flush();
  ESP.deepSleep((uint64_t)DEEP_SLEEP_SECONDS * 1000000ULL);
}

void setup() {
  Serial.begin(115200);
  Serial.println("\nBOOT OK");

  ledBegin();
  sensorBegin();

  Serial.println("Avvio WiFi...");

  wifiBegin();

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - start > WIFI_CONNECT_TIMEOUT_MS) {
      Serial.println("WiFi timeout, vado in sleep senza pubblicare.");
      goToSleep();
      return;
    }
    delay(200);
  }

  ledOn();
  Serial.print("WiFi connesso, IP: ");
  Serial.println(WiFi.localIP());

  float t = getTemp();
  float h = getHum();

  if (isnan(t) || isnan(h)) {
    Serial.println("Lettura sensore non valida.");
  } else {
    Serial.printf("temp=%.1f hum=%.1f\n", t, h);
    bool ok = supabasePostReading(t, h);
    Serial.println(ok ? "Pubblicato su Supabase." : "Pubblicazione fallita.");
  }

  goToSleep();
}

void loop() {
  // unreachable: setup() always ends in deep sleep, which resets the chip on wake
}
