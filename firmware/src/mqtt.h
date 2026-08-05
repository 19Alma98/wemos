#ifndef MQTT_H
#define MQTT_H
void mqttBegin();
void mqttLoop();
void mqttPublish(float temp, float hum);
#endif
