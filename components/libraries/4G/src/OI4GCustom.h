#include "OI4GNode.h"



#define SOCKET_CLOSE_TIMEOUT       120000
#define SOCKET_CONNECT_TIMEOUT     120000
#define SOCKET_WRITE_TIMEOUT       120000
#define SOCKET_FAIL -1


enum Protocols {
    TCP = 0,
    UDP
};

class OI4GCustom 
{

public:

    OI4GCustom();
    void begin(OI4GNode* client);

    int    socketCreate(uint16_t localPort = 0, Protocols protocol = UDP);

    bool   socketSetOption(uint8_t socketID, uint16_t level, uint16_t optName, uint32_t optValue, uint32_t optValue2 = 0);
    bool   socketResolveIp(const char* server, char* buffer);
    // Required for TCP, optional for UDP (for UDP socketConnect() + socketWrite() == socketSend())
    bool   socketConnect(uint8_t socketID, const char* remoteHost, const uint16_t remotePort);
    size_t socketWrite(uint8_t socketID, const uint8_t* buffer, size_t size);

    // TCP only
    bool   socketWaitForRead(uint8_t socketID, uint32_t timeout = OI4G_DEFAULT_READ_TIMOUT);
    size_t socketRead(uint8_t socketID, uint8_t* buffer, size_t length);

    // UDP only
    size_t socketSend(uint8_t socketID, const char* remoteHost, const uint16_t remotePort, const uint8_t* buffer, size_t size);
    bool   socketWaitForReceive(uint8_t socketID, uint32_t timeout = OI4G_DEFAULT_READ_TIMOUT);
    size_t socketReceive(uint8_t socketID, uint8_t* buffer, size_t length);

    bool   socketClose(uint8_t socketID, bool async = false);
    int    socketCloseAll();
    bool   socketFlush(uint8_t socketID, uint32_t timeout = 10000);
    bool   socketIsClosed(uint8_t socketID);
    bool   socketWaitForClose(uint8_t socketID, uint32_t timeout);

    size_t socketGetPendingBytes(uint8_t socketID);
    bool   socketHasPendingBytes(uint8_t socketID);


private:

    OI4GNode* _client; 
    Stream* _diagStream;


};
