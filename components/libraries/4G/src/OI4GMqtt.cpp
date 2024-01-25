#include "OI4GMqtt.h"


#ifdef DEBUG_MQTT
#warning "OI4GMqtt Debug mode is ON"
#define debugPrint(...)   { if (_diagStream) _diagStream->print(__VA_ARGS__); }
#define debugPrintln(...) { if (_diagStream) _diagStream->println(__VA_ARGS__); }
#else
#define debugPrint(...)
#define debugPrintln(...)
#endif


OI4GMqtt::OI4GMqtt() {
    _mqttPendingMessages = -1;
}

void OI4GMqtt::begin(OI4GNode* client) {

    _client=client;
    _diagStream=_client->getDiagStream();

}

void OI4GMqtt::getSuccessStatus() {
    char urcFlag=_client->getUrcFlag();
    //debugPrintln(urcFlag);
    if(urcFlag== MQTT_LOG_OK) {
        _mqttLoginResult=0;
    }
    else if (urcFlag == MQTT_LOG_ERROR) {
        _mqttLoginResult= 1;
    }

    else if(urcFlag == MQTT_SUB_OK) {
        _mqttSubscribeReason = 0;
    }

    else if(urcFlag == MQTT_SUB_ERROR) {
         _mqttSubscribeReason = 1;
    }

    else {
       _mqttLoginResult= -1;
    }

}

int8_t OI4GMqtt::mqttGetLoginResult()
{
    return _mqttLoginResult;
}

int16_t OI4GMqtt::mqttGetPendingMessages()
{
    return _mqttPendingMessages;
}

bool OI4GMqtt::mqttLogin(uint32_t timeout)
{
    char buffer[16];

    _mqttLoginResult = -1;

    _client->modemPrintln("AT+UMQTTC=1");

        uint32_t startTime = millis();
        
    if ((_client->readResponse(buffer, sizeof(buffer), "+UMQTTC: ", timeout) != OI4GResponseOK) || !_client->startsWith("1,1", buffer)) {
        return false;
    }
    // check URC synchronously
    while ((_mqttLoginResult == -1) && !is_timedout(startTime, timeout)) {
        mqttLoop();
        getSuccessStatus();
    }
    debugPrintln(_mqttLoginResult);
    return (_mqttLoginResult == 0);
}

bool OI4GMqtt::mqttLogout()
{
    char buffer[16];
    _mqttLoginResult = -1;

    _client->modemPrintln("AT+UMQTTC=0");

    return (_client->readResponse(buffer, sizeof(buffer), "+UMQTTC: ", UMQTT_TIMEOUT) == OI4GResponseOK) && _client->startsWith("0,1", buffer);
}

void OI4GMqtt::mqttLoop()
{
    Stream * _modemStream = _client->getModemStream();
    //sodaq_wdt_reset();
    if (!_modemStream->available()) {
        return;
    }

    int count = _client->readLn(_client->getInputBuffer(), _client->getInputBufferSize(), 250); // 250ms, how many bytes at which baudrate?
    //sodaq_wdt_reset();

    if (count <= 0) {
        return;
    }

    debugPrint("<< ");
    debugPrintln(_client->getInputBuffer());

    _client->checkURC(_client->getInputBuffer());
    
}

bool OI4GMqtt::mqttPing(const char* server)
{
    char buffer[16];

    _client->modemPrint("AT+UMQTTC=8,\"");
    _client->modemPrint(server);
    _client->modemPrintln('"');

    return (_client->readResponse(buffer, sizeof(buffer), "+UMQTTC: ", UMQTT_TIMEOUT) == OI4GResponseOK) && _client->startsWith("8,1", buffer);
}

bool OI4GMqtt::mqttPublish(const char* topic, const char* msg, size_t size, uint8_t qos, uint8_t retain, bool useHEX)
{
    char buffer[16];

    _client->modemPrint("AT+UMQTTC=2,");
    _client->modemPrint(qos);
    _client->modemPrint(',');
    _client->modemPrint(retain);
    if (useHEX) {
        _client->modemPrint(",1");
    }
    _client->modemPrint(",\"");
    _client->modemPrint(topic);
    _client->modemPrint("\",\"");

    for (size_t i = 0; i < size; ++i) {
        if (useHEX) {
            _client->modemPrint(static_cast<char>(NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(msg[i]))));
            _client->modemPrint(static_cast<char>(NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(msg[i]))));
        }
        else {
            _client->modemPrint((char)msg[i]);
        }
    }

    _client->modemPrintln('"');

    return (_client->readResponse(buffer, sizeof(buffer), "+UMQTTC: ", UMQTT_TIMEOUT) == OI4GResponseOK) && _client->startsWith("2,1", buffer);
}

