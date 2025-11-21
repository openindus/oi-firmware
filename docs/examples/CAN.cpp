#include "OpenIndus.h"
#include "Arduino.h"

Core core;

CAN_Message_t rxMsg;
CAN_Message_t txMsg;

void setup(void)
{
    Serial.begin(115200);
    
    core.can.begin(); // Initialize CAN peripheral

    txMsg.id = 11;
    txMsg.size = 8;
    txMsg.msg[0] = 'A';
    txMsg.msg[1] = 'U';
    txMsg.msg[2] = 'R';
    txMsg.msg[3] = 'E';
    txMsg.msg[4] = 'L';
    txMsg.msg[5] = 'I';
    txMsg.msg[6] = 'E';
    txMsg.msg[7] = 'N';
    txMsg.IDE = false; // Standard ID
    txMsg.RTR = false; // No remote request frame
    
    core.can.write(txMsg);
    core.can.setStandardFilter(0xFF0, 0x0AA);
}

void loop(void)
{
    if(core.can.available()) {
        rxMsg = core.can.read();
        Serial.printf("ID: %lx Size: %d Data: ", rxMsg.id, rxMsg.size);
        for(int i =0; i <rxMsg.size; i++) {
            Serial.printf("%x", rxMsg.msg[i]);
        }
        Serial.printf("\n");    
    }
    delay(100);
}