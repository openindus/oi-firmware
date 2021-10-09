#include "OpenIndus.h"
#include "Arduino.h"

void setup()
{
#ifdef DEBUG
    Serial.begin(115200);
#endif
}

void loop()
{
    Serial.println("Hello OpenIndus");
    delay(1000);
}
