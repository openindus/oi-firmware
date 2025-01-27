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
    printf("Hello OpenIndus!\n");
}

void loop(void)
{
#if defined(ARDUINO_ARCH_ESP32)
    delay(1000);
#endif
}