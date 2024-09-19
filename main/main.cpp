#include "OpenIndus.h"
#include "Arduino.h"

AnalogLS analogls;

void setup(void)
{
    printf("Hello OpenIndus !\n");
    delay(1000);

    analogls.addSensor(THERMOCOUPLE_T, {AIN_A_P, AIN_A_N});
    analogls.addSensor(RTD_TWO_WIRE, {AIN_C_P, AIN_C_N});
    // analogls.setConversionTime(100);
    
}

void loop(void)
{
    float data = 0.0;

    /* RTD */
    data = analogls.tc[0].readTemperature();
    printf(">RTD (CH0) - Temperature(°C): %f\n", data);
    
    data = analogls.rtd[0].readTemperature();
    printf(">RTD (CH2) - Temperature(°C): %f\n", data);
    printf("\n");
    delay(1000);
}