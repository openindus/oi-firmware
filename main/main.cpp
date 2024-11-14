#include "OpenIndus.h"
#include "Arduino.h"

AnalogLS analogls;

void setup(void)
{
    // Serial.begin(115200);
    printf("Hello OpenIndus !\n");

    // Add a type K thermocouple
    analogls.addSensor(THERMOCOUPLE_K, {AIN_B_P, AIN_B_N});
}

void loop(void)
{
    float data = 0.0;

    // data = analogls.tc[0].readMillivolts();
    // printf(">TC (CH0) - Voltage (mV): %f\n", data);

    data = analogls.tc[0].readTemperature();
    printf(">TC (CH0) - Temperature (°C): %f\n", data);

    delay(1000);
}