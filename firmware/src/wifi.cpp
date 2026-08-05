#include <ESP8266WiFi.h>
#include "config.h"
#include "led.h"

void wifiBegin(){
  ledOff();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}
