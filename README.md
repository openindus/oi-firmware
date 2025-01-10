# OpenIndus Firmware

## Get started

[See our online documentation](https://openindus.com/oi-content/doc/index.html)

## Platform: ESP32

### Configuration

```bash
pio run -t menuconfig
```

### Build & flash

```bash
pio run -t upload
```

### Monitor

```bash
pio device monitor
```

## Platform: Linux ARM

### Build

```bash
mkdir build
cd build
cmake ..
make
```

### Installation

### Run

```bash
./OI-Firmware
```

## PIO Library

How to generate PlatformIO librarie.