# NodeWatch Desktop Client

Qt 6 desktop receiver for NodeWatch telemetry.

## Features

- Starts an HTTP server on port `8080`
- Accepts `POST /telemetry` JSON payloads
- Displays all telemetry in the main list
- Shows high-temperature records (`temperature > 26`) in the alert list

## Requirements

- Qt 6 with modules: `Widgets`, `Network`, `HttpServer`
- CMake 3.21+

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/NodeWatchDesktop
```
