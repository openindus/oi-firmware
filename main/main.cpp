#include "OpenIndus.h"
#include "Arduino.h"

AnalogLS analogls;

void setup(void)
{
    // Serial.begin(115200);
    printf("Hello OpenIndus !\n");

    // Add a type K thermocouple
    analogls.addSensor(THERMOCOUPLE_J, {AIN_A_P, AIN_A_N});
    analogls.addSensor(THERMOCOUPLE_K, {AIN_B_P, AIN_B_N});
    analogls.addSensor(THERMOCOUPLE_T, {AIN_C_P, AIN_C_N});
    analogls.addSensor(RTD_PT100, {AIN_D_P, AIN_D_N});
}

void loop(void)
{
    float data = 0.0;

    // data = analogls.tc[0].readMillivolts();
    // printf(">TC (CH0) - Voltage (mV): %f\n", data);

    data = analogls.tc[0].readTemperature();
    printf(">TC J (CH0) - Temperature (°C): %f\n", data);
    data = analogls.tc[1].readTemperature();
    printf(">TC K (CH0) - Temperature (°C): %f\n", data);
    data = analogls.tc[2].readTemperature();
    printf(">TC T (CH0) - Temperature (°C): %f\n", data);
    data = analogls.rtd[0].readTemperature();
    printf(">RTD - Temperature (°C): %f\n", data);
    printf("\n\n");

    delay(1000);
}