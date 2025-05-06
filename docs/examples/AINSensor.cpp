#include "OpenIndus.h"
#include "Arduino.h"

AnalogLS analogls;

void setup(void)
{
    Serial.begin(115200);
    Serial.println("Hello OpenIndus !\n");
    delay(1000);

    // Add a PT1000 RTD sensor (two wire)
    analogls.addSensor(RTD_PT1000, {AIN_A_P, AIN_A_N});
    analogls.sensors[0]->setAcquisitionTime(SAMPLE_50_MS);
    analogls.sensors[0]->setStabilizationTime(50);

    // Add a type K thermocouple
    analogls.addSensor(THERMOCOUPLE_K, {AIN_B_P, AIN_B_N});

    // Add a strain gauge: SIGNAL+, SIGNAL-, EXCIT+, EXCIT-
    analogls.addSensor(STRAIN_GAUGE, {AIN_E_P, AIN_E_N, AIN_D_P, AIN_D_N});
}

void loop(void)
{
    float data = 0.0;

    // RTD sensor
    data = analogls.sensors[0]->readTemperature();
    Serial.printf(">RTD (CH0) - Temperature(°C): %.2f\n", data);
    delay(200);

    // Thermocouple
    data = analogls.sensors[1]->readMillivolts();
    Serial.printf(">TC (CH1) - Voltage (mV): %.2f\n", data);

    data = analogls.sensors[1]->readTemperature();
    Serial.printf(">TC (CH1) - Temperature (°C): %.2f\n", data);

    // Strain gauge
    data = analogls.sensors[2]->read();
    Serial.printf(">SG (ch2) - Value: %.2f\n", data);

    delay(250);
}
