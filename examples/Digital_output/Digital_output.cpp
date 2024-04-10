#include "OpenIndus.h"
#include "Arduino.h"

#ifdef OI_CORE
OICore core;
#endif
OIDiscrete discrete;

void monitor_current(void*)
{
    while (1) {    
        Serial.print("Get output current: ");
        Serial.println(discrete.getCurrent(DOUT_1));
        delay(100);
    }
}

void setup(void)
{
    Serial.begin(115200);

    // Task to monitor output current
    xTaskCreate(monitor_current, "Task", 10000, NULL, 1, NULL);
}

void loop(void)
{
    // Enable digital output 1
    discrete.digitalWrite(DOUT_1, HIGH);

    // Wait 1s
    delay(1000);

    // Disable digital output 1
    discrete.digitalWrite(DOUT_1, LOW);

    // Wait 1s
    delay(1000);
}