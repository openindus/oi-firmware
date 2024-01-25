#include "OI4GCustom.h"


#ifdef DEBUG_CUSTOM
#warning "OI4GCustom Debug mode is ON"
#define debugPrint(...)   { if (_diagStream) _diagStream->print(__VA_ARGS__); }
#define debugPrintln(...) { if (_diagStream) _diagStream->println(__VA_ARGS__); }
#else
#define debugPrint(...)
#define debugPrintln(...)
#endif

OI4GCustom::OI4GCustom() {

}

void OI4GCustom::begin(OI4GNode* client) {

    _client= client;
    _diagStream = _client->getDiagStream();
    //debugPrintln("hello i am debug in custom");
}

bool OI4GCustom::socketClose(uint8_t socketID, bool async)
{
    socketFlush(socketID);
    
    _client->modemPrint("AT+USOCL=");
    _client->modemPrint(socketID);

    if (async) {
        _client->modemPrintln(",1");
    }
    else {
        _client->modemPrintln();
    }

    _client->_socketClosedBit[socketID] = true;
    _client->_socketPendingBytes[socketID] = 0;

    if (_client->readResponse(NULL, 0, NULL, SOCKET_CLOSE_TIMEOUT) != OI4GResponseOK) {
        return false;
    }

    return true;
}

int OI4GCustom::socketCloseAll() {

    int closedCount = 0;

    for (uint8_t i = 0; i < SOCKET_COUNT; i++) {
        if (socketClose(i, false)) {
            closedCount++;
        }
    }

    // return the number of sockets we closed
    return closedCount;
}

bool OI4GCustom::socketConnect(uint8_t socketID, const char* remoteHost, const uint16_t remotePort)
{
    _client->modemPrint("AT+USOCO=");
    _client->modemPrint(socketID);
    _client->modemPrint(",\"");
    _client->modemPrint(remoteHost);
    _client->modemPrint("\",");
    _client->modemPrintln(remotePort);

    bool b = _client->readResponse(NULL, 0, NULL, SOCKET_CONNECT_TIMEOUT) == OI4GResponseOK;

    _client->_socketClosedBit  [socketID] = !b;

    return b;
}

bool OI4GCustom::socketResolveIp(const char* server, char* buffer) {
    _client->resolveIp(server, buffer);
}

int OI4GCustom::socketCreate(uint16_t localPort, Protocols protocol)
{
    _client->modemPrint("AT+USOCR=");
    _client->modemPrint(protocol == UDP ? "17" : "6");

    if (localPort > 0) {
        _client->modemPrint(',');
        _client->modemPrintln(localPort);
    }
    else {
        _client->modemPrintln();
    }

    char buffer[32];

    if (_client->readResponse(buffer, sizeof(buffer), "+USOCR: ") != OI4GResponseOK) {
        return SOCKET_FAIL;
    }

    int socketID;

    if ((sscanf(buffer, "%d", &socketID) != 1) || (socketID < 0) || (socketID > SOCKET_COUNT)) {
        return SOCKET_FAIL;
    }

    _client->_socketClosedBit   [socketID] = true;
    _client->_socketPendingBytes[socketID] = 0;

    return socketID;
}

bool OI4GCustom::socketFlush(uint8_t socketID, uint32_t timeout)
{
    uint32_t start = millis();

    while (_client->isAlive() && (!is_timedout(start, timeout)) && (!_client->_socketClosedBit[socketID])) {
        _client->modemPrint("AT+USOCTL=");
        _client->modemPrint(socketID);
        _client->modemPrintln(",11");
        
        char buffer[32];

        if (_client->readResponse(buffer, sizeof(buffer), "+USOCTL: ") != OI4GResponseOK) {
            if (_client->_socketClosedBit[socketID]) {
                return true;
            }
            else {
                return false;
            }
        }
        
        int pendingBytes = 0;
        if (sscanf(buffer, "%*d,11,%d", &pendingBytes) != 1) {
            return false;
        }
        else if (pendingBytes == 0){
            return true;
        }
        
        _client->wdtSafeDelay(300);
    }
    
    if (_client->_socketClosedBit[socketID]) {
        return true;
    }

    return false;
}

size_t OI4GCustom::socketGetPendingBytes(uint8_t socketID)
{
    return _client->_socketPendingBytes[socketID];
}

bool OI4GCustom::socketHasPendingBytes(uint8_t socketID)
{
    return socketGetPendingBytes(socketID) > 0;
}

