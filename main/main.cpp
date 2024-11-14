#include "OpenIndus.h"
#include "Arduino.h"


AnalogLS ls;
int rtd_index;

void setup(void)
{
    delay(1000);
    // Serial.begin(115200);
    // Serial.println("Hello World !");
    rtd_index = ls.addSensor(RTD_PT100, {AIN_A_P, AIN_A_N});
    rtd_index = ls.addSensor(RTD_PT100, {AIN_B_P, AIN_B_N});
    rtd_index = ls.addSensor(RTD_PT100, {AIN_C_P, AIN_C_N});
    ls.setAcquisitionTime(SAMPLE_100_MS);
    ls.setStabilizationTime(1000);
}

void loop(void)
{
    printf("resistor:%f\n",ls.rtd[0].readResistor());
    printf("resistor:%f\n",ls.rtd[1].readResistor());
    printf("resistor:%f\n",ls.rtd[2].readResistor());
    delay(1000);
}