#include "OI4GNode.h"

#define UMQTT_TIMEOUT              60000


typedef void(*PublishHandlerPtr)(const char* topic, const char* msg);

class OI4GMqtt {

public:

    OI4GMqtt();
    void begin(OI4GNode* client);
    void getSuccessStatus();

    int8_t  mqttGetLoginResult();
    int16_t mqttGetPendingMessages();

    bool mqttLogin(uint32_t timeout = 3 * 60 * 1000);
    bool mqttLogout();
    void mqttLoop();
    bool mqttPing(const char* server);
    bool mqttPublish(const char* topic, const char* msg, size_t size, uint8_t qos = 0, uint8_t retain = 0, bool useHEX = false);
    uint16_t mqttReadMessages(char* buffer, size_t size, uint32_t timeout = 60 * 1000);

    bool mqttSetAuth(const char* name, const char* pw);
    bool mqttSetCleanSession(bool enabled);
    bool mqttSetClientId(const char* id);
    bool mqttSetInactivityTimeout(uint16_t timeout);
    bool mqttSetLocalPort(uint16_t port);
    bool mqttSetSecureOption(bool enabled, int8_t profile = -1);
    bool mqttSetServer(const char* server, uint16_t port);
    bool mqttSetServerIP(const char* ip, uint16_t port);

    bool mqttSubscribe(const char* filter, uint8_t qos = 0, uint32_t timeout = 30 * 1000);
    bool mqttUnsubscribe(const char* filter);
    void mqttSetPublishHandler(PublishHandlerPtr handler);

private:

    int8_t    _mqttLoginResult;
    int16_t   _mqttPendingMessages;
    int8_t    _mqttSubscribeReason;
    Stream* _diagStream;
    OI4GNode* _client;
    PublishHandlerPtr _mqttPublishHandler = NULL;


};