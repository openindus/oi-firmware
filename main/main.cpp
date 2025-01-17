#include "OpenIndus.h"

#if defined(ARDUINO_ARCH_ESP32)

#include "Arduino.h"

void setup(void)
{
    Serial.begin(115200);
    Serial.println("Hello Openindus !");
}

void loop(void)
{
    delay(1000);
}

#else

int main(void)
{
    return 0;
}

#endif