#pragma once
#include "Cloud.hpp"

#if defined(OI_CORE)
    #include "ModuleStandalone.h"
    #define CLOUD_CONNECTED() ModuleStandalone::ledOn(LED_GREEN)
    #define CLOUD_OTA() ModuleStandalone::ledOn(LED_BLUE)
    #define CLOUD_AUTHENTICATING() ModuleStandalone::ledBlink(LED_GREEN, 500)
    #define CLOUD_DISCONNECTED() MODULE_PAIRED()
#endif

#define Cloud_LogTag "Cloud"

//General MQTT Config
#define Mqtt_Host CONFIG_CLOUD_MQTT_HOST                                      // MQTT Broker HOST
#define Mqtt_Default_Port CONFIG_CLOUD_MQTT_DEFAULT_PORT                      // MQTT Broker Port without TLS
#define Mqtt_QOS CONFIG_CLOUD_MQTT_QOS                                        // MQTT Quality of Service
#define Mqtt_Timeout CONFIG_CLOUD_MQTT_TIMEOUT                                // MQTT Disconnect timeout
#define Mqtt_Topic_Send_Suffix CONFIG_CLOUD_MQTT_TOPIC_SEND_SUFFIX            // Topic Suffix for sending values
#define Mqtt_Topic_Receive_Suffix CONFIG_CLOUD_MQTT_TOPIC_RECEIVE_SUFFIX      // Topic Suffix for received values
#define Mqtt_Last_Will_Topic_Suffix CONFIG_CLOUD_MQTT_LAST_WILL_TOPIC_SUFFIX  // Topic Suffix for MQTT LWT
#define Mqtt_Last_Will_Msg "0"                                                // Disconnect payload
#define Mqtt_Max_Buffer_Size 500                                              // Max buffer size for received messages

//MQTTS Config

// Enable MQTT over TLS
#ifdef CONFIG_CLOUD_MQTT_USE_TLS
#define Mqtt_Use_TLS true
#else
#define Mqtt_Use_TLS false
#endif

// Skip server certificate name validation
#ifdef CONFIG_CLOUD_MQTTS_ENABLE_CHECK
#define Mqtt_Skip_Name_Check true
#else
#define Mqtt_Skip_Name_Check false
#endif

#define Mqtt_TLS_Port CONFIG_CLOUD_MQTT_TLS_PORT      // MQTT Broker Port with TLS

//HTTP Config for authentication
#define HTTP_Host CONFIG_CLOUD_HTTP_HOST              // Authentication server host ex: "192.168.2.34"
#define HTTP_Port CONFIG_CLOUD_HTTP_PORT              // Authentication server port ex: 8000

// Enable HTTPS
#ifdef CONFIG_CLOUD_HTTP_TLS
#define HTTP_Use_TLS true
#else
#define HTTP_Use_TLS false
#endif

// Skip server certificate name validation
#ifdef CONFIG_CLOUD_HTTPS_ENABLE_CHECK
#define HTTP_Skip_Name_Check true
#else
#define HTTP_Skip_Name_Check false
#endif


#define HTTP_Token_Refresh_Route CONFIG_CLOUD_TOKEN_REFRESH_ROUTE     // Refresh route, used to refresh the access token
#define HTTP_Token_Check_Route CONFIG_CLOUD_TOKEN_CHECK_ROUTE         // Authentication route, used to check if the access token is valid
#define HTTP_Broker_Init_Route CONFIG_CLOUD_SETUP_ROUTE               // Setup route
#define HTTP_BUFFER_SIZE 3000                                         // Maximum HTTP response buffer size
#define HTTP_Token_Validity CONFIG_CLOUD_TOKEN_VALIDITY_PERIOD        // Access token expiration delay

//Security Key Config
#define _Token_BUFFER_SIZE 256                    // Maximum token buffer size
#define _Access_Token_Key "Cloud_A_token"         // NVS access token key name
#define _Refresh_Token_Key "Cloud_R_token"        // NVS refresh token key name
#define _UUID_Key "Cloud_UUID"                    // NVS UUID key name

//Authentication
#define Cloud_Auth_timeout CONFIG_CLOUD_AUTHENTICATION_TIMEOUT // Authentication timeout (used to trigger authentication if the mqtt client is disconnected)
#define Cloud_Auth_retry_delay CONFIG_CLOUD_RETRY_DELAY        // Authentication retry delay

//OTA Firmware update config
#define Cloud_OTA_topic "/OTA"                    // Topic Suffix for ota versions info
#define Cloud_OTA_route "/en/get-ota/"            // OTA Get firmware route
#define Cloud_OTA_status_key "OTAState"           // OTA Status save key for NVS

#define Mqtt_Port (Mqtt_Use_TLS == true) ? Mqtt_TLS_Port : Mqtt_Default_Port
#define Mqtt_URI (Mqtt_Use_TLS == true) ? "mqtts://" Mqtt_Host : "mqtt://" Mqtt_Host

char *http_get_bearer(const char *path, const char *token,
                    unsigned short *status_code);
char *getToken(const char *uuid);
char *getBrokerCertificate(const char *access_token);
int saveNVS(const char *key, const char *value);
char *getNVS(const char *key);
void task_authenticate(void *data);
void updateSytemTimeNTP(void *data);
