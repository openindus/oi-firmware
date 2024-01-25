
#include "OI4GHttp.h"


#ifdef DEBUG_HTTP
#warning "OI4GHttp Debug mode is ON"
#define debugPrint(...)   { if (_diagStream) _diagStream->print(__VA_ARGS__); }
#define debugPrintln(...) { if (_diagStream) _diagStream->println(__VA_ARGS__); }
#else
#define debugPrint(...)
#define debugPrintln(...)
#endif

static uint8_t httpRequestMapping[] = {
    4, // 0 POST
    1, // 1 GET
    0, // 2 HEAD
    2, // 3 DELETE
    3, // 4 PUT
};

OI4GHttp::OI4GHttp() {

}



void OI4GHttp::begin(OI4GNode* client) {
    _client= client;
    _diagStream = _client->getDiagStream();
    _client->checkRegistration();
    //debugPrintln("hello i am debug in http");
}

void OI4GHttp::getSuccessStatus(HttpRequestTypes requestType) {
    char urcFlag=_client->getUrcFlag();
    if(urcFlag== HTTP_OK) {
        _httpRequestSuccessBit[requestType]= True;
    }
    else if (urcFlag == HTTP_ERROR) {
        _httpRequestSuccessBit[requestType]= False;
    }
    else {
        _httpRequestSuccessBit[requestType]= Undefined;
    }

}

uint32_t OI4GHttp::httpGet(const char* server, uint16_t port, const char* endpoint,
                            char* buffer, size_t bufferSize, uint32_t timeout, bool useURC)
{
    // First just handle the request and let the file be read into the UBlox file system
    uint32_t file_size = httpRequest(server, port, endpoint, GET, NULL, 0, NULL, 0, timeout, useURC);
    if (file_size == 0) {
        return 0;
    }

    // Find out the header size
    _httpGetHeaderSize = httpGetHeaderSize(HTTP_RECEIVE_FILENAME);

     debugPrint("[httpGet] header size: ");
     debugPrintln(_httpGetHeaderSize);

    if (_httpGetHeaderSize == 0) {
        return 0;
    }

    if (!buffer) {
        return file_size - _httpGetHeaderSize;
    }

    // Fill the buffer starting from the header
    return _client->readFilePartial(HTTP_RECEIVE_FILENAME, (uint8_t*)buffer, bufferSize, _httpGetHeaderSize);
}

/**
* Return the size of the HTTP response header
*
* This function searches from the start of the file until two CRLFs have
* been seen.
* The file is left unmodified.
*/
uint32_t OI4GHttp::httpGetHeaderSize(const char* filename)
{
    uint32_t file_size;

    bool status = _client->getFileSize(filename, file_size);
    if (!status) {
        return 0;
    }

    int state = 0; // 0 nothing, 1=CR, 2=CRLF, 3=CRLFCR, 4=CRLFCRLF
    uint8_t buffer[64];
    uint32_t offset = 0;

    while (offset < file_size && state != 4) {
        size_t size;
        size = _client->readFilePartial(filename, buffer, sizeof(buffer), offset);

        if (size == 0) {
            return 0;
        }

        size_t ix;
        for (ix = 0; state != 4 && ix < sizeof(buffer); ix++) {
            if ((state == 0 || state == 2) && buffer[ix] == '\r') {
                state++;
            }
            else if ((state == 1 || state == 3) && buffer[ix] == '\n') {
                state++;
            }
            else {
                state = 0;
            }
        }

        if (state == 4) {
            return offset + ix;
        }

        offset += size;
    }

    return 0;
}

size_t OI4GHttp::httpGetPartial(uint8_t* buffer, size_t size, uint32_t offset)
{
    if (_httpGetHeaderSize == 0) {
        _httpGetHeaderSize = httpGetHeaderSize(HTTP_RECEIVE_FILENAME);
    }

    if (_httpGetHeaderSize == 0) {
        // Don't trust a file without HTTP response header
        return 0;
    }

    return _client->readFilePartial(HTTP_RECEIVE_FILENAME, buffer, size, _httpGetHeaderSize + offset);
}

