# OpenIndus Firmware Agent Instructions

This document helps AI coding agents become immediately productive in the OpenIndus ESP-IDF firmware codebase.

## Project Overview

| Aspect | Details |
|--------|---------|
| Framework | ESP-IDF v5.5 |
| Target | ESP32-S3 |
| Language | C/C++ (Arduino-compatible) |
| Build System | CMake |
| Board Support | OI-Core, OI-CoreLite, OI-Discrete, OI-Mixed, OI-Stepper, OI-RelayHP/LP, OI-AnalogLS, OIDc |

## Build & Deploy Commands

```bash
# Configure for ESP32-S3
idf.py set-target esp32s3
idf.py menuconfig

# Build
idf.py build

# Flash and monitor
idf.py flash -p /dev/ttyUSB0
idf.py monitor -p /dev/ttyUSB0
```

## Architecture Overview

```
components/
├── arduino/          # Arduino core + libraries
└── openindus/        # OpenIndus
    ├── api/          # API (Module, Middleware, System)
    ├── drivers/      # Drivers (ADS114S08, AD5413, etc.)
```

## Key Conventions

### File Headers
Every new file must begin with this Doxygen block:
```c
/**
 * @file filename.ext
 * @brief One-line description of the module
 * @author Your Name (your.email@openindus.com)
 * @copyright (c) [2026] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */
```

### Logging
- Always declare `static const char* TAG = "module_name";` before any function
- Use `ESP_LOGx(TAG, "...")` macros only — never `printf` or `puts`

### Naming
| Element | Convention | Example |
|---------|-----------|---------|
| C functions | `snake_case` | `discrete_read_input` |
| C++ classes | `PascalCase` | `SensorDriver` |
| C++ methods | `camelCase` | `readTemperature` |
| Macros | `UPPER_CASE` | `MAX_RETRY_COUNT` |

### Error Handling

All driver functions return `esp_err_t`. Use `ESP_OK` for success, and check error codes:

```c
esp_err_t ret = driver_init();
if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Init failed: %s", esp_err_to_name(ret));
    return ret;
}
```

## Important Directories

| Path | Purpose |
|------|---------|
| `main/` | Main application entry point (`main.cpp`) |
| `components/openindus/` | OpenIndus APIs and drivers |
| `components/arduino/` | Arduino core and libraries |
| `docs/` | Documentation (see [`docs/README.md`](docs/README.md)) |
| `test/` | Functional tests (see [`test/README.md`](test/README.md)) |

## Related Documentation

- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/v5.5/esp32s3/index.html)
- [Arduino-ESP32 Documentation](https://docs.espressif.com/projects/arduino-esp32/en/latest/)
- [OpenIndus Online Docs](https://openindus.com/oi-content/doc/index.html)
