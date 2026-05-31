#include <Arduino.h>

#define LED_PIN LED_BUILTIN

void ledBegin(){
  pinMode(LED_PIN, OUTPUT);
}

void ledOn(){
  digitalWrite(LED_PIN, LOW);
}

void ledOff(){
  digitalWrite(LED_PIN, HIGH);
}
