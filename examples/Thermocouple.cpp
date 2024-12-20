#include "OpenIndus.h"
#include "Arduino.h"

AnalogLS analogls;

void setup(void)
{
    Serial.begin(115200);
    Serial.println("Hello OpenIndus !\n");

    // Add a type K thermocouple
    analogls.addSensor(THERMOCOUPLE_K, {AIN_E_P, AIN_E_N});
}

void loop(void)
{
    float data = 0.0;

    data = analogls.sensors[0]->readMillivolts();
    Serial.print(">TC (CH0) - Voltage (mV): ");
    Serial.println(data);

    data = analogls.sensors[0]->readTemperature();
    Serial.print(">TC (CH0) - Temperature (°C): ");
    Serial.println(data);

    delay(250);
}
