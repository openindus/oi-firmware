/**
 * @file Modem.cpp
 * @brief Modem example
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "OpenIndus.h"
#include "Arduino.h"

Core core;

void setup(void)
{
    printf("Hello OpenIndus!\n");

    core.modem = new Modem();
    core.modem->begin("TM");
    
    int rssi, ber;
    core.modem->getSignalQuality(rssi, ber);
    printf("RSSI: %d, BER: %d\n", rssi, ber);

    core.modem->sendSMS("+336********", "test");

    core.modem->connect();
    core.modem->disconnect();
}

void loop(void)
{
    delay(1000);
}