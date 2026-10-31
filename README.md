# Temperature & Humidity Sensor Dashboard

A simple IoT project based on a **WeMos D1 Mini Pro (ESP8266)** and a **DHT22 sensor**.

The device wakes up periodically, connects to Wi-Fi, reads temperature and humidity, publishes the reading to a **Supabase** table over HTTPS, and goes back to deep sleep. A static **dashboard** (hosted on Vercel) reads the history from Supabase and shows live values and charts.

```text
DHT22 → ESP8266 (deep sleep, every hour) → Supabase (readings table) → dashboard-web (Vercel)
```

## Features

* Temperature and humidity monitoring using a DHT22 sensor
* Deep sleep between readings to save power
* Readings stored in Supabase (Postgres) over the Supabase REST API
* Static browser dashboard with live values and historical charts (Chart.js)
* No build step for the dashboard: single `index.html`
* PlatformIO-based development environment

## Hardware

### Components

* WeMos D1 Mini Pro (ESP8266)
* DHT22 temperature/humidity sensor
* USB cable
* Wi-Fi network (2.4 GHz)

### Wiring

| DHT22 Pin | WeMos D1 Mini |
| --------- | ------------- |
| VCC       | 3.3V          |
| GND       | GND           |
| DATA      | D4            |

Current firmware configuration (`firmware/src/config.h`):

```cpp
#define DHT_PIN D4
#define DHT_TYPE DHT22
#define DEEP_SLEEP_SECONDS 3600
```

## Software Stack

### Firmware

* ESP8266 Arduino Framework
* ESP8266 WiFi, `WiFiClientSecure` and `ESP8266HTTPClient`
* DHT Sensor Library
* Adafruit Unified Sensor

### Backend

* Supabase (Postgres + REST API), table `readings`

### Frontend

* `dashboard-web/index.html` (HTML, CSS, JavaScript)
* `@supabase/supabase-js` and Chart.js, loaded from CDN
* Vercel for static hosting

## Project Structure

```text
TemperatureHumiditySensor/
├── firmware/
│   ├── src/
│   │   ├── main.cpp          # boot → Wi-Fi → read sensor → publish → deep sleep
│   │   ├── sensor.cpp/.h     # DHT22 wrapper
│   │   ├── wifi.cpp/.h       # Wi-Fi connection
│   │   ├── supabase.cpp/.h   # HTTPS POST to the readings table
│   │   ├── led.cpp/.h        # onboard LED status
│   │   ├── logger.cpp        # placeholder (empty)
│   │   ├── config.h          # local secrets, NOT in git
│   │   └── config.h.example  # template for config.h
│   ├── platformio.ini
│   └── .gitignore
│
├── dashboard-web/
│   ├── index.html
│   └── vercel.json
│
└── README.md
```

## Installation

### Clone the Repository

```bash
git clone <repository-url>
cd TemperatureHumiditySensor
```

### Open with PlatformIO

Open the `firmware` folder in Visual Studio Code with the PlatformIO extension installed.

### Configure the Firmware

Copy the template and fill in your values:

```powershell
cd firmware/src
copy config.h.example config.h
```

Edit `src/config.h`:

```cpp
#define WIFI_SSID "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

#define SUPABASE_URL "https://YOUR-PROJECT-REF.supabase.co"
#define SUPABASE_SERVICE_KEY "YOUR-SERVICE-ROLE-KEY"
#define SUPABASE_TABLE "readings"
```

* `SUPABASE_SERVICE_KEY` is the **service_role** key (Supabase → Project Settings → API → "secret" key), not the anon key. It bypasses RLS, so it must stay in `config.h` only. `config.h` is gitignored.
* Changing Wi-Fi network: only `WIFI_SSID` and `WIFI_PASSWORD` need to change. The device publishes over the internet, so no local IP setup is required. The ESP8266 supports 2.4 GHz only, and does not support enterprise (WPA2-Enterprise) or captive-portal networks.

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

Expected output on each wake-up:

```text
BOOT OK
Avvio WiFi...
WiFi connesso, IP: 192.168.x.x
temp=24.8 hum=57.3
Pubblicato su Supabase.
Deep sleep...
```

If Wi-Fi does not connect within `WIFI_CONNECT_TIMEOUT_MS` (15 s), the device logs `WiFi timeout, vado in sleep senza pubblicare.` and sleeps without publishing.

## Supabase Setup

1. Create a project on [supabase.com](https://supabase.com).
2. Create a table `readings`:

   | column       | type                     | notes                      |
   | ------------ | ------------------------ | -------------------------- |
   | `id`         | `bigint` (identity), PK  |                            |
   | `created_at` | `timestamptz`            | default `now()`            |
   | `temp`       | `float8`                 | °C                         |
   | `hum`        | `float8`                 | %RH                        |

3. Enable RLS on `readings` and add a policy so the anon role can `SELECT` (used by the dashboard). No `INSERT`/`UPDATE`/`DELETE` policy for anon is needed, since the firmware writes with the service_role key.
4. Copy the project URL and keys into `config.h` (firmware) and `dashboard-web/index.html` (dashboard, anon key only).

**Free tier note:** Supabase pauses free projects after 7 days without activity. If the dashboard stops updating, check the project in the Supabase dashboard and click **Restore project**. The URL and keys stay the same.

## Usage

Deploy the dashboard:

1. Import the repository on Vercel (or drag `dashboard-web/` in).
2. Use the default static settings. `vercel.json` only sets `cleanUrls`.
3. Open the deployed URL.

The dashboard displays:

* Current temperature and humidity
* Trend compared to the previous reading
* Historical temperature and humidity chart, with a selectable time range

## Security Notes

* The dashboard embeds the **anon** key client-side by design. Row Level Security is what keeps it safe, so the anon role must only have `SELECT` on `readings`.
* The firmware uses the **service_role** key, which bypasses RLS. Never put it in `dashboard-web/` or any other public file.

## PlatformIO Configuration

Target board (`firmware/platformio.ini`):

```ini
[env:d1_mini_pro]
platform = espressif8266
board = d1_mini_pro
framework = arduino
monitor_speed = 115200
```

Dependencies:

```ini
adafruit/DHT sensor library
adafruit/Adafruit Unified Sensor
```

## Future Improvements

* Home Assistant integration
* OTA firmware updates
* Responsive mobile dashboard polish
* Multiple sensor support
* Alarm thresholds
* Dark/light theme selection

## License

MIT License

## Author

Personal ESP8266 project built with PlatformIO and a WeMos D1 Mini Pro.
