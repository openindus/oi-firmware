#include "OpenIndus.h"
#include "Arduino.h"

Analogls analogls;
float temperature = 0;
float temperature_b = 0;

int test = 0;

void setup(void)
{
    Analogls analogls = Analogls();
    analogls.AddRTD(ANALOG_CHANNEL_B, PT100, RTD_3WIRE, ADS1X4S08_FULLSCALE_5V, ADS1X4S08_MAG_100uA, 100/38.5);
    analogls.AddRTD(ANALOG_CHANNEL_A, PT100, RTD_3WIRE, ADS1X4S08_FULLSCALE_5V, ADS1X4S08_MAG_100uA, 100/38.5);
}

void loop(void)
{   
    
    temperature = analogls.GetChannelData(ANALOG_CHANNEL_A, PT100);
    temperature = analogls.GetChannelData(ANALOG_CHANNEL_A, PT100);
    temperature_b = analogls.GetChannelData(ANALOG_CHANNEL_B, PT100);
    temperature_b = analogls.GetChannelData(ANALOG_CHANNEL_B, PT100);

    printf("temperature:\n  - A : %f\n  - B : %f\n", temperature, temperature_b);

    delay(1000);
    test++;
}