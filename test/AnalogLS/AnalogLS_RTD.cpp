#include "OpenIndus.h"
#include "Arduino.h"

AnalogLS analogls;

float data;

void setup(void)
{
    printf("Hello OpenIndus !\n");

    delay(1000);

    analogls.addSensor(RTD_TWO_WIRE, {AIN_A_P, AIN_A_N});
    analogls.addSensor(RTD_TWO_WIRE, {AIN_E_P, AIN_E_N});
    analogls.addSensor(RTD_TWO_WIRE, {AIN_E_N, AIN_D_N});

    printf("*** RTD ***\n");

    /* RTD - channel 0 */
    printf("channel 0 : \n");
    data = analogls.rtd[0].readTemperature();
    printf("\tTemperature = %f\n", data);

    delay(1000);

    /* RTD - channel 1 */
    printf("channel 1 : \n");
    data = analogls.rtd[1].readRTD();
    printf("\tR_RTD = %f\n", data);
    data = analogls.rtd[1].readTemperature();
    printf("\tTemperature = %f\n", data);

    /* RTD - channel 2 */
    printf("channel 2 : \n");
    data = analogls.rtd[2].readRTD();
    printf("\tR_RTD = %f\n", data);
    data = analogls.rtd[2].readTemperature();
    printf("\tTemperature = %f\n", data);
}

void loop(void)
{
    delay(1000);
}