//#define UNIT_TEST
#ifndef UNIT_TEST

#include <Arduino.h>
//#include "OI4GNode.h"

#include "OI4GMqtt.h"
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

#define TOPIC        "OI4G/demo/value"

#define MSG "salut aurelien :)"
static OI4GNode oi4g; 
static OI4GMqtt mqtt1;

static bool isReady;
void mqttPub(const char* topic, const char * msg);


void setup() {
  // put your setup code here, to run once:


    DEBUG_STREAM.begin(115200);
    DEBUG_STREAM.println("\nCode init mqtt pub...\n");
    
    oi4g.init(MODEM_STREAM);
    oi4g.setDebug(DEBUG_STREAM);
    mqtt1.begin(&oi4g);   
    isReady = oi4g.on();
    isReady = oi4g.connect(CURRENT_APN, CURRENT_URAT, CURRENT_MNO_PROFILE, CURRENT_OPERATOR, BAND_MASK_UNCHANGED, LTEM_BANDMASK);
    DEBUG_STREAM.println(isReady ? "Modem ready" : "Modem failed");

    DEBUG_STREAM.println("Setup done...");

    if (isReady) {

        mqttPub(TOPIC, MSG);
    }
}

void loop() {
  // put your main code here, to run repeatedly:
}

void mqttPub(const char* topic, const char * msg)
{

  DEBUG_STREAM.println("Msg size : ");
      DEBUG_STREAM.println(strlen(msg));
     mqtt1.mqttSetServer("test.mosquitto.org", 1883);
     mqtt1.mqttSetClientId("oi4gtest_pub");
     if(!mqtt1.mqttLogin()) {
       DEBUG_STREAM.println("Login error...");
     }
     
     if(!mqtt1.mqttPublish(topic, msg, strlen(msg))) { //you can sub to the topic on a linux terminal with the command : mosquitto_sub -h test.mosquitto.org -t "OI4G/demo/#"
       DEBUG_STREAM.println("Publish failed...");
     }
     mqtt1.mqttLogout();
}

#endif