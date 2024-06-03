#include "OpenIndus.h"
#include "Arduino.h"

#define NODE_ID 6

OICore core;

CAN_Message_t tx_msg = {.id=NODE_ID, .size=1, .IDE=false, .RTR=false, .msg={0}};

void setup(void)
{
    Serial.begin(115200);
    
    /* Initialize CAN */
    core.can.begin(1000000); // Bit rates : 1MHz
    core.can.setStandardFilter(0xFF0, 0x0AA);
}

void loop(void)
{
    static int counter = 0;

    tx_msg.msg[0] = counter;
    core.can.write(tx_msg);
    counter++;

    delay(1000);
}