#include "OpenIndus.h"
#include "Arduino.h"

AnalogLS analogls;

void setup(void)
{
    printf("Hello OpenIndus !\n");
    delay(1000);

    analogls.addSensor(RAW_SENSOR,{AIN_A_P, AIN_A_N});
    // analogls.addSensor(RTD_TWO_WIRE, {AIN_B_P, AIN_B_N});
    // analogls.addSensor(RTD_TWO_WIRE, {AIN_C_P, AIN_C_N});
    // analogls.addSensor(RTD_TWO_WIRE, {AIN_D_P, AIN_D_N});
    // analogls.addSensor(RTD_TWO_WIRE, {AIN_E_P, AIN_E_N});
    analogls.setStabilizationTime(1000);
    analogls.setAcquisitionTime(SAMPLE_400_MS);
    analogls.raw[0].setGain(GAIN_4);
}

void loop(void)
{
    float data = 0.0;

    /* RTD */
    data = analogls.raw[0].readMillivolts();
    printf(">RAW (CH0): %f\n", data);
    // data = analogls.rtd[1].readTemperature();
    // printf(">RTD (CH1) - Temperature(°C): %f\n", data);
    // data = analogls.rtd[2].readTemperature();
    // printf(">RTD (CH2) - Temperature(°C): %f\n", data);
    // data = analogls.rtd[3].readTemperature();
    // printf(">RTD (CH3) - Temperature(°C): %f\n", data);
    // data = analogls.rtd[4].readTemperature();
    // printf(">RTD (CH4) - Temperature(°C): %f\n", data);
    printf("\n");
    // delay(1000);
}