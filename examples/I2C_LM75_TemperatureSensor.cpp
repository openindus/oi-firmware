/**
 * @file I2C_LM75_TemperatureSensor.cpp
 * @author OpenIndus
 * @brief Example code for I2C communication with LM75 temperature sensor
 * @version 0.1
 * @date 2023-12-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include "OpenIndus.h"
#include "Arduino.h"
#include "Wire.h"

#define LM75_I2C_ADDRESS    0x48
#define LM75_TEMP_REG       0
#define LM75_CONFIG_REG     1

unsigned LM75_getReg(uint8_t reg)
{
    unsigned Result = 0xFFFF;
    Serial.print("getReg"); Serial.println(uint8_t(reg),HEX);

    Wire.beginTransmission(LM75_I2C_ADDRESS);
    Wire.write(reg); // pointer reg
    Wire.endTransmission();

    uint8_t c;

    Wire.requestFrom(LM75_I2C_ADDRESS, uint8_t(2));
    if (Wire.available())
    {
        c = Wire.read();
        Result = c;
        if(reg != LM75_CONFIG_REG)
        {
            Result = (unsigned(c))<<8;
            if(Wire.available())
            {
                c = Wire.read();
                Result |= (unsigned(c));
            }
            else
            {
                Serial.println("Error ");
                Result = 0xFFFF;
            }
        }
    }

    return(Result);
}

float LM75_getTemp()
{
    union
    {
        unsigned short tempX;
        short tempS;
    } temperature;

    temperature.tempX = LM75_getReg(LM75_TEMP_REG);
    return (temperature.tempS / 256.0F);
}

//-------------------------------------------------------------------------------

void setup() 
{
    Serial.begin(115200);
    Wire.begin();
}

void loop()
{
    Serial.print("Temperature from LM75 is: ");
    Serial.print(LM75_getTemp());
    Serial.println("°C");
}
