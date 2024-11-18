#include "OpenIndus.h"
#include "Arduino.h"

AnalogLS analogls;

void setup(void)
{
    Serial.begin(115200);
    Serial.print("Hello OpenIndus !");

    /* Add a strain gauge :           SIGNAL+  SIGNAL-  EXCIT+   EXCIT- */
    analogls.addSensor(STRAIN_GAUGE, {AIN_B_P, AIN_B_N, AIN_A_P, AIN_A_N});
}

void loop(void)
{
    Serial.print(">SG value: ");
    Serial.println(analogls.sg[0].read());

    delay(1000);
}