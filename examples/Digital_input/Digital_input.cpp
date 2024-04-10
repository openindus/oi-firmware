#include "OpenIndus.h"
#include "Arduino.h"

#if defined OI_CORE
OICore module;
#elif defined OI_DISCRETE
OIDiscrete module;
#endif

void callback(void*)
{
    Serial.printf("Interrupt on DIN_1 !\n");
}

void setup(void)
{
    Serial.begin(115200);

    // Attach an interrupt callback to digital input 1
    module.attachInterrupt(DIN_1, callback);
}

void loop(void)
{
    // Read digital input 2
    int value = module.digitalRead(DIN_2);
    Serial.printf("Value of DIN_2: %d\n", value);

    delay(1000); // Wait 1s
}
