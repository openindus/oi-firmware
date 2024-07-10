#include "OpenIndus.h"
#include "Arduino.h"

AnalogLS analogls;

void setup(void)
{
    printf("Hello OpenIndus !\n");
    delay(1000);

    analogls.setConversionTime(1000);
    analogls.addSensor(THERMOCOUPLE, TYPE_K, {AIN_E_P, AIN_E_N});
}

void loop(void)
{
    float data = 0.0;

    /* Thermocouple */
    data = analogls.tc[0].readVoltage();
    printf(">TC (CH0) - Voltage (mV): %f\n", data);
    data = analogls.tc[0].readTemperature();
    printf(">TC (CH0) - Temperature (°C): %f\n", data);
    printf("\n");
}