// Creates an HTTP POST request and optionally returns the received data.
// Note. Endpoint should include the initial "/".
// The UBlox device stores the received data in http_last_response_<profile_id>
uint32_t OI4GHttp::httpPost(const char* server, uint16_t port, const char* endpoint,
                             char* responseBuffer, size_t responseSize,
                             const char* sendBuffer, size_t sendSize, uint32_t timeout, bool useURC)
{
    _client->deleteFile(HTTP_SEND_TMP_FILENAME); // cleanup the file first (if exists)

    if (!_client->writeFile(HTTP_SEND_TMP_FILENAME, (uint8_t*)sendBuffer, sendSize)) {
        debugPrintln(DEBUG_STR_ERROR "Could not create the http tmp file!");
        return 0;
    }

    return httpPostFromFile(server, port, endpoint, responseBuffer, responseSize, HTTP_SEND_TMP_FILENAME, timeout, useURC);
}

// Creates an HTTP POST request and optionally returns the received data.
// Note. Endpoint should include the initial "/".
// The UBlox device stores the received data in http_last_response_<profile_id>
// Request body must first be prepared in a file on the modem
uint32_t OI4GHttp::httpPostFromFile(const char* server, uint16_t port, const char* endpoint,
                                     char* responseBuffer, size_t responseSize,
                                     const char* fileName, uint32_t timeout, bool useURC)
{
    // First just handle the request and let the file be read into the UBlox file system
    uint32_t file_size = httpRequestFromFile(server, port, endpoint, POST, responseBuffer, responseSize, fileName,
                                             timeout, useURC);
    if (file_size == 0) {
        return 0;
    }

    // Find out the header size
    _httpGetHeaderSize = httpGetHeaderSize(HTTP_RECEIVE_FILENAME);

    debugPrint("[httpPost] header size: ");
    debugPrintln(_httpGetHeaderSize);

    if (_httpGetHeaderSize == 0) {
        return 0;
    }

    if (!responseBuffer) {
        return file_size - _httpGetHeaderSize;
    }

    // Fill the buffer starting from the header
    return _client->readFilePartial(HTTP_RECEIVE_FILENAME, (uint8_t*)responseBuffer, responseSize, _httpGetHeaderSize);
}

