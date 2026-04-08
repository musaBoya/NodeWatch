# NodeWatch Desktop Client

Qt 6 desktop receiver for NodeWatch telemetry.

## Source Layout

- `config/`: loading and validating desktop runtime settings
- `logging/`: reusable logger module (ported from `stl_wrapper`)
- `server/`: HTTP endpoint and request parsing
- `models/`: telemetry data model
- `storage/`: SQLite persistence layer
- `ui/`: Qt widgets and window behavior
- `rules/`: alert/threshold rules

## Features

- Loads runtime config from `nodewatch.ini` (auto-created on first run)
- Accepts `POST /telemetry` JSON payloads
- Displays all telemetry in the main list
- Applies configurable alert thresholds for warnings
- Persists telemetry to a local SQLite database
- Loads recent telemetry history on startup
- Supports configurable server bind address, port, db path, and log level

## Requirements

- Qt 6 with modules: `Widgets`, `Network`, `HttpServer`, `Sql`
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

## Configuration

- Default config path: `./build/nodewatch.ini` (next to executable)
- Override path: set `NODEWATCH_CONFIG=/absolute/path/to/nodewatch.ini`
- Example config template: `nodewatch.ini.example`

Available keys:

- `server.port`
- `server.bind_address`
- `storage.db_path`
- `alerts.temperature_threshold`
- `logging.level` (`debug`, `info`, `warning`, `error`)
- `logging.file_path`
