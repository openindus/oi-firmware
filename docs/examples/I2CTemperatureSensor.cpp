/**
 * @file I2CTemperatureSensor.cpp
 * @author OpenIndus
 * @brief Example code for I2C communication with LM75 temperature sensor
 * @version 0.1
 * @date 2023-12-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <Arduino.h>
#include <Wire.h>

#define LM75_I2C_ADDRESS    0x48
#define LM75_TEMP_REG       0
#define LM75_CONFIG_REG     1

#define I2C_SDA_PIN         36
#define I2C_SCL_PIN         37

unsigned LM75_getReg(uint8_t reg)
{
    unsigned Result = 0xFFFF;

    Wire.beginTransmission(LM75_I2C_ADDRESS);
    Wire.write(reg); // pointer reg
    Wire.endTransmission();

    uint8_t c;

    Wire.requestFrom(LM75_I2C_ADDRESS, 2);
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
                Serial.println("Error");
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

    /* Initialize I2C */
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    
    /* Set clock speed */
    Wire.setClock(100000);
}

void loop()
{
    /* Read temperature */
    float temperature = LM75_getTemp();

    /* Print result */
    Serial.print("Temperature from LM75 is: ");
    Serial.print(temperature);
    Serial.println("°C");

    /* Wait until next read */
    delay(500);
}