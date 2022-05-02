#include "OpenIndus.h"
#include "Arduino.h"
#include <unity.h>

#define TEST_NUMBER_OF_CYCLES 1

void test_status(void)
{
    if (Stepper.getStatus(MOTOR1) == 0)
    {
        TEST_FAIL_MESSAGE("Unable to get status");
    }
}

void setup() 
{
    UNITY_BEGIN();
    for (unsigned int i=0; i<TEST_NUMBER_OF_CYCLES; i++)
    {
        RUN_TEST(test_status);
    }
    UNITY_END();
}

void loop() 
{
    delay(10);
}
