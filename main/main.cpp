#include "OpenIndus.h"
#include "Arduino.h"

uint16_t status = 0;

void setup() 
{
    // put your setup code here, to run once:
}

void loop() 
{
    // put your main code here, to run repeatedly:
    status = Stepper.cmdGetStatus(0);
    delay(1000);
    ESP_LOGI("Test", "Status %d", status);
}
