#include "OI4GNode.h"


#define HTTP_RECEIVE_FILENAME  "http_last_response_0"
#define HTTP_SEND_TMP_FILENAME "http_tmp_put_0"


enum HttpRequestTypes {
    POST = 0,
    GET,
    HEAD,
    DELETE,
    PUT,
    HttpRequestTypesMAX
};


enum States
{
    False,
    True,
    Undefined
};

typedef States states_t;

class OI4GHttp {

public: 
OI4GHttp();
void begin(OI4GNode* client);

// Creates an HTTP GET request and optionally returns the received data.
uint32_t httpGet(const char* server, uint16_t port, const char* endpoint,
                    char* buffer, size_t bufferSize, uint32_t timeout = 60000, bool useURC = true);

// Determine HTTP header size
uint32_t httpGetHeaderSize(const char* filename);

// Return a partial result of the previous HTTP Request (GET or POST)
// Offset 0 is the byte directly after the HTTP Response header
size_t httpGetPartial(uint8_t* buffer, size_t size, uint32_t offset);

// Creates an HTTP POST request and optionally returns the received data.
// Note. Endpoint should include the initial "/".
// The UBlox device stores the received data in http_last_response_<profile_id>
uint32_t httpPost(const char* server, uint16_t port, const char* endpoint,
                    char* responseBuffer, size_t responseSize,
                    const char* sendBuffer, size_t sendSize, uint32_t timeout = 60000, bool useURC = true);

// Creates an HTTP POST request and optionally returns the received data.
// Note. Endpoint should include the initial "/".
// The UBlox device stores the received data in http_last_response_<profile_id>
// Request body must first be prepared in a file on the modem
uint32_t httpPostFromFile(const char* server, uint16_t port, const char* endpoint,
                    char* responseBuffer, size_t responseSize,
                    const char* fileName, uint32_t timeout = 60000, bool useURC = true);

// Creates an HTTP request using the (optional) given buffer and
// (optionally) returns the received data.
// endpoint should include the initial "/".
size_t httpRequest(const char* server, uint16_t port, const char* endpoint,
                    HttpRequestTypes requestType = HttpRequestTypes::GET,
                    char* responseBuffer = NULL, size_t responseSize = 0,
                    const char* sendBuffer = NULL, size_t sendSize = 0, uint32_t timeout = 60000, bool useURC = true);

    // Creates an HTTP request using the (optional) given buffer and
    // (optionally) returns the received data.
    // endpoint should include the initial "/".
    // Request body must first be prepared in a file on the modem
    // Can only be used for POST and PUT requests
    size_t httpRequestFromFile(const char* server, uint16_t port, const char* endpoint,
                    HttpRequestTypes requestType = HttpRequestTypes::GET,
                    char* responseBuffer = NULL, size_t responseSize = 0,
                    const char* fileName = NULL, uint32_t timeout = 60000, bool useURC = true);

//  Paremeter index has a range [0-4]
//  Parameters 'name' and 'value' can have a maximum length of 64 characters
//  Parameters 'name' and 'value' must not include the ':' character
bool httpSetCustomHeader(uint8_t index, const char* name, const char* value);
bool httpClearCustomHeader(uint8_t index);


private: 

OI4GNode* _client; 
Stream* _diagStream;
uint32_t  _httpGetHeaderSize;
states_t _httpRequestSuccessBit[HttpRequestTypesMAX];

void getSuccessStatus(HttpRequestTypes requestType);

};