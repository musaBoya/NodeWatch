# NodeWatch

This repository can be used as a starter template for small telemetry or device-monitoring projects built with:

- **ESP32 firmware** for generating and sending telemetry data
- **Qt desktop application** for receiving and visualizing telemetry in real time

The project is designed as a simple local monitoring setup where an ESP32 device sends JSON telemetry data over Wi-Fi to a desktop receiver.

## Project Structure

```text
NodeWatch/
├── desktop/
│   └── qt_client/
├── firmware/
│   └── ep32_sender/
└── docs/
```

## Overview

### Desktop Receiver

The desktop app is a **Qt 6** application that:

* starts an HTTP server
* listens for telemetry on `/telemetry`
* parses incoming JSON payloads
* shows received records in the UI
* highlights high-temperature entries as errors using a configurable threshold
* loads runtime settings from a desktop config file

### ESP32 Sender

The firmware is an **ESP-IDF** project that:

* connects to Wi-Fi
* generates simple telemetry values
* sends them to the desktop application using HTTP POST
* repeats this at a configurable interval

## How It Works

1. The desktop application starts an HTTP server using configured bind address and port (defaults: `0.0.0.0:8080`)
2. The ESP32 connects to the configured Wi-Fi network
3. The ESP32 sends telemetry to:

```text
http://<desktop-ip>:8080/telemetry
```

4. The desktop app receives JSON payloads in this format:

```json
{
  "device_id": "esp32_01",
  "timestamp": 1710000000,
  "temperature": 27,
  "humidity": 48,
  "status": "ok"
}
```

5. Telemetry is displayed in the UI and saved to local SQLite storage
6. If temperature is above the configured threshold (default: `26`), the entry is also shown in the error list

## Features

* Simple local telemetry pipeline
* HTTP-based communication between ESP32 and desktop
* Qt desktop UI for live monitoring
* JSON telemetry parsing
* Local SQLite persistence for telemetry history
* Configurable firmware values via `menuconfig`
* Configurable desktop runtime settings via `nodewatch.ini`
* High-temperature warning behavior with configurable threshold

## Desktop Application

Location:

```text
desktop/qt_client
```

### Requirements

* Qt 6
* CMake 3.21+
* Qt components:

  * `Widgets`
  * `Network`
  * `HttpServer`
  * `Sql`

### Build

```bash
cd desktop/qt_client
cmake -B build
cmake --build build
```

### Run

```bash
./build/NodeWatchDesktop
```

When started, the app opens a window, loads `nodewatch.ini` (auto-created if missing), and begins listening for telemetry requests.

## ESP32 Firmware

Location:

```text
firmware/ep32_sender
```

### Requirements

* ESP-IDF
* ESP32 development board
* Wi-Fi connection shared with the desktop receiver

### Configuration

You can configure the firmware with:

```bash
idf.py menuconfig
```

Open:

```text
NodeWatch Sender Configuration
```

Available values include:

* WiFi SSID
* WiFi Password
* Telemetry Server URL
* Device ID
* Telemetry send interval

Default values currently include:

```text
SSID: CHANGE_ME
Password: CHANGE_ME
Server URL: http://0.0.0.0:8080/telemetry
Device ID: esp32_01
Interval: 10000 ms
```

### Build

```bash
cd firmware/ep32_sender
idf.py build
```

### Flash and Monitor

```bash
idf.py -p /dev/ttyUSB0 flash monitor
```

## Temperature Warning Logic

The desktop client treats telemetry entries with temperature greater than `alerts.temperature_threshold` as warning/error entries and displays them in a separate list.

The default threshold is `26.0`, and it can be changed in the desktop config file.

## Notes

* The current firmware sends generated sample telemetry values rather than reading from a physical sensor.
* The communication model is intended for local network testing and prototyping.
* The project is a good base for extending into:

  * real sensor integration
  * persistent logging
  * charts and dashboards
  * multi-device monitoring
  * alert rules
