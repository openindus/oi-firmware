/**
 * @file main.cpp
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "OpenIndus.h"
#if defined(ARDUINO_ARCH_ESP32)
#include "Arduino.h"
#endif

void setup(void)
{
#if defined(ARDUINO_ARCH_ESP32)
    Serial.begin(115200);
    Serial.println("Hello Openindus !");
#endif
}

void loop(void)
{
#if defined(ARDUINO_ARCH_ESP32)
    delay(1000);
#endif
}