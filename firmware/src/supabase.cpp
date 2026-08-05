#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include "config.h"
#include "supabase.h"

bool supabasePostReading(float temp, float hum) {
  if (isnan(temp) || isnan(hum)) return false;

  WiFiClientSecure client;
  client.setInsecure(); // no cert pinning: Supabase's LE/DigiCert chain rotates

  HTTPClient http;
  String url = String(SUPABASE_URL) + "/rest/v1/" + SUPABASE_TABLE;

  if (!http.begin(client, url)) return false;

  http.addHeader("Content-Type", "application/json");
  http.addHeader("apikey", SUPABASE_SERVICE_KEY);
  http.addHeader("Authorization", String("Bearer ") + SUPABASE_SERVICE_KEY);
  http.addHeader("Prefer", "return=minimal");

  char payload[64];
  snprintf(payload, sizeof(payload), "{\"temp\":%.1f,\"hum\":%.1f}", temp, hum);

  int code = http.POST(payload);
  http.end();

  return code >= 200 && code < 300;
}
