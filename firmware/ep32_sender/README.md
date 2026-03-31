# NodeWatch ESP32 Sender

This firmware sends telemetry data from an ESP32 device to the NodeWatch desktop receiver over Wi-Fi using HTTP POST.

## Configurable values

Use `idf.py menuconfig` and open:

`NodeWatch Sender Configuration`

You can set:

- WiFi SSID
- WiFi Password
- Telemetry Server URL
- Device ID
- Telemetry send interval

## Build

```bash
idf.py build