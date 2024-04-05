#include "OpenIndus.h"
#include "Arduino.h"

using namespace OI;

#define NODE_ID 9

Core core;

CAN_Message_t rx_msg;
CAN_Message_t tx_msg = {.id=NODE_ID, .size=1, .IDE=false, .RTR=false, .msg={0}};

int counter = 0;

void setup()
{
    Serial.begin(115200);
    
    /* Initialize CAN */
    core.can.begin(1000000); // Bit rates : 1MHz
    core.can.setStandardFilter(0xFF0, 0x0AA);
}

void loop()
{
    /* Read */
    if(core.can.available()) {
        rx_msg = core.can.read();
        Serial.printf("MSG: id(%d), size(%d) data(%d)\n", rx_msg.id, rx_msg.size, rx_msg.msg[0]);        
    }

    /* Write */
    tx_msg.msg[0] = counter;
    core.can.write(tx_msg);
    counter++;

    delay(1000);
}