// returns number of read messages
uint16_t OI4GMqtt::mqttReadMessages(char* buffer, size_t size, uint32_t timeout)
{
    if (buffer == NULL || size == 0) {
        return 0;
    }

    char bufferIn[16];

    _client->modemPrintln("AT+UMQTTC=6");

    uint32_t startTime = millis();

    if ((_client->readResponse(bufferIn, sizeof(bufferIn), "+UMQTTC: ", UMQTT_TIMEOUT) != OI4GResponseOK) || !_client->startsWith("6,1", bufferIn)) {
        return 0;
    }

    // check URC synchronously
    while ((_mqttPendingMessages == -1) && !is_timedout(startTime, timeout)) {
        mqttLoop();
        _mqttPendingMessages=_client->_pendingMessages;
    }

    if (_mqttPendingMessages <= 0) {
        return 0;
    }

    uint16_t messages = 0;
    uint16_t outSize  = 0;
    char* topicStart = 0;
    char* messageStart = 0;

    while (messages < _mqttPendingMessages && !is_timedout(startTime, timeout)) {
        int count = _client->readLn(_client->getInputBuffer(), _client->getInputBufferSize(), 250);
        //sodaq_wdt_reset();

        if (count <= 0) {
            continue;
        }

        bool isTopic = _client->startsWith("Topic:", _client->getInputBuffer());
        bool isMessage = _client->startsWith("Msg:", _client->getInputBuffer());

        if (isTopic || isMessage) {
            // init publish msg vars when seeing a topic (topic comes first)
            if (isTopic) {
                topicStart = 0;
                messageStart = 0;

                outSize = 0;
            }

            if (outSize >= size - 1) {
                break;
            }

            uint8_t headerSize = isMessage ? strlen("Msg:") : strlen("Topic:");
            count -= headerSize;
            if (isTopic) {
                count--; // remove the LF at the end
            }

            if (outSize + (uint16_t)count > size - 1) {
                count = size - 1 - outSize;
            }

            memcpy(buffer + outSize, _client->getInputBuffer() + headerSize, count);
            if (isTopic) {
                topicStart = buffer + outSize;
            }
            else { // isMessage
                messageStart = buffer + outSize;
            }

            outSize += count;
            buffer[outSize++] = 0;

            // if there is already a full set of topic+message read, call the handler
            if ((topicStart != 0) && (messageStart != 0)) {
                messages++;

                if (_mqttPublishHandler) {
                    _mqttPublishHandler(topicStart, messageStart);
                }
            }

        }
    }

    _mqttPendingMessages -= messages;
    return messages;
}

bool OI4GMqtt::mqttSetAuth(const char* name, const char* pw)
{
    char buffer[16];

    _client->modemPrint("AT+UMQTT=4,\"");
    _client->modemPrint(name);
    _client->modemPrint("\",\"");
    _client->modemPrint(pw);
    _client->modemPrintln('"');

    return (_client->readResponse(buffer, sizeof(buffer), "+UMQTT: ", UMQTT_TIMEOUT) == OI4GResponseOK) && _client->startsWith("4,1", buffer);
}

bool OI4GMqtt::mqttSetCleanSession(bool enabled)
{
    char buffer[16];

    _client->modemPrint("AT+UMQTT=12,");
    _client->modemPrintln(enabled ? '1' : '0');

    return (_client->readResponse(buffer, sizeof(buffer), "+UMQTT: ", UMQTT_TIMEOUT) == OI4GResponseOK) && _client->startsWith("12,1", buffer);
}

