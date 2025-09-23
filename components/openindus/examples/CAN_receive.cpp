#include "OpenIndus.h"
#include "Arduino.h"

#define NODE_ID 11

Core core;

CAN_Message_t rx_msg;

void setup(void)
{    
    /* Initialize CAN */
    core.can.begin(1000000); // Bit rates : 1MHz
    core.can.setStandardFilter(0xFF0, 0x0AA);
}

void loop(void)
{
    if(core.can.available()) {
        rx_msg = core.can.read();
        printf("MSG: id(%ld), size(%d) data(%d)\n", rx_msg.id, rx_msg.size, rx_msg.msg[0]);        
    }

    delay(10);
}