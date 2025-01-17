#include "OpenIndus.h"
#include "Arduino.h"

AnalogLS analogls;

void setup(void)
{
    Serial.begin(115200);
    Serial.println("Hello OpenIndus !\n");
    delay(1000);

    // Add a three wire PT100
    analogls.addSensor(RTD_PT100, {AIN_A_P, AIN_A_N, AIN_B_N});
    
    // Set acquisition time to 50ms --> adc will sample input during 50ms and return a filtered value
    analogls.setAcquisitionTime(SAMPLE_50_MS); 
    
    // Wait 50ms before taking a sample for signal to establlish (through RC external RC filter for example)
    analogls.setStabilizationTime(50);
}

void loop(void)
{
    float data = 0.0;

    /* RTD */
    data = analogls.rtd[0].readTemperature();
    Serial.println(">RTD (CH0) - Temperature(°C): %f\n", data);
}