#include <ESP8266WiFi.h>
#include "config.h"
#include "led.h"

void wifiBegin(){
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    ledOff();
  }
  ledOn();
}
