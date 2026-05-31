#include <DHT.h>
#include "config.h"

DHT dht(DHT_PIN, DHT_TYPE);

void sensorBegin(){
  dht.begin();
}

float getTemp(){
  return dht.readTemperature();
}

float getHum(){
  return dht.readHumidity();
}
