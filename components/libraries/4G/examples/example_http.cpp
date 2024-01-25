//#define UNIT_TEST
#ifndef UNIT_TEST

#include <Arduino.h>
//#include "OI4GNode.h"
#include "OI4GHttp.h"
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

#define HTTP_HOST        "4gdev.openindus.com"
#define HTTP_PORT        80
#define HTTP_QUERY       "/service.php?type=send_data&name=temp1&val1="

static OI4GNode oi4g; 
static OI4GHttp http1;

static bool isReady;
void sendRequest(float * sensorVal);
float sensorVal = 13.0;
char test_buffer[2048];
const char* post_data = "test";


void setup() {
  // put your setup code here, to run once:


    DEBUG_STREAM.begin(115200);
    DEBUG_STREAM.println("\nCode init...\n");
    
    oi4g.init(MODEM_STREAM);
    oi4g.setDebug(DEBUG_STREAM);
    
     
    isReady = oi4g.on();
    isReady = oi4g.connect(CURRENT_APN, CURRENT_URAT, CURRENT_MNO_PROFILE, CURRENT_OPERATOR, BAND_MASK_UNCHANGED, LTEM_BANDMASK);
    DEBUG_STREAM.println(isReady ? "Modem ready" : "Modem failed");

    DEBUG_STREAM.println("Setup done...");

    if (isReady) {

         http1.begin(&oi4g); 
        http1.httpPost("postman-echo.com", HTTP_PORT, "/post", test_buffer, sizeof(test_buffer), post_data, 5);
        sendRequest(&sensorVal);
    }
}

void loop() {
  // put your main code here, to run repeatedly:
}

void sendRequest(float * sensorVal)
{
    char rxBuff[2048];
    std::stringstream ss;
    ss << *sensorVal;
    std::string temp = HTTP_QUERY + ss.str();
    const char * query = temp.c_str();
    uint32_t i = http1.httpGet(HTTP_HOST, HTTP_PORT, query , rxBuff, sizeof(rxBuff));
    DEBUG_STREAM.print("Read bytes: ");
    DEBUG_STREAM.println(i);

    if (i > 0) {
        rxBuff[i] = 0;
        DEBUG_STREAM.println("Buffer:");
        DEBUG_STREAM.println(rxBuff);
    }
}

#endif