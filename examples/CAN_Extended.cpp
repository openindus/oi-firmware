#include "OpenIndus.h"
#include "Arduino.h"

CAN_Message_t rx_msg;
CAN_Message_t tx_msg;

void setup()
{
    CAN.begin(1000000, true);

    tx_msg.id = 11111;
    tx_msg.size = 8;
    tx_msg.msg[0] = 'A';
    tx_msg.msg[1] = 'U';
    tx_msg.msg[2] = 'R';
    tx_msg.msg[3] = 'E';
    tx_msg.msg[4] = 'L';
    tx_msg.msg[5] = 'I';
    tx_msg.msg[6] = 'E';
    tx_msg.msg[7] = 'N';

    tx_msg.IDE = true; // Extended ID, not standard
    tx_msg.RTR = false; // No remote request frame
    
    CAN.write(tx_msg);
    CAN.setExtendedFilter(0xFFF0, 0xAAA0);
}

void loop()
{
    if(CAN.available())
    {
        rx_msg = CAN.read();
        printf("NEW MESSAGE // ID : %d, length : %d MSG :  ", rx_msg.id, rx_msg.size);
        for(int i =0; i <rx_msg.size; i++)
        {
            printf("%x", rx_msg.msg[i]);
        }
        printf(" // \n");
        
    }
    delay(100);
}