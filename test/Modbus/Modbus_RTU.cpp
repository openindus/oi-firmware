#include "OpenIndus.h"
#include "Arduino.h"
#include "ArduinoModbus.h"

Core core;

void setup()
{
    printf("Modbus Temperature & Humidity Sensor\n");

    /* Start the Modbus RTU client */
    if (!ModbusRTUClient.begin(9600)) {
        printf("Failed to start Modbus RTU Client!\n");
        while (10);
    }
}

void loop()
{
    /* Send request to read the input register */
    if (!ModbusRTUClient.requestFrom(1, INPUT_REGISTERS, 0x0001, 2)) {
        printf("failed to read registers: %d\n", ModbusRTUClient.lastError());
    } else {
        /* Read temperature and humidity */
        float temperature = (ModbusRTUClient.read() / 10.0);
        float humidity = (ModbusRTUClient.read() / 10.0);
        printf("temperature: %d\n", temperature);
        printf("humidity: %d\n", humidity);
    }
    delay(5000);
}
