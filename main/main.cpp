#include "OpenIndus.h"
#include "Arduino.h"

AnalogLS ls;
int rtd_index;
void setup(void)
{
    // Serial.begin(115200);
    // Serial.println("Hello World !");
    printf("add sensor\n");
    rtd_index = ls.addSensor(RTD_PT100, {AIN_C_P, AIN_C_N});
    printf("add acq\n");
    ls.setAcquisitionTime(SAMPLE_100_MS);
    printf("add stab\n");
    ls.setStabilizationTime(1000);
}

void loop(void)
{
    printf("resistor:%f\n",ls.rtd[0].readResistor());
    delay(1000);
}