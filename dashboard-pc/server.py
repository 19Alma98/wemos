
import paho.mqtt.client as mqtt
import csv
from datetime import datetime

def on_message(client, userdata, msg):
    print(msg.payload.decode())
    data=eval(msg.payload.decode())

    with open("log.csv","a") as f:
        f.write(f"{datetime.now()},{data['temp']},{data['hum']}\n")

client=mqtt.Client()
client.on_message=on_message
client.connect("localhost",1883,60)
client.subscribe("wemos/dht22")
client.loop_forever()
