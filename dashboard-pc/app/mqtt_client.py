import json

import paho.mqtt.client as mqtt

from .config import MQTT_BROKER, MQTT_PORT, MQTT_TOPIC
from .csv_logger import log_reading


def on_connect(client, userdata, flags, reason_code, properties):
    print(f"Connesso al broker (rc={reason_code})")
    client.subscribe(MQTT_TOPIC)


def on_message(client, userdata, msg):
    print(msg.payload.decode())
    try:
        data = json.loads(msg.payload.decode())
    except (json.JSONDecodeError, UnicodeDecodeError):
        print("Payload non valido, ignorato")
        return

    log_reading(data["temp"], data["hum"])


def build_client() -> mqtt.Client:
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    client.on_connect = on_connect
    client.on_message = on_message
    return client


def run() -> None:
    client = build_client()
    client.connect(MQTT_BROKER, MQTT_PORT, 60)
    client.loop_forever()
