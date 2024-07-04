#include "OpenIndus.h"
#include "Arduino.h"

AnalogLS analogls;

float data;

void setup(void)
{
    printf("Hello OpenIndus !\n");

    delay(1000);

    analogls.addSensor(RTD_TWO_WIRE, AINLS_A_P, AINLS_A_N);
    analogls.addSensor(RTD_TWO_WIRE, AINLS_E_P, AINLS_E_N);

    printf("*** RTD ***\n");

    /* RTD - channel 0 */
    printf("channel 0 : \n");

    analogls.rtd[0].select();
    data = analogls.rtd[0].readTemperature(1000);
    printf("Temperature = %f\n", data);

    delay(1000);

    /* RTD - channel 1 */
    printf("channel 1 : \n");

    analogls.rtd[1].select();
    data = analogls.rtd[1].readRTD(1000);
    printf("R_RTD = %f\n", data);

    analogls.rtd[1].select();
    data = analogls.rtd[1].readTemperature(1000);
    printf("Temperature = %f\n", data);
}

void loop(void)
{
    delay(1000);
}