bool OI4GMqtt::mqttSetClientId(const char* id)
{
    char buffer[16];

    _client->modemPrint("AT+UMQTT=0,\"");
    _client->modemPrint(id);
    _client->modemPrintln('"');

    return (_client->readResponse(buffer, sizeof(buffer), "+UMQTT: ", UMQTT_TIMEOUT) == OI4GResponseOK) && _client->startsWith("0,1", buffer);
}

bool OI4GMqtt::mqttSetInactivityTimeout(uint16_t timeout)
{
    char buffer[16];

    _client->modemPrint("AT+UMQTT=10,");
    _client->modemPrintln(timeout);

    return (_client->readResponse(buffer, sizeof(buffer), "+UMQTT: ", UMQTT_TIMEOUT) == OI4GResponseOK) && _client->startsWith("10,1", buffer);
}

bool OI4GMqtt::mqttSetLocalPort(uint16_t port)
{
    char buffer[16];

    _client->modemPrint("AT+UMQTT=1,");
    _client->modemPrintln(port);

    return (_client->readResponse(buffer, sizeof(buffer), "+UMQTT: ", UMQTT_TIMEOUT) == OI4GResponseOK) && _client->startsWith("1,1", buffer);
}

bool OI4GMqtt::mqttSetSecureOption(bool enabled, int8_t profile)
{
    char buffer[16];

    _client->modemPrint("AT+UMQTT=11,");
    _client->modemPrint(enabled ? '1' : '0');

    if (profile >= 0) {
        _client->modemPrint(',');
        _client->modemPrintln(profile);
    }

    return (_client->readResponse(buffer, sizeof(buffer), "+UMQTT: ", UMQTT_TIMEOUT) == OI4GResponseOK) && _client->startsWith("11,1", buffer);
}

bool OI4GMqtt::mqttSetServer(const char* server, uint16_t port)
{
    char buffer[16];

    _client->modemPrint("AT+UMQTT=2,\"");
    _client->modemPrint(server);

    if (port > 0) {
        _client->modemPrint("\",");
        _client->modemPrintln(port);
    }
    else {
        _client->modemPrintln('"');
    }

  return (_client->readResponse(buffer, sizeof(buffer), "+UMQTT: ", UMQTT_TIMEOUT) == OI4GResponseOK) && _client->startsWith("2,1", buffer);
}

bool OI4GMqtt::mqttSetServerIP(const char* ip, uint16_t port)
{
    char buffer[16];

    _client->modemPrint("AT+UMQTT=3,\"");
    _client->modemPrint(ip);

    if (port > 0) {
        _client->modemPrint("\",");
        _client->modemPrintln(port);
    }
    else {
        _client->modemPrintln('"');
    }

    return (_client->readResponse(buffer, sizeof(buffer), "+UMQTT: ", UMQTT_TIMEOUT) == OI4GResponseOK) && _client->startsWith("3,1", buffer);
}

bool OI4GMqtt::mqttSubscribe(const char* filter, uint8_t qos, uint32_t timeout)
{
    char buffer[16];
    
    _mqttSubscribeReason = -1;

    _client->modemPrint("AT+UMQTTC=4,");
    _client->modemPrint(qos);
    _client->modemPrint(",\"");
    _client->modemPrint(filter);
    _client->modemPrintln('"');

    uint32_t startTime = millis();

    if ((_client->readResponse(buffer, sizeof(buffer), "+UMQTTC: ", UMQTT_TIMEOUT) != OI4GResponseOK) || !_client->startsWith("4,1", buffer)) {
        return false;
    }

    // check URC synchronously
    while ((_mqttSubscribeReason == -1) && !is_timedout(startTime, timeout)) {
        mqttLoop();
        getSuccessStatus();
    }

    return (_mqttSubscribeReason == 0);
}

bool OI4GMqtt::mqttUnsubscribe(const char* filter)
{
    char buffer[16];

    _client->modemPrint("AT+UMQTTC=5,\"");
    _client->modemPrint(filter);
    _client->modemPrintln('"');

    return (_client->readResponse(buffer, sizeof(buffer), "+UMQTTC: ", UMQTT_TIMEOUT) == OI4GResponseOK) && _client->startsWith("5,1", buffer);
}

void OI4GMqtt::mqttSetPublishHandler(PublishHandlerPtr handler)
{
    if (handler) {
        _mqttPublishHandler = handler;
    }
}