// Creates an HTTP request using the (optional) given buffer and
// (optionally) returns the received data.
// endpoint should include the initial "/".
size_t OI4GHttp::httpRequest(const char* server, uint16_t port, const char* endpoint, HttpRequestTypes requestType,
                              char* responseBuffer, size_t responseSize,
                              const char* sendBuffer, size_t sendSize, uint32_t timeout, bool useURC)
{
    // before starting the actual http request, create any files needed in the fs of the modem
    // that way there is a chance to abort sending the http req command in case of an fs error
    if (requestType == PUT || requestType == POST) {
        if (!sendBuffer || sendSize == 0) {
            debugPrintln(DEBUG_STR_ERROR "There is no sendBuffer or sendSize set!");
            return 0;
        }

        _client->deleteFile(HTTP_SEND_TMP_FILENAME); // cleanup the file first (if exists)

        if (!_client->writeFile(HTTP_SEND_TMP_FILENAME, (uint8_t*)sendBuffer, sendSize)) {
            debugPrintln(DEBUG_STR_ERROR "Could not create the http tmp file!");
            return 0;
        }

        return httpRequestFromFile(server, port, endpoint, requestType, responseBuffer, responseSize,
                                   HTTP_SEND_TMP_FILENAME, timeout, useURC);
    }

    // reset http profile 0
    _client->modemPrintln("AT+UHTTP=0");
    if (_client->readResponse() != OI4GResponseOK) {
        return 0;
    }

    _client->deleteFile(HTTP_RECEIVE_FILENAME); // cleanup the file first (if exists)

    if (requestType >= HttpRequestTypesMAX) {
        debugPrintln(DEBUG_STR_ERROR "Unknown request type!");
        return 0;
    }

    // set server host name
    _client->modemPrint("AT+UHTTP=0,");
    _client->modemPrint(_client->isValidIPv4(server) ? "0,\"" : "1,\"");
    _client->modemPrint(server);
    _client->modemPrintln("\"");
    if (_client->readResponse() != OI4GResponseOK) {
        return 0;
    }

    // set port
   // if (port != 80) {
        _client->modemPrint("AT+UHTTP=0,5,");
        _client->modemPrintln(port);

        if (_client->readResponse() != OI4GResponseOK) {
            return 0;
        }
    //}

    // reset the success bit before calling a new request
    _httpRequestSuccessBit[requestType] = Undefined;

    _client->modemPrint("AT+UHTTPC=0,");
    _client->modemPrint(requestType < sizeof(httpRequestMapping) ? httpRequestMapping[requestType] : 1);
    _client->modemPrint(",\"");
    _client->modemPrint(endpoint);
    _client->modemPrintln("\",\"\""); // empty filename = default = "http_last_response_0" (DEFAULT_HTTP_RECEIVE_FILENAME)

    if (_client->readResponse() != OI4GResponseOK) {
        return 0;
    }

    _client->modemPrint("AT+UDNSRN=0,\"");
    _client->modemPrint(server);
    _client->modemPrintln("\"");

    if (_client->readResponse() != OI4GResponseOK) {
        return 0;
    }

    // check for success while checking URCs
    // This loop relies on readResponse being called via isAlive()
    uint32_t start = millis();
    uint32_t delay_count = 50;

    while ((_httpRequestSuccessBit[requestType] == Undefined) && !is_timedout(start, timeout)) {
        if (useURC) {
            _client->isAlive();
            //do the link between OI4GNode & OI4GHttp
            getSuccessStatus(requestType);

            if (_httpRequestSuccessBit[requestType] != Undefined) {
                break;
            }
        }
        else {
            uint32_t size;
            _client->getFileSize(HTTP_RECEIVE_FILENAME, size);
            _client->modemPrintln("AT+UHTTPER=0");
            if (_client->readResponse(NULL, 100) == OI4GResponseOK) {
                break;
            }
        }

        _client->wdtSafeDelay(delay_count);
        // Next time wait a little longer, but not longer than 5 seconds
        if (delay_count < 5000) {
            delay_count += 250;
        }
    }

    if (_httpRequestSuccessBit[requestType] == True) {
        uint32_t file_size;
        if (!_client->getFileSize(HTTP_RECEIVE_FILENAME, file_size)) {
            debugPrintln(DEBUG_STR_ERROR "Could not determine file size");
            return 0;
        }
        if (responseBuffer && responseSize > 0 && file_size < responseSize) {
            return _client->readFile(HTTP_RECEIVE_FILENAME, (uint8_t*)responseBuffer, responseSize);
        }
        else {
            // On AVR this can give size_t overflow
            return file_size;
        }
    }
    else if (_httpRequestSuccessBit[requestType] == False) {
        debugPrintln(DEBUG_STR_ERROR "An error occurred with the http request!");
        return 0;
    }
    else {
        debugPrintln(DEBUG_STR_ERROR "Timed out waiting for a response for the http request!");
        return 0;
    }

    return 0;
}

