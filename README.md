# OpenIndus Firmware

[![Version](https://img.shields.io/badge/version-2.0.0-blue.svg)]()
[![Framework](https://img.shields.io/badge/framework-ESP--IDF_V5.5-green.svg)](https://docs.espressif.com/projects/esp-idf/en/v5.5/esp32s3/index.html)

## Configuration

```bash
idf.py set-target esp32s3
idf.py menuconfig
```

## Build firmware

```bash
idf.py build
```

## Flash and monitor

```bash
idf.py flash -p /dev/ttyUSB0
idf.py monitor -p /dev/ttyUSB0
```