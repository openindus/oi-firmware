//#define UNIT_TEST
#ifdef UNIT_TEST
#include "OI4GNode.h"
#include "OI4GHttp.h"
#include <unity.h>

#define MODEM_STREAM Serial2
#define CURRENT_APN      "TM"
#define CURRENT_OPERATOR AUTOMATIC_OPERATOR
#define CURRENT_URAT     OI4G_LTEM_URAT  
#define CURRENT_MNO_PROFILE MNOProfiles::STANDARD_EUROPE
#define LTEM_BANDMASK BAND_MASK_UNCHANGED

#define HTTP_HOST        "4gdev.openindus.com"
#define HTTP_PORT        80
#define HTTP_QUERY       "/service.php?type=send_data&name=temp1&val1=1"

static OI4GNode node;
static OI4GHttp http1;
char test_buff[2048];

void test_on(void) {
    
    TEST_ASSERT_TRUE(node.on())
}

void test_CCID(void) {
    char buffer[25];
    if (node.getCCID(buffer, sizeof(buffer))) {
        if(sscanf(buffer, "+CCID: %s", test_buff)==1) {
        TEST_ASSERT_EQUAL_INT(20,sizeof(test_buff));
        }   

        else {
            TEST_FAIL_MESSAGE("Buffer empty");
        }
    }
    
    
}

void test_IMSI(void) {
    char buffer[25];
    strcpy(test_buff, "");
    node.getIMSI(buffer, sizeof(buffer));
    
    TEST_ASSERT_NOT_EQUAL("",test_buff);
    
}

void test_operator(void) {
    uint16_t mcc;
    uint16_t mnc;

    node.getOperatorInfo(&mcc, &mnc);
    TEST_ASSERT_EQUAL_INT(208, mcc);


}

void test_operator_string(void) {
    char buffer[24];
    node.getOperatorInfoString(buffer, sizeof(buffer));

    TEST_ASSERT_EQUAL_STRING("", buffer); //TODO : ADD OPERATOR


}

void test_cell_info(void) {

    uint16_t tac=0;
    uint32_t cid=0;
    uint16_t urat;

    node.getCellInfo(&tac, &cid,&urat);

    TEST_ASSERT_TRUE((tac !=0 && cid !=0 && (urat==7 || urat==8)));

}

void test_epoch(void) {
    uint32_t epoch=0;
    node.getEpoch(&epoch);
    TEST_ASSERT_TRUE(epoch!=0);
}

void test_connect(void) {
    static bool isReady;
    isReady = node.connect(CURRENT_APN, CURRENT_URAT, CURRENT_MNO_PROFILE, CURRENT_OPERATOR, BAND_MASK_UNCHANGED, LTEM_BANDMASK);
    TEST_ASSERT_TRUE(isReady);
}

void test_http_get(void) {
    strcpy(test_buff, "");
    http1.begin(&node);
     uint32_t i = http1.httpGet(HTTP_HOST, HTTP_PORT, HTTP_QUERY , test_buff, sizeof(test_buff));
     TEST_ASSERT_GREATER_OR_EQUAL(1, i);
}

void test_http_post(void) {
    const char* post_data = "test";
    strcpy(test_buff, "");
    http1.begin(&node);
     uint32_t i = http1.httpPost("postman-echo.com", HTTP_PORT, "/post" , test_buff, sizeof(test_buff), post_data, strlen(post_data));
     TEST_ASSERT_GREATER_OR_EQUAL(1, i);
}



void setup() {

    delay(2000);
    MODEM_STREAM.begin(node.getBaudrate());
    node.init(MODEM_STREAM);
    
    UNITY_BEGIN();
    RUN_TEST(test_on);
    RUN_TEST(test_IMSI);
    RUN_TEST(test_operator);
    RUN_TEST(test_operator_string);
    RUN_TEST(test_cell_info);
    RUN_TEST(test_epoch);
    RUN_TEST(test_connect);

    RUN_TEST(test_http_post);
    RUN_TEST(test_http_get);
    

    

    UNITY_END();
}

void loop() {

}


#endif