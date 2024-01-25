//#define UNIT_TEST
#ifndef UNIT_TEST

#include <Arduino.h>
//#include "OI4GNode.h"
#include "OI4GHttp.h"
#include "OI4GMqtt.h"
#include "OI4GSms.h"
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

#define DEST        "+33638260230"
#define TEXT      "Test message"
#define PIN        "1503"

static OI4GNode oi4g; 
static OI4GSms sms1;

static bool isReady;
bool        isAlreadyConnected = false; 
static bool isSend;
void sendMsg(const char * dest, const char * text);
static void receiveHandler(char* buffer);
char buffer[520];

void setup() {
  // put your setup code here, to run once:


    DEBUG_STREAM.begin(115200);
    DEBUG_STREAM.println("\nCode init...\n");
    
    oi4g.init(MODEM_STREAM);
    oi4g.setDebug(DEBUG_STREAM);
    
    if(!oi4g.isOn()) {
      isReady = oi4g.on();
    }
    
    //if(!oi4g.isConnected()) {
      //isAlreadyConnected = false;
       isReady = oi4g.connect(CURRENT_APN, CURRENT_URAT, CURRENT_MNO_PROFILE, CURRENT_OPERATOR, BAND_MASK_UNCHANGED, LTEM_BANDMASK);
    //}
   
    DEBUG_STREAM.println((isReady || isAlreadyConnected) ? "Modem ready" : "Modem failed");

    DEBUG_STREAM.println("Setup done...");

    if ((isReady || isAlreadyConnected)) {

        sms1.begin(&oi4g, "1503");  
        sendMsg(DEST, TEXT);
        
    }

    
}

void loop() {
  // put your main code here, to run repeatedly:
  sms1.smsLoop();
}

void sendMsg(const char * dest, const char * text)
{
    
    sms1.selectMessageFormat(true);  //text mode
    //sms1.checkMessageStorage();
    sms1.setServiceCenterAddr("+33689004000");
    //sms1.incomeMessageAlertMode(2,1);
    delay(3000);
    //sms1.writeTextToMemory("+33638260230", "hello test", buffer, sizeof(buffer));
    //sms1.getServiceCenterAddr(buffer, sizeof(buffer));
    isSend= sms1.sendFromStorage(0);
    if(isSend) {
      DEBUG_STREAM.println("SMS send...");
    }

    else {
      DEBUG_STREAM.println("Error sending SMS...");
    }

   //isSend= sms1.sendTextMessage(dest, text);
  //DEBUG_STREAM.println(isSend);
  sms1.readMessage(sms1.getLastMsgIndex(),buffer, sizeof(buffer));
  DEBUG_STREAM.println(buffer);
}

static void receiveHandler(char* buffer) {
  sms1.readMessage(sms1.getLastMsgIndex(), buffer, sizeof(buffer) );
  DEBUG_STREAM.println("New message : ");
  DEBUG_STREAM.println(buffer);
}

#endif