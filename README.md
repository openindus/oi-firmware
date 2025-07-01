# OpenIndus Framework

[![Version](https://img.shields.io/badge/version-1.7.3-blue.svg)](https://github.com/openindus/oi-firmware)
[![Platform](https://img.shields.io/badge/platform-ESP32-green.svg)](https://docs.espressif.com/projects/esp-idf/en/latest/)
[![Framework](https://img.shields.io/badge/framework-ESP--IDF%20%7C%20Arduino-orange.svg)](https://platformio.org/)

OpenIndus Framework is a library for developing applications on OpenIndus industrial modules. It provides a unified API to control digital/analog I/O, stepper motors, relays, and communication protocols (CAN, RS485, Ethernet, Wi-Fi, Bluetooth).

## Supported Modules

- **OI-Core** - Main module
- **OI-Discrete** - Digital I/O
- **OI-Mixed** - Digital and Analog I/O
- **OI-Stepper** - Stepper motor
- **OI-RelayHP** - High power relays
- **OI-Dc** - DC motor
- **OI-AnalogLS** - Analog sensors (PT100, PT1000, Thermocouples, etc.)

## Quick Start

```cpp
#include "OpenIndus.h"
#include "Arduino.h"

Discrete discrete;

void setup() {
    printf("Hello OpenIndus!\n");
}

void loop() {
    int value = discrete.digitalRead(DIN_1);
    printf("DIN_1: %d\n", value);
    delay(1000);
}
```

## Build and Flash

```bash
# Configuration
pio run -t menuconfig

# Build and flash
pio run -e core -t upload --upload-port=/dev/ttyUSB0

# Monitor
pio device monitor -p /dev/ttyUSB0
```

### Build Targets

Available environments: `core`, `discrete`, `mixed`, `stepper`, `relayhp`, `dc`, `analogls`

## Documentation

[See our online documentation](https://openindus.com/oi-content/doc/index.html)

## Support

- Email: contact@openindus.com
- Issues: [GitHub Issues](https://github.com/openindus/oi-firmware/issues)