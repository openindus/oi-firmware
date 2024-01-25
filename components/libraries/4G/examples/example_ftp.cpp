//#define UNIT_TEST
#ifndef UNIT_TEST

#include <Arduino.h>
//#include "OI4GNode.h"

#include "OI4GFtp.h"
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
static OI4GFtp ftp1;

static bool isReady;
void ftpTest(const char * server, const char* user=NULL, const char * pw=NULL);


void setup() {
  // put your setup code here, to run once:


    DEBUG_STREAM.begin(115200);
    DEBUG_STREAM.println("\nCode init...\n");
    
    oi4g.init(MODEM_STREAM);
    oi4g.setDebug(DEBUG_STREAM);
    ftp1.begin(&oi4g);   
    isReady = oi4g.on();
    isReady = oi4g.connect(CURRENT_APN, CURRENT_URAT, CURRENT_MNO_PROFILE, CURRENT_OPERATOR, BAND_MASK_UNCHANGED, LTEM_BANDMASK);
    DEBUG_STREAM.println(isReady ? "Modem ready" : "Modem failed");

    DEBUG_STREAM.println("Setup done...");

    if (isReady) {

        ftpTest("ftp.swfwmd.state.fl.us", "anonymous", "aly.ndaw@openindus.com");
    }
}

void loop() {
  // put your main code here, to run repeatedly:
}

void ftpTest(const char * server, const char* user, const char * pw)
{
     ftp1.ftpSetServer(server);
     delay(1000);
     ftp1.ftptSetAuth(user, pw);
     char buffer[20];
     uint8_t read_buff[2048];
     uint32_t size;
   
     ftp1.ftpSetMode(false);
     ftp1.ftpResolveIp(server, buffer);
     ftp1.ftpSetAccount("test_account");
     if(ftp1.ftpLogin()) {
       //delay(1000);
       
       //ftp1.ftpGetInfos();
       
       ftp1.ftpRetrieveFile("README.txt", "test_file");
  
       
       oi4g.readFilePartial("test_file", read_buff, sizeof(read_buff),6000);
       DEBUG_STREAM.println((const char *)read_buff);
      
       oi4g.deleteFile("test_file");
       oi4g.listFiles();
     }
     

}

#endif