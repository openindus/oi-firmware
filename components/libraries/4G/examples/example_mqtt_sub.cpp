//#define UNIT_TEST
#ifndef UNIT_TEST

#include <Arduino.h>
//#include "OI4GNode.h"

#include "OI4GMqtt.h"
#include <Wire.h>

#include <string>
#include <sstream>

#define MODEM_STREAM Serial1
#define RX 35
#define TX 32
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
static OI4GMqtt mqtt1;

static bool isReady;
void mqttInit();
static void handlePublish(const char* topic, const char* msg);


void setup() {
  // put your setup code here, to run once:


    DEBUG_STREAM.begin(115200);
    //MODEM_STREAM.begin(oi4g.getBaudrate());
    MODEM_STREAM.begin(oi4g.getBaudrate(),SERIAL_8N1,RX,TX);
    DEBUG_STREAM.println("\nCode init...\n");
    
    oi4g.init(MODEM_STREAM);
    oi4g.setDebug(DEBUG_STREAM);
    mqtt1.begin(&oi4g);   
    isReady = oi4g.on();
    isReady = oi4g.connect(CURRENT_APN, CURRENT_URAT, CURRENT_MNO_PROFILE, CURRENT_OPERATOR, BAND_MASK_UNCHANGED, LTEM_BANDMASK);
    DEBUG_STREAM.println(isReady ? "Modem ready" : "Modem failed");

    DEBUG_STREAM.println("Setup done...");

    if (isReady) {

        mqttInit();
    }
}

static uint32_t prevPing = millis();
char receive_buffer[200];

void loop() {                                           //you can publish to the topic on a linux terminal with the command : mosquitto_pub -h test.mosquitto.org -t "OI4G/demo/value" -m "text"
  // put your main code here, to run repeatedly:
  mqtt1.mqttReadMessages(receive_buffer, sizeof(receive_buffer));
    
  
  uint32_t now = millis();
    int32_t since = now - prevPing;
    if (since > (1000L * 4 * 60)) {
        // Send ping
        if (!mqtt1.mqttPing("test.mosquitto.org")) {
            DEBUG_STREAM.println("Ping failed...");
        } 

        else {
          DEBUG_STREAM.println("Ping OK...");
        }
    
        prevPing = now;
    }
    delay(5000);
}
void mqttInit()
{
     mqtt1.mqttSetServer("test.mosquitto.org", 1883);
     mqtt1.mqttSetClientId("oi4gtest_sub");
     //mqtt1.mqttSetPublishHandler(handlePublish);
     
     if(mqtt1.mqttLogin()) {
      if(mqtt1.mqttSubscribe("OI4G/test/topic")) {
        DEBUG_STREAM.println("Subscribe failed...");
      }
  }

}

/*static void handlePublish(const char* topic, const char* msg) {
  DEBUG_STREAM.println(topic);
  DEBUG_STREAM.println(msg);
}*/

#endif