# OpenIndus library

## Supported Modules

- OICore / OICoreLite
- OIDiscrete
- OIMixed
- OIStepper
- OIRelayHP / OIRelayLP
- OIAnalogLS
- OIDc

## Quick Start

```cpp
#include "OpenIndus.h"

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

## Documentation

[See our online documentation](https://openindus.com/oi-content/doc/index.html)

## Support

- Email: contact@openindus.com
- Issues: [GitHub Issues](https://github.com/openindus/oi-firmware/issues)