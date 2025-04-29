# OpenIndus Framework

## Get started

[See our online documentation](https://openindus.com/oi-content/doc/index.html)

## Platform: ESP32

### Configuration

```bash
pio run -t menuconfig
```

### Build and flash

```bash
pio run -t upload --upload-port=/dev/ttyUSB0
```

### Monitor

```bash
pio device monitor -p /dev/ttyUSB0
```

## Platform: Linux ARM

### Toolchain

```bash
sudo apt install gcc-arm-linux-gnueabihf
sudo apt install g++-arm-linux-gnueabihf
```

### Configure and compile with CMake

```bash
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../../toolchain.cmake
make
```

### Install and run on OICore+

```bash
scp oi-firmware root@oi-core-plus.local:~/
ssh root@oi-core-plus.local
./oi-firmware
```