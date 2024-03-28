#include "OpenIndus.h"
#include "Arduino.h"

using namespace OI;

Core core;

CAN_Message_t rx_msg;
CAN_Message_t tx_msg;

void setup()
{
    Serial.begin(115200);
    
    core.can.begin();

    tx_msg.id = 11;
    tx_msg.size = 8;
    tx_msg.msg[0] = 'A';
    tx_msg.msg[1] = 'U';
    tx_msg.msg[2] = 'R';
    tx_msg.msg[3] = 'E';
    tx_msg.msg[4] = 'L';
    tx_msg.msg[5] = 'I';
    tx_msg.msg[6] = 'E';
    tx_msg.msg[7] = 'N';

    tx_msg.IDE = false; // Standard ID, not extended
    tx_msg.RTR = false; // No remote request frame
    
    core.can.write(tx_msg);
    core.can.setStandardFilter(0xFF0, 0x0AA);
}

void loop()
{
    if(core.can.available())
    {
        rx_msg = core.can.read();
        Serial.printf("NEW MESSAGE // ID : %d, length : %d MSG :  ", rx_msg.id, rx_msg.size);
        for(int i =0; i <rx_msg.size; i++)
        {
            Serial.printf("%x", rx_msg.msg[i]);
        }
        Serial.printf(" // \n");
        
    }
    delay(100);
}