import os

from dotenv import load_dotenv

load_dotenv()

MQTT_BROKER = os.getenv("MQTT_BROKER", "localhost")
MQTT_PORT = int(os.getenv("MQTT_PORT", "1883"))
MQTT_TOPIC = os.getenv("MQTT_TOPIC", "wemos/dht22")
CSV_FILE = os.getenv("CSV_FILE", "logs/sensor_log.csv")
