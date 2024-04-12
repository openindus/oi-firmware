#include "OpenIndus.h"
#include "Arduino.h"
#include "ArduinoModbus.h"

OICore core;

void setup()
{
    Serial.begin(115200);
    Serial.println("Modbus Temperature & Humidity Sensor");

    /* Start the Modbus RTU client */
    if (!ModbusRTUClient.begin(9600)) {
        Serial.println("Failed to start Modbus RTU Client!");
        while (10);
    }
}

void loop()
{
    /* Send request to read the input register */
    if (!ModbusRTUClient.requestFrom(1, INPUT_REGISTERS, 0x0001, 2)) {
        Serial.print("failed to read registers! ");
        Serial.println(ModbusRTUClient.lastError());
    } else {
        /* Read temperature and humidity */
        float temperature = (ModbusRTUClient.read() / 10.0);
        float humidity = (ModbusRTUClient.read() / 10.0);
        Serial.print("temperature: ");
        Serial.println(temperature);
        Serial.print("humidity: ");
        Serial.println(humidity);
    }
    delay(5000);
}
