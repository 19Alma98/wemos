#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "config.h"
#include "mqtt.h"

static WiFiClient espClient;
static PubSubClient client(espClient);

static unsigned long lastReconnectAttempt = 0;

static bool mqttReconnect() {
  if (client.connect(MQTT_CLIENT_ID)) {
    Serial.println("MQTT connesso!");
  } else {
    Serial.print("MQTT connessione fallita, rc=");
    Serial.println(client.state());
  }
  return client.connected();
}

void mqttBegin() {
  client.setServer(MQTT_BROKER, MQTT_PORT);
}

void mqttLoop() {
  if (!client.connected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      mqttReconnect();
    }
  } else {
    client.loop();
  }
}

void mqttPublish(float temp, float hum) {
  if (!client.connected()) return;
  if (isnan(temp) || isnan(hum)) return;

  char payload[64];
  snprintf(payload, sizeof(payload), "{\"temp\":%.1f,\"hum\":%.1f}", temp, hum);
  client.publish(MQTT_TOPIC, payload);
}