bool OI4GCustom::socketIsClosed(uint8_t socketID)
{
    return _client->_socketClosedBit[socketID];
}

size_t OI4GCustom::socketRead(uint8_t socketID, uint8_t* buffer, size_t size)
{
    if (!socketHasPendingBytes(socketID)) {
        // no URC has happened, no socket to read
        debugPrintln("Reading from without available bytes!");
        return 0;
    }

    _client->execCommand("AT+UDCONF=1,1");

    size = _min(size, _min(OI4G_MAX_SOCKET_BUFFER, _client->_socketPendingBytes[socketID]));

    char   outBuffer[OI4G_MAX_SOCKET_BUFFER];
    int    retSocketID;
    size_t retSize;

    _client->modemPrint("AT+USORD=");
    _client->modemPrint(socketID);
    _client->modemPrint(',');
    _client->modemPrintln(size);

    if (_client->readResponse(outBuffer, sizeof(outBuffer), "+USORD: ") != OI4GResponseOK) {
        return 0;
    }

    if (sscanf(outBuffer, "%d,%d,\"%[^\"]\"", &retSocketID, &retSize, outBuffer) != 3) {
        return 0;
    }

    if ((retSocketID < 0) || (retSocketID >= SOCKET_COUNT)) {
        return 0;
    }

    _client->_socketPendingBytes[socketID] -= retSize;

    if (buffer != NULL && size > 0) {
        for (size_t i = 0; i < retSize * 2; i += 2) {
            buffer[i / 2] = HEX_PAIR_TO_BYTE(outBuffer[i], outBuffer[i + 1]);
        }
    }

    return retSize;
}

size_t OI4GCustom::socketReceive(uint8_t socketID, uint8_t* buffer, size_t size)
{
    if (!socketHasPendingBytes(socketID)) {
        // no URC has happened, no socket to read
        debugPrintln("Reading from without available bytes!");
        return 0;
    }

    _client->execCommand("AT+UDCONF=1,1");

    size = _min(size, _min(OI4G_MAX_SOCKET_BUFFER, _client->_socketPendingBytes[socketID]));

    char   outBuffer[OI4G_MAX_SOCKET_BUFFER];
    int    retSocketID;
    size_t retSize;

    _client->modemPrint("AT+USORF=");
    _client->modemPrint(socketID);
    _client->modemPrint(',');
    _client->modemPrintln(size);

    if (_client->readResponse(outBuffer, sizeof(outBuffer), "+USORF: ") != OI4GResponseOK) {
        return 0;
    }

    if (sscanf(outBuffer, "%d,\"%*[^\"]\",%*d,%d,\"%[^\"]\"", &retSocketID, &retSize, outBuffer) != 3) {
        return 0;
    }

    if ((retSocketID < 0) || (retSocketID >= SOCKET_COUNT)) {
        return 0;
    }

    _client->_socketPendingBytes[socketID] -= retSize;

    if (buffer != NULL && size > 0) {
        for (size_t i = 0; i < retSize * 2; i += 2) {
            buffer[i / 2] = HEX_PAIR_TO_BYTE(outBuffer[i], outBuffer[i + 1]);
        }
    }

    return retSize;
}

size_t OI4GCustom::socketSend(uint8_t socketID, const char* remoteHost, const uint16_t remotePort,
                             const uint8_t* buffer, size_t size)
{
    if (size > MAX_SEND_MESSAGE_SIZE) {
        debugPrintln("Message exceeded maximum size!");
        return 0;
    }

    _client->execCommand("AT+UDCONF=1,1");

    _client->modemPrint("AT+USOST=");
    _client->modemPrint(socketID);
    _client->modemPrint(",\"");
    _client->modemPrint(remoteHost);
    _client->modemPrint("\",");
    _client->modemPrint(remotePort);
    _client->modemPrint(',');
    _client->modemPrint(size);
    _client->modemPrint(",\"");

    for (size_t i = 0; i < size; ++i) {
        _client->modemPrint(static_cast<char>(NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(buffer[i]))));
        _client->modemPrint(static_cast<char>(NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(buffer[i]))));
    }

    _client->modemPrintln('"');

    char outBuffer[64];

    if (_client->readResponse(outBuffer, sizeof(outBuffer), "+USOST: ", SOCKET_WRITE_TIMEOUT) != OI4GResponseOK) {
        return 0;
    }

    int retSocketID;
    int sentLength;

    if ((sscanf(outBuffer, "%d,%d", &retSocketID, &sentLength) != 2) || (retSocketID < 0) || (retSocketID > SOCKET_COUNT)) {
        return 0;
    }

    return sentLength;
}

