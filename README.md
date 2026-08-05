# Temperature & Humidity Sensor Dashboard

A simple IoT project based on a **WeMos D1 Mini Pro (ESP8266)** and a **DHT22 sensor**.

The device connects to Wi-Fi, reads temperature and humidity values, and hosts a lightweight web dashboard that displays live sensor data and charts directly from the ESP8266.

## Features

* Temperature monitoring using a DHT22 sensor
* Humidity monitoring using a DHT22 sensor
* Embedded web server running on the ESP8266
* Real-time browser dashboard
* Live chart visualization using Chart.js
* JSON API endpoint for sensor data
* Simple status and testing endpoints
* PlatformIO-based development environment

## Hardware

### Components

* WeMos D1 Mini Pro (ESP8266)
* DHT22 temperature/humidity sensor
* USB cable
* Wi-Fi network

### Wiring

| DHT22 Pin | WeMos D1 Mini |
| --------- | ------------- |
| VCC       | 3.3V          |
| GND       | GND           |
| DATA      | D4            |

Current firmware configuration:

```cpp
#define DHT_PIN D4
#define DHT_TYPE DHT22
```

## Software Stack

### Firmware

* ESP8266 Arduino Framework
* ESP8266WebServer
* DHT Sensor Library
* Adafruit Unified Sensor

### Frontend

* HTML
* JavaScript
* Chart.js (loaded from CDN)

## Project Structure

```text
TemperatureHumiditySensor/
├── firmware/
│   ├── src/
│   │   ├── main.cpp
│   │   ├── sensor.cpp
│   │   ├── sensor.h
│   │   ├── supabase.cpp
│   │   ├── supabase.h
│   │   ├── wifi.cpp
│   │   ├── wifi.h
│   │   ├── led.cpp
│   │   ├── led.h
│   │   ├── logger.cpp
│   │   └── config.h
│   └── platformio.ini
│
└── dashboard-web/
    ├── index.html
    └── vercel.json
```

## Installation

### Clone the Repository

```bash
git clone <repository-url>
cd TemperatureHumiditySensor
```

### Open with PlatformIO

Open the `firmware` folder in Visual Studio Code with the PlatformIO extension installed.

### Configure Wi-Fi

Edit `src/config.h`:

```cpp
#define WIFI_SSID "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
```

### Build and Upload

PlatformIO core path added to user PATH (`%USERPROFILE%\.platformio\penv\Scripts`). Open a **new** terminal for it to take effect, then from `firmware/`:

```powershell
cd firmware
pio run
pio run --target upload
```

### Open Serial Monitor

```bash
pio device monitor
```

Serial monitor prints the assigned IP (`IP assegnato: 192.168.x.x`) — that's the dashboard URL.

Expected startup output:

```text
BOOT OK

Avvio WiFi...
WiFi connesso!
IP assegnato: 192.168.x.x
Web avviato
```

## Usage

After connecting to Wi-Fi, open a browser and navigate to:

```text
http://<device-ip>/
```

Example:

```text
http://192.168.0.119/
```

The dashboard will display:

* Current temperature
* Current humidity
* Live temperature graph

Data is refreshed every 2 seconds.

## API Endpoints

### Dashboard

```http
GET /
```

Returns the web dashboard.

### Sensor Data

```http
GET /data
```

Example response:

```json
{
  "temp": 24.8,
  "hum": 57.3
}
```

### Health Check

```http
GET /test
```

Response:

```text
OK
```

Useful for verifying that the web server is running correctly.

## PlatformIO Configuration

Target board:

```ini
board = d1_mini_pro
platform = espressif8266
framework = arduino
```

Dependencies:

```ini
adafruit/DHT sensor library
adafruit/Adafruit Unified Sensor
```

## Future Improvements

* MQTT publishing
* Home Assistant integration
* Historical data storage
* OTA firmware updates
* Responsive mobile dashboard
* Multiple sensor support
* Alarm thresholds
* Dark/light theme selection

## Web Dashboard (`dashboard-web/`)

Static, single-file dashboard (`dashboard-web/index.html`) that reads sensor history directly from Supabase. No build step — deploy the `dashboard-web/` directory as-is on Vercel.

**Before deploying:** the firmware and the dashboard share the same anon key, so RLS on the `readings` table must allow both `SELECT` and `INSERT` for the anon role (no `UPDATE`/`DELETE`). The dashboard embeds this key client-side by design — RLS is what keeps that safe, not secrecy of the key. If you want the dashboard to be read-only, give the firmware a separate key/policy instead of reusing the anon key.

## License

MIT License

## Author

Personal ESP8266 project built with PlatformIO and a WeMos D1 Mini Pro.
