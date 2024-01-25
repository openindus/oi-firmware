//#define UNIT_TEST
#ifndef UNIT_TEST

#include <Arduino.h>
//#include "OI4GNode.h"

#include "OI4GCustom.h"
#include <Wire.h>

#include <string>
#include <sstream>

#define DEBUG_STREAM Serial

#define OPERATOR 
#define CURRENT_APN      "TM"
#define CURRENT_OPERATOR AUTOMATIC_OPERATOR
#define CURRENT_URAT     OI4G_LTEM_URAT  
#define CURRENT_MNO_PROFILE MNOProfiles::STANDARD_EUROPE

#ifndef LTEM_BANDMASK
#define LTEM_BANDMASK BAND_MASK_UNCHANGED
#endif

#define READ_TIMEOUT_MS 100
#define BUFFER_SIZE 256


static OI4GNode oi4g; 
static OI4GCustom cus1;

static bool isReady;
void socketTest(const char * server);


void setup() {
  // put your setup code here, to run once:


    DEBUG_STREAM.begin(115200);
    DEBUG_STREAM.println("\nCode init...\n");
    
    oi4g.init(MODEM_STREAM);
    oi4g.setDebug(DEBUG_STREAM);
    cus1.begin(&oi4g);   
    isReady = oi4g.on();
    isReady = oi4g.connect(CURRENT_APN, CURRENT_URAT, CURRENT_MNO_PROFILE, CURRENT_OPERATOR, BAND_MASK_UNCHANGED, LTEM_BANDMASK);
    DEBUG_STREAM.println(isReady ? "Modem ready" : "Modem failed");

    DEBUG_STREAM.println("Setup done...");

    if (isReady) {

        socketTest("ftp.u-blox.com");
    }
}

void loop() {
  // put your main code here, to run repeatedly:
}

void socketTest(const char * server)
{
     char buffer[20];
     
     uint8_t receive_buffer[32];
     cus1.socketCreate(0,(Protocols)TCP);
     cus1.socketResolveIp(server, buffer);
     cus1.socketConnect(0, buffer, 7);
     if(cus1.socketWaitForRead(0, 3000)) {
       cus1.socketRead(0,receive_buffer,sizeof(receive_buffer));
     }
     DEBUG_STREAM.print("Received data : ");
     DEBUG_STREAM.println((const char*)receive_buffer);
}

#endif