bool OI4GCustom::socketSetOption(uint8_t socketID, uint16_t level, uint16_t optName, uint32_t optValue, uint32_t optValue2)
{
    _client->modemPrint("AT+USOSO=");
    _client->modemPrint(socketID);
    _client->modemPrint(',');
    _client->modemPrint(level);
    _client->modemPrint(',');
    _client->modemPrint(optName);
    _client->modemPrint(',');
    _client->modemPrint(optValue);

    if (optValue2 > 0) {
        _client->modemPrint(',');
        _client->modemPrintln(optValue2);
    }
    else {
        _client->modemPrintln();
    }

    return (_client->readResponse() == OI4GResponseOK);
}

bool OI4GCustom::socketWaitForClose(uint8_t socketID, uint32_t timeout)
{
    uint32_t startTime = millis();

    while (_client->isAlive() && !socketIsClosed(socketID) && !is_timedout(startTime, timeout)) {
        _client->wdtSafeDelay(10);
    }

    return socketIsClosed(socketID);
}

bool OI4GCustom::socketWaitForRead(uint8_t socketID, uint32_t timeout)
{
    if (socketHasPendingBytes(socketID)) {
        return true;
    }

    uint32_t startTime = millis();

    while (!socketHasPendingBytes(socketID) && !socketIsClosed(socketID) && !is_timedout(startTime, timeout)) {
        _client->modemPrint("AT+USORD=");
        _client->modemPrint(socketID);
        _client->modemPrintln(",0");

        char buffer[128];

        if (_client->readResponse(buffer, sizeof(buffer), "+USORD: ") == OI4GResponseOK) {
            int retSocketID;
            int receiveSize;

            if (sscanf(buffer, "%d,%d", &retSocketID, &receiveSize) == 2) {
                _client->_socketPendingBytes[retSocketID] = receiveSize;
            }
        }

        _client->wdtSafeDelay(10);
    }

    return socketHasPendingBytes(socketID);
}

bool OI4GCustom::socketWaitForReceive(uint8_t socketID, uint32_t timeout)
{
    if (socketHasPendingBytes(socketID)) {
        return true;
    }

    uint32_t startTime = millis();

    while (!socketHasPendingBytes(socketID) && !is_timedout(startTime, timeout)) {
        _client->modemPrint("AT+USORF=");
        _client->modemPrint(socketID);
        _client->modemPrintln(",0");

        char buffer[128];

        if (_client->readResponse(buffer, sizeof(buffer), "+USORF: ") == OI4GResponseOK) {
            int retSocketID;
            int receiveSize;

            if (sscanf(buffer, "%d,%d", &retSocketID, &receiveSize) == 2) {
                _client->_socketPendingBytes[retSocketID] = receiveSize;
            }
        }

        _client->wdtSafeDelay(250);
    }

    return socketHasPendingBytes(socketID);
}

size_t OI4GCustom::socketWrite(uint8_t socketID, const uint8_t* buffer, size_t size)
{
    _client->execCommand("AT+UDCONF=1,1");

    _client->modemPrint("AT+USOWR=");
    _client->modemPrint(socketID);
    _client->modemPrint(",");
    _client->modemPrintln(size);

    if (_client->readResponse() != OI4GResponsePrompt) {
        return 0;
    }

    // After the @ prompt reception, wait for a minimum of 50 ms before sending data.
    delay(51);

    for (size_t i = 0; i < size; i++) {
        _client->modemPrint(static_cast<char>(NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(buffer[i]))));
        _client->modemPrint(static_cast<char>(NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(buffer[i]))));
    }

    debugPrintln();

    char outBuffer[64];

    if (_client->readResponse(outBuffer, sizeof(outBuffer), "+USOWR: ", SOCKET_WRITE_TIMEOUT) != OI4GResponseOK) {
        return 0;
    }

    int retSocketID;
    int sentLength;

    if ((sscanf(outBuffer, "%d,%d", &retSocketID, &sentLength) != 2) || (retSocketID < 0) || (retSocketID > SOCKET_COUNT)) {
        return 0;
    }

    return sentLength;
}