// Creates an HTTP request using the (optional) given buffer and
// (optionally) returns the received data.
// endpoint should include the initial "/".
// Request body must first be prepared in a file on the modem
size_t OI4GHttp::httpRequestFromFile(const char* server, uint16_t port, const char* endpoint, HttpRequestTypes requestType,
                                      char* responseBuffer, size_t responseSize,
                                      const char* fileName, uint32_t timeout, bool useURC)
{
    if (requestType != PUT && requestType != POST) {
        return 0;
    }

    // reset http profile 0
    _client->modemPrintln("AT+UHTTP=0");
    if (_client->readResponse() != OI4GResponseOK) {
        return 0;
    }

    _client->deleteFile(HTTP_RECEIVE_FILENAME); // cleanup the file first (if exists)

    if (requestType >= HttpRequestTypesMAX) {
        debugPrintln(DEBUG_STR_ERROR "Unknown request type!");
        return 0;
    }

    // set server host name
    _client->modemPrint("AT+UHTTP=0,");
    _client->modemPrint(_client->isValidIPv4(server) ? "0,\"" : "1,\"");
    _client->modemPrint(server);
    _client->modemPrintln("\"");
    if (_client->readResponse() != OI4GResponseOK) {
        return 0;
    }

    // set port
    if (port != 80) {
        _client->modemPrint("AT+UHTTP=0,5,");
        _client->modemPrintln(port);

        if (_client->readResponse() != OI4GResponseOK) {
            return 0;
        }
    }

    // reset the success bit before calling a new request
    _httpRequestSuccessBit[requestType] = Undefined;

    _client->modemPrint("AT+UHTTPC=0,");
    _client->modemPrint(requestType < sizeof(httpRequestMapping) ? httpRequestMapping[requestType] : 1);
    _client->modemPrint(",\"");
    _client->modemPrint(endpoint);
    _client->modemPrint("\",\"\",\"");
    _client->modemPrint(fileName);
    _client->modemPrintln(requestType == POST ? "\",1" : "\"\n");

    if (_client->readResponse() != OI4GResponseOK) {
        return 0;
    }

    // check for success while checking URCs
    // This loop relies on readResponse being called via isAlive()
    uint32_t start = millis();
    uint32_t delay_count = 50;
    while ((_httpRequestSuccessBit[requestType] == Undefined) && !is_timedout(start, timeout)) {
        if (useURC) {
            _client->isAlive();
            //do the link between OI4GNode & OI4GHttp
            getSuccessStatus(requestType);
            if (_httpRequestSuccessBit[requestType] != Undefined) {
                break;
            }
        }
        else {
            uint32_t size;
            _client->getFileSize(HTTP_RECEIVE_FILENAME, size);
            _client->modemPrintln("AT+UHTTPER=0");
            if (_client->readResponse(NULL, 100) == OI4GResponseOK) {
                break;
            }
        }

        _client->wdtSafeDelay(delay_count);
        // Next time wait a little longer, but not longer than 5 seconds
        if (delay_count < 5000) {
            delay_count += 250;
        }
    }

    if (_httpRequestSuccessBit[requestType] == True) {
        uint32_t file_size;
        if (!_client->getFileSize(HTTP_RECEIVE_FILENAME, file_size)) {
            debugPrintln(DEBUG_STR_ERROR "Could not determine file size");
            return 0;
        }
        if (responseBuffer && responseSize > 0 && file_size < responseSize) {
            return _client->readFile(HTTP_RECEIVE_FILENAME, (uint8_t*)responseBuffer, responseSize);
        }
        else {
            // On AVR this can give size_t overflow
            return file_size;
        }
    }
    else if (_httpRequestSuccessBit[requestType] == False) {
        debugPrintln(DEBUG_STR_ERROR "An error occurred with the http request!");
        return 0;
    }
    else {
        debugPrintln(DEBUG_STR_ERROR "Timed out waiting for a response for the http request!");
        return 0;
    }

    return 0;
}


//  Paremeter index has a range [0-4]
//  Parameters 'name' and 'value' can have a maximum length of 64 characters
//  Parameters 'name' and 'value' must not include the ':' character
bool OI4GHttp::httpSetCustomHeader(uint8_t index, const char* name, const char* value)
{
    _client->modemPrint("AT+UHTTP=0,9,\"");
    _client->modemPrint(index);
    _client->modemPrint(':');

    if (name != NULL) {
        _client->modemPrint(name);
        _client->modemPrint(':');
        _client->modemPrint(value);
    }

    _client->modemPrintln('"');

    return (_client->readResponse() == OI4GResponseOK);
}

bool OI4GHttp::httpClearCustomHeader(uint8_t index)
{
    return httpSetCustomHeader(index, NULL, NULL);
}


