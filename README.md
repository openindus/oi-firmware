# OpenIndus Framework

[![Version](https://img.shields.io/badge/version-1.7.3-blue.svg)]()
[![Platform](https://img.shields.io/badge/platform-PlatformIO-green.svg)](https://docs.platformio.org/en/latest/)
[![Framework](https://img.shields.io/badge/framework-ESP--IDF%20%7C%20Arduino-orange.svg)](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/index.html)

The OpenIndus Framework is a library for developing applications on OpenIndus industrial modules. It provides a unified API to control digital and analog I/O, stepper motors, relays, and communication protocols such as RS485, CAN, and Ethernet.

## Supported Modules

- OICore / OICoreLite
- OIDiscrete
- OIMixed
- OIStepper
- OIRelayHP
- OIDc
- OIAnalogLS

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