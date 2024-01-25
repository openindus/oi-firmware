#include "OI4GNode.h"


#define EPOCH_TIME_OFF             946684800  /* This is 1st January 2000, 00:00:00 in epoch time */
#define EPOCH_TIME_YEAR_OFF        100        /* years since 1900 */
#define ATTACH_TIMEOUT             180000
#define ATTACH_NEED_REBOOT         40000
#define CGACT_TIMEOUT              150000
#define COPS_TIMEOUT               180000
#define ISCONNECTED_CSQ_TIMEOUT    10000
#define REBOOT_DELAY               1250
#define REBOOT_TIMEOUT             15000
#define POWER_OFF_DELAY            5000

#define OI4G_GSM_TERMINATOR "\r\n"
#define OI4G_DEFAULT_INPUT_BUFFER_SIZE 1024
#define OI4G_GSM_TERMINATOR_LEN (sizeof(OI4G_GSM_TERMINATOR) - 1)

#ifdef DEBUG_NODE
#warning "OI4GNode Debug mode is ON"
#define debugPrint(...)   { if (_diagStream) _diagStream->print(__VA_ARGS__); }
#define debugPrintln(...) { if (_diagStream) _diagStream->println(__VA_ARGS__); }
#else
#define debugPrint(...)
#define debugPrintln(...)
#endif




//-------------------------------------------------------------------------------------------------//

OI4GNode::OI4GNode() :
    _modemStream(0),
    _diagStream(0),
    _appendCommand(false),
    _CSQtime(0),
    _startOn(0)
{
    _isBufferInitialized = false;
    _inputBuffer         = 0;
    _inputBufferSize     = OI4G_DEFAULT_INPUT_BUFFER_SIZE;
    _lastRSSI            = 0;
    _minRSSI             = -113;  // dBm
    _pin                 = 0;
    _onoff               = false;
    _urcFlag             =0;
    _pendingMessages     =0;
    _lastMsg         =0;
    _currentUrat    =0;

    memset(_socketClosedBit,    1, sizeof(_socketClosedBit));
    memset(_socketPendingBytes, 0, sizeof(_socketPendingBytes));
    memset(_urcBuffer, 0, sizeof(_urcBuffer));
}

// Initializes the modem instance. Sets the modem stream and the on-off power pins.
void OI4GNode::init(Stream& stream, uint8_t cid)
{
    debugPrintln("[init] started.");

    initBuffer(); // safe to call multiple times

    setModemStream(stream);
    ((HardwareSerial*)_modemStream)->begin(getBaudrate(),SERIAL_8N1,RX,TX);
    _cid   = cid;
}

void OI4GNode::wdtSafeDelay(uint32_t ms)
{
  // Delay step size
  uint32_t delay_step = 10;
  uint32_t endMS = millis() + ms;

  // Loop through and reset between steps
  while (millis() < endMS) {
    //sodaq_wdt_reset();

    int32_t remaining = (endMS - millis());
    delay((remaining  > delay_step) ? delay_step : remaining);
  }
}

// Turns the modem on and returns true if successful.
bool OI4GNode::on()
{
    _startOn = millis();

    if(!_onoff) {

    debugPrintln("Starting up modem...");
    pinMode(SARA_R4XX_TOGGLE, OUTPUT);
    digitalWrite(SARA_R4XX_TOGGLE, HIGH);
    delay(2000);
    digitalWrite(SARA_R4XX_TOGGLE, LOW);
    // We should be able to reduce this to 50ms or something
    wdtSafeDelay(2000);
    pinMode(SARA_R4XX_TOGGLE, INPUT);

    }

   

    // wait for power up
    bool timeout = true;
    for (uint8_t i = 0; i < 10; i++) {
        if (isAlive()) {
            timeout = false;
            _onoff = true;
            break;
        }
   }

    if (timeout) {
        debugPrintln("Error: No Reply from Modem");
        return false;
    }

    // Extra read just to clear the input stream
    readResponse(NULL, 0, NULL, 250);

    return true;

}

// Turns the modem off and returns true if successful.
bool OI4GNode::off()
{
    // Safety command to shutdown, response is ignored
    if (_onoff) {
        modemPrintln("AT+CPWROFF");
        readResponse(NULL, 0, NULL, 1000);
        wdtSafeDelay(POWER_OFF_DELAY);
    }
  

    return true;
}


//to code timers see 3GPP TS 24.008 table 10.5.163 and table 10.5.163a (page 615-616)
// ex : sleep_time : Ob00100100 = 4 minutes
bool OI4GNode::setPSM(bool enable, const char* sleep_time, const char* active_time) {

    modemPrint("AT+CPSMS=");
    modemPrint(enable ? '1' : '0');
    modemPrint(",,,\"");
    modemPrint(sleep_time);
    modemPrint("\",\"");
    modemPrint(active_time);
    modemPrintln("\"");

    return (readResponse() == OI4GResponseOK);


}

bool OI4GNode::getPSM(char * buffer, size_t size) {

    if(buffer == NULL || size < 32 +1 ) {
        return false;
    }

    return (execCommand("AT+CPSMS?", DEFAULT_READ_MS, buffer, size) == OI4GResponseOK) && (strlen(buffer) > 0) && (atoll(buffer) > 0);

}


bool OI4GNode::attachNode(uint32_t timeout)
{
    uint32_t start = millis();
    uint32_t delay_count = 500;

    while (!is_timedout(start, timeout)) {
        if (isAttached()) {
            if (checkIPv4() || (execCommand("AT+CGACT=1", CGACT_TIMEOUT) && checkIPv4())) {
                return true;
            }
        }

        wdtSafeDelay(delay_count);

        // Next time wait a little longer, but not longer than 5 seconds
        if (delay_count < 5000) {
            delay_count += 1000;
        }
    }

    return false;
}


bool OI4GNode::connect(const char* apn, const char* uratSelect, uint8_t mnoProfile,
    const char* operatorSelect, const char* bandMaskLTE, const char* bandMaskNB)
{

    clearAllResponsesRead();

    if (!setVerboseErrors(true)) {
        return false;
    }

    if (!execCommand("ATE0")) {
        return false;
    }

    if (!checkCFUN()) {
        return false;
    }

    /*if (!execCommand("AT+CREG=1")) {
        return false;
    }*/

    if (!checkProfile(mnoProfile)) {
        return false;
    }


    if (!checkUrat(uratSelect != 0 ? uratSelect : DEFAULT_URAT)) {
        return false;
    }


    if (!checkBandMasks(bandMaskLTE, bandMaskNB)) {
        return false;
    }

    if (!checkCOPS(operatorSelect != 0 ? operatorSelect : AUTOMATIC_OPERATOR, uratSelect != 0 ? uratSelect : DEFAULT_URAT)) {
        return false;
    }

    int8_t i = checkApn(apn);
    if (i < 0) {
        return false;
    }

    if (!execCommand("AT+CFUN=15")) {  //reboot the module to take effect
        return false;
    }


    uint32_t tm = millis();

    if (!waitForSignalQuality()) {
        return false;

    }

    if (i == 0 && !attachNode(ATTACH_TIMEOUT)) {
        return false;
    }

    if (millis() - tm > ATTACH_NEED_REBOOT) {
        reboot();
        
        if (!waitForSignalQuality()) {
            return false;
        }

        if (!attachNode(ATTACH_TIMEOUT)) {
            return false;
        }

        

    }

    return doSIMcheck();
}

bool OI4GNode::resolveIp(const char* server, char* buffer) {

        modemPrint("AT+UDNSRN=0,\"");
        modemPrint(server);
        modemPrintln("\"");
        if(readResponse()==OI4GResponseOK) {
            strcpy(buffer, _urcBuffer);
            return true;
        }

        else {
            return false;
        }
}

bool OI4GNode::disconnect()
{
    return execCommand("AT+CGATT=0", 40000);
}

bool OI4GNode::getCCID(char* buffer, size_t size)
{
    if (buffer == NULL || size < 20 + 1) {
        return false;
    }

    modemPrintln("AT+CCID");

    return (readResponse(buffer, size, "+CCID: ") == OI4GResponseOK) && (strlen(buffer) > 0);
}

// Gets the International Mobile Subscriber Identity
// Returns true if successful.
bool OI4GNode::getIMSI(char* buffer, size_t size)
{
    if (buffer == NULL || size < 20 + 1) {
        return false;
    }

    return (execCommand("AT+CIMI", DEFAULT_READ_MS, buffer, size) == OI4GResponseOK) && (strlen(buffer) > 0) && (atoll(buffer) > 0);
}

bool OI4GNode::getOperatorInfo(uint16_t* mcc, uint16_t* mnc)
{
    modemPrintln("AT+COPS=3,2");

    if (readResponse() != OI4GResponseOK) {
        return false;
    }

    modemPrintln("AT+COPS?");

    char responseBuffer[64];
    memset(responseBuffer, 0, sizeof(responseBuffer));

    uint32_t operatorCode = 0;

    if ((readResponse(responseBuffer, sizeof(responseBuffer), "+COPS: ") == OI4GResponseOK) && (strlen(responseBuffer) > 0)) {

        if (sscanf(responseBuffer, "%*d,%*d,\"%u\"", &operatorCode) == 1) {
            uint16_t divider = (operatorCode > 100000) ? 1000 : 100;

            *mcc = operatorCode / divider;
            *mnc = operatorCode % divider;

            return true;
        }
    }

    return false;
}

bool OI4GNode::getOperatorInfoString(char* buffer, size_t size)
{
    if (size < 32 + 1) {
         return false;
    }

    buffer[0] = 0;

    modemPrintln("AT+COPS=3,0");

    if (readResponse() != OI4GResponseOK) {
        return false;
    }

    modemPrintln("AT+COPS?");

    char responseBuffer[64];
    memset(responseBuffer, 0, sizeof(responseBuffer));

    if ((readResponse(responseBuffer, sizeof(responseBuffer), "+COPS: ") == OI4GResponseOK) && (strlen(responseBuffer) > 0)) {

        if (sscanf(responseBuffer, "%*d,%*d,\"%[^\"]\"", buffer) == 1) {
            return true;
        }
    }

    return false;
}

bool OI4GNode::getCellInfo(uint16_t* tac, uint32_t* cid, uint16_t* urat)
{
    modemPrintln("AT+CEREG=2");

    if (readResponse() != OI4GResponseOK) {
        return false;
    }

    modemPrintln("AT+CEREG?");

    char responseBuffer[64];
    memset(responseBuffer, 0, sizeof(responseBuffer));

    if ((readResponse(responseBuffer, sizeof(responseBuffer), "+CEREG: ") == OI4GResponseOK) && (strlen(responseBuffer) > 0)) {
        if (sscanf(responseBuffer, "2,%*d,\"%hx\",\"%x\",%hi", tac, cid, urat) == 3) {
            switch(*urat) {
                case 7:
                case 8: *urat = 7; break;
                case 9: *urat = 8; break;
            }
            return true;
        }
        else {
            // if +CEREG did not return the tac/cid/act
            // lets try +CGREG for GPRS registration info
            modemPrintln("AT+CGREG=2");

            if (readResponse() != OI4GResponseOK) {
                return false;
            }

            modemPrintln("AT+CGREG?");

            memset(responseBuffer, 0, sizeof(responseBuffer));

            if ((readResponse(responseBuffer, sizeof(responseBuffer), "+CGREG: ") == OI4GResponseOK) && (strlen(responseBuffer) > 0)) {
                if (sscanf(responseBuffer, "2,%*d,\"%hx\",\"%x\"", tac, cid) == 2) {
                    *urat = 9;
                    return true;
                }
            }
        }
    }

    return false;
}

bool OI4GNode::getEpoch(uint32_t* epoch)
{
    modemPrintln("AT+CCLK?");

    char buffer[128];

    if (readResponse(buffer, sizeof(buffer), "+CCLK: ") != OI4GResponseOK) {
        return false;
    }

    // format: "yy/MM/dd,hh:mm:ss+TZ
    int y, m, d, h, min, sec, tz;
    if (sscanf(buffer, "\"%d/%d/%d,%d:%d:%d+%d\"", &y, &m, &d, &h, &min, &sec, &tz) == 7 ||
        sscanf(buffer, "\"%d/%d/%d,%d:%d:%d\"", &y, &m, &d, &h, &min, &sec) == 6)
    {
        *epoch = dateTimeToEpoch(y, m, d, h, min, sec);
        return true;
    }

    return false;
}

bool OI4GNode::getFirmwareVersion(char* buffer, size_t size)
{
    if (buffer == NULL || size < 30 + 1) {
        return false;
    }

    return execCommand("AT+CGMR", DEFAULT_READ_MS, buffer, size);
}

bool OI4GNode::getFirmwareRevision(char* buffer, size_t size)
{
    if (buffer == NULL || size < 30 + 1) {
        return false;
    }

    return execCommand("ATI9", DEFAULT_READ_MS, buffer, size);
}

// Gets International Mobile Equipment Identity.
// Returns true if successful.
bool OI4GNode::getIMEI(char* buffer, size_t size)
{
    if (buffer == NULL || size < 15 + 1) {
        return false;
    }

    return (execCommand("AT+CGSN", DEFAULT_READ_MS, buffer, size) == OI4GResponseOK) && (strlen(buffer) > 0) && (atoll(buffer) > 0);
}

SimStatus OI4GNode::getSimStatus()
{
    modemPrintln("AT+CPIN?");

    char buffer[32];

    if (readResponse(buffer, sizeof(buffer)) != OI4GResponseOK) {
        return SimStatusUnknown;
    }

    char status[16];

    if (sscanf(buffer, "+CPIN: %" STR(sizeof(status) - 1) "s", status) == 1) {
        return startsWith("READY", status) ? SimReady : SimNeedsPin;
    }

    return SimMissing;
}

bool OI4GNode::execCommand(const char* command, uint32_t timeout, char* buffer, size_t size)
{
    modemPrintln(command);

    return (readResponse(buffer, size, NULL, timeout) == OI4GResponseOK);
}

// Returns true if the modem replies to "AT" commands without timing out.
bool OI4GNode::isAlive()
{
    return execCommand(STR_AT, 450);
}

// Returns true if the modem is attached to the network and has an activated data connection.
bool OI4GNode::isAttached()
{
    modemPrintln("AT+CGATT?");

    char buffer[16];

    if (readResponse(buffer, sizeof(buffer), "+CGATT: ", 10 * 1000) != OI4GResponseOK) {
        return false;
    }

    return (strcmp(buffer, "1") == 0);
}

// Returns true if the modem is connected to the network and IP address is not 0.0.0.0.
bool OI4GNode::isConnected()
{
    return isAttached() && waitForSignalQuality(ISCONNECTED_CSQ_TIMEOUT) && checkIPv4();
}

// Returns true if defined IP4 address is not 0.0.0.0.
bool OI4GNode::checkIPv4()
{
    modemPrintln("AT+CGDCONT?");

    char buffer[256];

    if (readResponse(buffer, sizeof(buffer), "+CGDCONT: ") != OI4GResponseOK) {
        return false;
    }

    if (strncmp(buffer, "1,\"IP\"", 6) != 0 || strncmp(buffer + 6, ",\"\"", 3) == 0) {
        return false;
    }

    char ip[32];

    if (sscanf(buffer + 6, ",\"%*[^\"]\",\"%[^\"]\",0,0,0,0", ip) != 1) {
        return false;
    }

    return strlen(ip) >= 7 && strcmp(ip, "0.0.0.0") != 0;
}


bool OI4GNode::isValidIPv4(const char* str)
{
    uint8_t  segs  = 0; // Segment count
    uint8_t  chcnt = 0; // Character count within segment
    uint16_t accum = 0; // Accumulator for segment

    if (!str) {
        return false;
    }

    // Process every character in string
    while (*str != '\0') {
        // Segment changeover
        if (*str == '.') {
            // Must have some digits in segment
            if (chcnt == 0) {
                return false;
            }

            // Limit number of segments
            if (++segs == 4) {
                return false;
            }

            // Reset segment values and restart loop
            chcnt = accum = 0;
            str++;
            continue;
        }

        // Check numeric
        if ((*str < '0') || (*str > '9')) {
            return false;
        }

        // Accumulate and check segment
        if ((accum = accum * 10 + *str - '0') > 255) {
            return false;
        }

        // Advance other segment specific stuff and continue loop
        chcnt++;
        str++;
    }

    // Check enough segments and enough characters in last segment
    if (segs != 3) {
        return false;
    }

    if (chcnt == 0) {
        return false;
    }

    // Address OK
    return true;
}


void OI4GNode::clearAllResponsesRead()
{
    uint32_t start = millis();

    // make sure all the responses within the timeout have been read
    while ((readResponse(NULL, 0, NULL, 1000) != OI4GResponseTimeout) && !is_timedout(start, 2000)) {}
}

bool OI4GNode::setApn(const char* apn)
{
    modemPrint("AT+CGDCONT=");
    modemPrint(_cid);
    modemPrint(",\"IP\",\"");
    modemPrint(apn);
    modemPrintln('"');

    return (readResponse() == OI4GResponseOK);
}

void OI4GNode::setPin(const char * pin)
{
    size_t len = strlen(pin);
    _pin = static_cast<char*>(realloc(_pin, len + 1));
    strcpy(_pin, pin);
    setSimPin(pin);
}

bool OI4GNode::setRadioActive(bool on)
{
    modemPrint("AT+CFUN=");
    modemPrintln(on ? "1" : "0");

    return (readResponse() == OI4GResponseOK);
}

bool OI4GNode::setVerboseErrors(bool on)
{
    modemPrint("AT+CMEE=");
    modemPrintln(on ? '2' : '0');

    return (readResponse() == OI4GResponseOK);
}

int8_t OI4GNode::convertCSQ2RSSI(uint8_t csq) const
{
    return -113 + 2 * csq;
}

uint8_t OI4GNode::convertRSSI2CSQ(int8_t rssi) const
{
    return (rssi + 113) / 2;
}

// Gets the Received Signal Strength Indication in dBm and Bit Error Rate.
// Returns true if successful.
bool OI4GNode::getRSSIAndBER(int8_t* rssi, uint8_t* ber)
{
    static char berValues[] = { 49, 43, 37, 25, 19, 13, 7, 0 }; // 3GPP TS 45.008 [20] subclause 8.2.4

    modemPrintln("AT+CSQ");

    char buffer[256];

    if (readResponse(buffer, sizeof(buffer), "+CSQ: ") != OI4GResponseOK) {
        return false;
    }

    int csqRaw;
    int berRaw;

    if (sscanf(buffer, "%d,%d", &csqRaw, &berRaw) != 2) {
        return false;
    }

    *rssi = ((csqRaw == 99) ? 0 : convertCSQ2RSSI(csqRaw));
    *ber  = ((berRaw == 99 || static_cast<size_t>(berRaw) >= sizeof(berValues)) ? 0 : berValues[berRaw]);

    return true;
}


bool OI4GNode::deleteFile(const char* filename)
{
    // TODO escape filename characters
    modemPrint("AT+UDELFILE=\"");
    modemPrint(filename);
    modemPrintln('"');

    return (readResponse() == OI4GResponseOK);
}

bool OI4GNode::listFiles() {
     modemPrintln("AT+ULSTFILE");

     return (readResponse()==OI4GResponseOK);
}

bool OI4GNode::getFileSize(const char* filename, uint32_t& size)
{
    size = 0;

    modemPrint("AT+ULSTFILE=2,\"");
    modemPrint(filename);
    modemPrintln('"');

    char buffer[32];

    if (readResponse(buffer, sizeof(buffer), "+ULSTFILE: ") != OI4GResponseOK) {
        return false;
    }

    return (sscanf(buffer, "%4u", &size) == 1);
}

size_t OI4GNode::readFile(const char* filename, uint8_t* buffer, size_t size)
{

    if (!buffer || size == 0) {
        return 0;
    }

    // first, make sure the buffer is sufficient
    uint32_t filesize = 0;
    if (!getFileSize(filename, filesize)) {
        debugPrintln(DEBUG_STR_ERROR "Could not determine file size");
        return 0;
    }

    if (filesize > size) {
        debugPrintln(DEBUG_STR_ERROR "The buffer is not big enough to store the file");
        return 0;
    }

    modemPrint("AT+URDFILE=\"");
    modemPrint(filename);
    modemPrintln('"');

    char checkChar = 0;

    // reply identifier
    size_t len = readBytesUntil(' ', _inputBuffer, _inputBufferSize);
    if (len == 0 || strstr(_inputBuffer, "+URDFILE:") == NULL) {
        debugPrintln(DEBUG_STR_ERROR "+URDFILE literal is missing!");
        return 0;
    }

    // filename
    len = readBytesUntil(',', _inputBuffer, _inputBufferSize);

    // filesize
    len = readBytesUntil(',', _inputBuffer, _inputBufferSize);
    filesize = 0; 
    if (sscanf(_inputBuffer, "%4u", &filesize) != 1) {
        debugPrintln(DEBUG_STR_ERROR "Could not parse the file size!");
        return 0;
    }
    if (filesize == 0 || filesize > size) {
        debugPrintln(DEBUG_STR_ERROR "Size error!");
        return 0;
    }

    // opening quote character
    checkChar = timedRead();
    if (checkChar != '"') {
        debugPrintln(DEBUG_STR_ERROR "Missing starting character (quote)!");
        return 0;
    }

    // actual file buffer, written directly to the provided result buffer
    len = readBytes(buffer, filesize);
    if (len != filesize) {
        debugPrintln(DEBUG_STR_ERROR "File size error!");
        return 0;
    }

    // closing quote character
    checkChar = timedRead();
    if (checkChar != '"') {
        debugPrintln(DEBUG_STR_ERROR "Missing termination character (quote)!");
        return 0;
    }

    // read final OK response from modem and return the filesize
    return (readResponse() == OI4GResponseOK ? filesize : 0);
}

size_t OI4GNode::readFilePartial(const char* filename, uint8_t* buffer, size_t size, uint32_t offset)
{

    if (!buffer || size == 0) {
        return 0;
    }

    modemPrint("AT+URDBLOCK=\"");
    modemPrint(filename);
    modemPrint("\",");
    modemPrint(offset);
    modemPrint(',');
    modemPrintln(size);

    // reply identifier
    //   +URDBLOCK: http_last_response_0,86,"..."
    // where 86 is an example of the size
    size_t len = readBytesUntil(' ', _inputBuffer, _inputBufferSize);
    if (len == 0 || strstr(_inputBuffer, "+URDBLOCK:") == NULL) {
        debugPrintln(DEBUG_STR_ERROR "+URDBLOCK literal is missing!");
        return 0;
    }

    // skip filename
    len = readBytesUntil(',', _inputBuffer, _inputBufferSize);
    // TODO check filename. Note, there are no quotes

    // read the number of bytes
    len = readBytesUntil(',', _inputBuffer, _inputBufferSize);
    uint32_t blocksize = 0; // reset the var before reading from reply string
    if (sscanf(_inputBuffer, "%4u", &blocksize) != 1) {
        debugPrintln(DEBUG_STR_ERROR "Could not parse the block size!");
        return 0;
    }
    if (blocksize == 0 || blocksize > size) {
        debugPrintln(DEBUG_STR_ERROR "Size error!");
        return 0;
    }

    // opening quote character
    char quote = timedRead();
    if (quote != '"') {
        debugPrintln(DEBUG_STR_ERROR "Missing starting character (quote)!");
        return 0;
    }

    // actual file buffer, written directly to the provided result buffer
    len = readBytes(buffer, blocksize);
    if (len != blocksize) {
        debugPrintln(DEBUG_STR_ERROR "File size error!");
        return 0;
    }

    // closing quote character
    quote = timedRead();
    if (quote != '"') {
        debugPrintln(DEBUG_STR_ERROR "Missing termination character (quote)!");
        return 0;
    }

    // read final OK response from modem and return the filesize
    return (readResponse() == OI4GResponseOK ? blocksize : 0);
}

// If the file already exists, the data will be appended to the file already stored in the file system.
bool OI4GNode::writeFile(const char* filename, const uint8_t* buffer, size_t size)
{
    modemPrint("AT+UDWNFILE=\"");
    modemPrint(filename);
    modemPrint("\",");
    modemPrintln(size);

    if (readResponse() != OI4GResponsePrompt) {
        return false;
    }

    for (size_t i = 0; i < size; i++) {
        writeByte(buffer[i]);
    }

    return (readResponse() == OI4GResponseOK);
}



//--------------------------------------------------------------------------------------------------------//

int8_t OI4GNode::checkApn(const char* requiredAPN)
{
    modemPrintln("AT+CGDCONT?");

    char buffer[256];

    if (readResponse(buffer, sizeof(buffer), "+CGDCONT: ") != OI4GResponseOK) {
        return -1;
    }

    if (strncmp(buffer, "1,\"IP\"", 6) == 0 && strncmp(buffer + 6, ",\"\"", 3) != 0) {
        char apn[64];
        char ip[32];

        if (sscanf(buffer + 6, ",\"%[^\"]\",\"%[^\"]\",0,0,0,0", apn, ip) != 2) { return -1; }

        if (strcmp(apn, requiredAPN) == 0) {
            if (strlen(ip) >= 7 && strcmp(ip, "0.0.0.0") != 0) { 
                return 1; 
            }
            else {
                return 0;
            }
        }
    }

    return setApn(requiredAPN) ? 0 : -1;
}

bool OI4GNode::checkBandMasks(const char* bandMaskLTE, const char* bandMaskNB)
{
    // if no changes required
    if ((bandMaskLTE == BAND_MASK_UNCHANGED) && (bandMaskNB == BAND_MASK_UNCHANGED)) {
        return true;
    }
    

    modemPrintln("AT+UBANDMASK?");

    char buffer[128];

    if (readResponse(buffer, sizeof(buffer), "+UBANDMASK: ") != OI4GResponseOK) {
        return false;
    }

    char bm0[32];
    char bm1[32];
    if (sscanf(buffer, "0,%[^,],1,%s", bm0, bm1) != 2) {
        return false;
    }

    bool setLTEMask = (strcmp(bm0, bandMaskLTE) != 0) && (bandMaskLTE != BAND_MASK_UNCHANGED);
    bool setNBMask = (strcmp(bm1, bandMaskNB) != 0) && (bandMaskNB != BAND_MASK_UNCHANGED);

    // masks are both already match those requested
    if (!setLTEMask && !setNBMask) {
        return true;
    }

    modemPrint("AT+UBANDMASK=");
    if (setLTEMask) {
        modemPrint("0,");
        modemPrint(bandMaskLTE);
        if (setNBMask) {
            modemPrint(",");
        }
    }
    if (setNBMask) {
        modemPrint("1,");
        modemPrint(bandMaskNB);
    }
    modemPrintln();

    if (readResponse() != OI4GResponseOK) {
        return false;
    }

    reboot();

    return true;
}

bool OI4GNode::checkCFUN()
{
    modemPrintln("AT+CFUN?");

    char buffer[64];

    if (readResponse(buffer, sizeof(buffer), "+CFUN: ") != OI4GResponseOK) {
        return false;
    }

    return ((strcmp(buffer, "1") == 0) || setRadioActive(true));
}


bool OI4GNode::checkRegistration() {
    char buffer[64];
    
    modemPrintln("AT+CREG?"); 
    return (readResponse(buffer, sizeof(buffer), "+CREG")== OI4GResponseOK);
    
}

bool OI4GNode::checkCOPS(const char* requiredOperator, const char* requiredURAT)
{
    // If auto operator and not NB1, always send the command
    if ((strcmp(requiredOperator, AUTOMATIC_OPERATOR) == 0) && (strcmp(requiredURAT, OI4G_LTEM_URAT) == 0)){
        return execCommand("AT+COPS=0,2", COPS_TIMEOUT);
    }

    modemPrintln("AT+COPS=3,2");

    if (readResponse() != OI4GResponseOK) {
        return false;
    }

    modemPrintln("AT+COPS?");

    char buffer[64];

    if (readResponse(buffer, sizeof(buffer), "+COPS: ", COPS_TIMEOUT) != OI4GResponseOK) {
        return false;
    }

    if (strcmp(requiredOperator, AUTOMATIC_OPERATOR) == 0) {
        return ((strncmp(buffer, "0", 1) == 0) || execCommand("AT+COPS=0,2", COPS_TIMEOUT));
    }
    else if ((strncmp(buffer, "1", 1) == 0) && (strncmp(buffer + 5, requiredOperator, strlen(requiredOperator)) == 0)) {
        return true;
    }
    else {
        modemPrint("AT+COPS=1,2,\"");
        modemPrint(requiredOperator);
        modemPrintln('"');

        return (readResponse(NULL, 0, NULL, COPS_TIMEOUT) == OI4GResponseOK);
    }
}

bool OI4GNode::checkProfile(const uint8_t requiredProfile)
{
    modemPrintln("AT+UMNOPROF?");

    char buffer[64];
    if (readResponse(buffer, sizeof(buffer), "+UMNOPROF: ") != OI4GResponseOK) {
        return false;
    }

    if (atoi(buffer) == requiredProfile) {
        return true;
    }

    modemPrintln("AT+COPS=2");
    if (readResponse() != OI4GResponseOK) {
        return false;
    }

    modemPrint("AT+UMNOPROF=");
    modemPrintln(requiredProfile);
    if (readResponse() != OI4GResponseOK) {
        return false;
    }

    reboot();

    return true;
}

bool OI4GNode::checkUrat(const char* requiredURAT)
{

    strcpy(_currentUrat, requiredURAT);
    // Only try and skip if single URAT
    if (strlen(requiredURAT) == 1) {
        modemPrintln("AT+URAT?");

        char buffer[64];

        if (readResponse(buffer, sizeof(buffer), "+URAT: ") != OI4GResponseOK || strlen(buffer) == 0) {
            return false;
        }

        if (strcmp(buffer, requiredURAT) == 0) {
            return true;
        }
    }

    modemPrintln("AT+COPS=2");
    if (readResponse() != OI4GResponseOK) {
        return false;
    }

    modemPrint("AT+URAT=");
    modemPrintln(requiredURAT);
    if (readResponse() != OI4GResponseOK) {
        return false;
    }

    reboot();

    return true;
}

bool OI4GNode::doSIMcheck()
{
    const uint8_t retry_count = 10;

    for (uint8_t i = 0; i < retry_count; i++) {
        if (i > 0) {
            wdtSafeDelay(250);
        }

        SimStatus simStatus = getSimStatus();
        if (simStatus == SimNeedsPin) {
            if (_pin == 0 || *_pin == '\0' || !setSimPin(_pin)) {
                debugPrintln(DEBUG_STR_ERROR "SIM needs a PIN but none was provided, or setting it failed!");
                return false;
            }
        }
        else if (simStatus == SimReady) {
            return true;
        }
    }

    return false;
}

bool OI4GNode::checkURC(char* buffer)
{
    _urcFlag = 0;
    memset(_urcBuffer, 0, MAX_SEND_MESSAGE_SIZE);

    if (buffer[0] != '+') {
        return false;
    }

    int param1, param2;
    char param3[1024];

    if (sscanf(buffer, "+UFOTAS: %d,%d", &param1, &param2) == 2) {
        #ifdef DEBUG_NODE
        debugPrint("Unsolicited: FOTA: ");
        debugPrint(param1);
        debugPrint(", ");
        debugPrintln(param2);
        #endif

        return true;
    }

    if (sscanf(buffer, "+UHTTPER: 0,%*d,%d", &param1) == 1) {
        debugPrint("Unsolicited: UHTTPER: ");
        debugPrintln(param1);

        //_httpRequestSuccessBit[1] = param1 == 0 ? TriBoolTrue : TriBoolFalse;
        _urcFlag = param1 == 0 ? HTTP_OK : HTTP_ERROR; 

        return true;
    }

    if (sscanf(buffer, "+UUHTTPCR: 0,%d,%d", &param1, &param2) == 2) {

        enum HttpRequestTypes {
            POST = 0,
            GET,
            HEAD,
            DELETE,
            PUT,
            HttpRequestTypesMAX
        };

        static uint8_t mapping[] = {
            HEAD,   // 0
            GET,    // 1
            DELETE, // 2
            PUT,    // 3
            POST,   // 4
        };

        int requestType = param1 < (int)sizeof(mapping) ? mapping[param1] : -1;
        if (requestType >= 0) {
            debugPrint("Unsolicited: UUHTTPCR: ");
            debugPrint(requestType);
            debugPrint(": ");
            debugPrintln(param2);

            if (param2 == 0) {
                _urcFlag = HTTP_ERROR;
            }
            else if (param2 == 1) {
                _urcFlag = HTTP_OK;
            }
        }

        return true;
    }

    if (sscanf(buffer, "+UUSORD: %d,%d", &param1, &param2) == 2) {
        debugPrint("Unsolicited: Socket ");
        debugPrint(param1);
        debugPrint(": ");
        debugPrintln(param2);

        if (param1 >= 0 && param1 < SOCKET_COUNT) {
            _socketPendingBytes[param1] = param2;
        }

        return true;
    }

    if (sscanf(buffer, "+UUSORF: %d,%d", &param1, &param2) == 2) {
        debugPrint("Unsolicited: Socket ");
        debugPrint(param1);
        debugPrint(": ");
        debugPrintln(param2);

        if (param1 >= 0 && param1 < SOCKET_COUNT) {
            _socketPendingBytes[param1] = param2;
        }

        return true;
    }

    if (sscanf(buffer, "+UUSOCL: %d", &param1) == 1) {
        debugPrint("Unsolicited: Socket ");
        debugPrintln(param1);

        if (param1 >= 0 && param1 < SOCKET_COUNT) {
            _socketClosedBit[param1] = true;
        }

        return true;
    }

    if (sscanf(buffer, "+UUMQTTC: 1,%d", &param1) == 1) {
        debugPrint("Unsolicited: MQTT login result: ");
        debugPrintln(param1);

        //_mqttLoginResult = param1;
        _urcFlag= (param1 == 0 ? MQTT_LOG_OK : MQTT_LOG_ERROR);

        return true;
    }

    if (sscanf(buffer, "+UUMQTTC: 4,%d,%d,\"%[^\"]\"", &param1, &param2, param3) == 3) {
        debugPrint("Unsolicited: MQTT subscription result: ");
        debugPrint(param1);
        debugPrint(", ");
        debugPrint(param2);
        debugPrint(", ");
        debugPrintln(param3);

       //_mqttSubscribeReason = param1;
        _urcFlag= (param1 == 1 ? MQTT_SUB_OK : MQTT_SUB_ERROR);

        return true;
    }

    if (sscanf(buffer, "+UUMQTTCM: 6,%d", &param1) == 1) {
        debugPrint("Unsolicited: MQTT pending messages:");
        debugPrintln(param1);

        //_mqttPendingMessages = param1;
        _pendingMessages= param1;

        return true;
    }

    //FTP URC info
    if (sscanf(buffer, "+UUFTPCR: %d,%d", &param1, &param2) == 1) {
        debugPrint("Unsolicited: FTP data messages +UUFTPCR: ");
        debugPrint(param1);
        debugPrint(",")
        debugPrintln(param2);

        return true;
    }

    if (sscanf(buffer, "+UUFTPCD: 13,%d,%s", &param1, param3) == 1) {
        debugPrint("Unsolicited: FTP data messages:");
        debugPrintln(param1);
        debugPrintln(param3);
        strcpy(_urcBuffer, param3);

        return true;
    }

    if (sscanf(buffer, "+UUFTPCD: 14,%d,\"%[^\"]\"", &param1, param3) == 1) {
        debugPrint("Unsolicited: FTP data messages:");
        debugPrintln(param1);

        strcpy(_urcBuffer, param3);


        return true;
    }

    if (sscanf(buffer, "+UDNSRN: \"%[^\"]\"",  param3) == 1) {

        strcpy(_urcBuffer, param3);


        return true;
    }

    if(sscanf(buffer, "+CMTI: %*s,%d",&param1)==1) {
        debugPrintln("Unsolicited: New SMS received with index:");
        debugPrintln(param1);

        _lastMsg=param1;
        return true;
    }

    return false;
}


OI4GResponseTypes OI4GNode::readResponse(char* outBuffer, size_t outMaxSize, const char* prefix, uint32_t timeout)
{
    bool usePrefix    = prefix != NULL && prefix[0] != 0;
    bool useOutBuffer = outBuffer != NULL && outMaxSize > 0;

    uint32_t from = NOW;

    size_t outSize = 0;

    if (outBuffer) {
        outBuffer[0] = 0;
    }

    while (!is_timedout(from, timeout)) {
        int count = readLn(_inputBuffer, _inputBufferSize, 250); // 250ms, how many bytes at which baudrate?
        //wdtSafeReset();
        if (count <= 0) {
            continue;
        }
        debugPrint("<< ");
        debugPrintln(_inputBuffer);

        if (startsWith(STR_AT, _inputBuffer)) {
            continue; // skip echoed back command
        }

        if (startsWith(STR_RESPONSE_OK, _inputBuffer)) {
            return OI4GResponseOK;
        }

        if (startsWith(STR_RESPONSE_ERROR, _inputBuffer) ||
                startsWith(STR_RESPONSE_CME_ERROR, _inputBuffer) ||
                startsWith(STR_RESPONSE_CMS_ERROR, _inputBuffer)) {
            return OI4GResponseError;
        }

        if ((_inputBuffer[0] == STR_RESPONSE_SOCKET_PROMPT) || (_inputBuffer[0] == STR_RESPONSE_FILE_PROMPT)) {
            return OI4GResponsePrompt;
        }

        bool hasPrefix = usePrefix && useOutBuffer && startsWith(prefix, _inputBuffer);

        if (!hasPrefix && checkURC(_inputBuffer)) {
            continue;
        }

        if (hasPrefix || (!usePrefix && useOutBuffer)) {
            if (outSize > 0 && outSize < outMaxSize - 1) {
                outBuffer[outSize++] = LF;
                return OI4GResponseOK;
            }

            if (outSize < outMaxSize - 1) {
                char* inBuffer = _inputBuffer;
                if (hasPrefix) {
                    int i = strlen(prefix);
                    count -= i;
                    inBuffer += i;
                }
                if (outSize + count > outMaxSize - 1) {
                    count = outMaxSize - 1 - outSize;
                }
                memcpy(outBuffer + outSize, inBuffer, count);
                outSize += count;
                outBuffer[outSize] = 0;
            }
            continue;
            
        }
        return OI4GResponseOK;
        
    }

    
    //debugPrintln("<< timed out");

    return OI4GResponseTimeout;
}

void OI4GNode::reboot()
{
    modemPrintln("AT+CFUN=15");

    // wait up to 2000ms for the modem to come up
    uint32_t start = millis();

    while ((readResponse() != OI4GResponseOK) && !is_timedout(start, 2000)) {}

    // wait for the reboot to start
    wdtSafeDelay(REBOOT_DELAY);

    while (!is_timedout(start, REBOOT_TIMEOUT)) {
        if (getSimStatus() == SimReady) {
            break;
        }
    }

    // echo off again after reboot
    execCommand("ATE0");

    // extra read just to clear the input stream
    readResponse(NULL, 0, NULL, 250);
}

bool OI4GNode::setSimPin(const char* simPin)
{
    modemPrint("AT+CPIN=\"");
    modemPrint(simPin);
    modemPrintln('"');

    return (readResponse() == OI4GResponseOK);
}

bool OI4GNode::waitForSignalQuality(uint32_t timeout)
{
    uint32_t start = millis();
    const int8_t minRSSI = getMinRSSI();
    int8_t rssi;
    uint8_t ber;

    uint32_t delay_count = 500;

    while (!is_timedout(start, timeout)) {
        if (getRSSIAndBER(&rssi, &ber)) {
            if (rssi != 0 && rssi >= minRSSI) {
                _lastRSSI = rssi;
                _CSQtime = (int32_t)(millis() - start) / 1000;
                return true;
            }
        }

        wdtSafeDelay(delay_count);

        // Next time wait a little longer, but not longer than 5 seconds
        if (delay_count < 5000) {
            delay_count += 1000;
        }
    }

    return false;
}


uint32_t OI4GNode::dateTimeToEpoch(int y, int m, int d, int h, int min, int sec)
{
    struct tm tm;

    tm.tm_isdst = -1;
    tm.tm_yday  = 0;
    tm.tm_wday  = 0;
    tm.tm_year  = y + EPOCH_TIME_YEAR_OFF;
    tm.tm_mon   = m - 1;
    tm.tm_mday  = d;
    tm.tm_hour  = h;
    tm.tm_min   = min;
    tm.tm_sec   = sec;

    return mktime(&tm);
}

bool OI4GNode::startsWith(const char* pre, const char* str)
{
    return (strncmp(pre, str, strlen(pre)) == 0);
}

void OI4GNode::writeProlog()
{
    if (!_appendCommand) {
        debugPrint(">> ");
        _appendCommand = true;
    }
}

// Write a byte, as binary data
size_t OI4GNode::writeByte(uint8_t value)
{
    writeProlog();
    debugPrint(value);
    return _modemStream->write(value);
}

size_t OI4GNode::modemPrint(const String& buffer)
{
    writeProlog();
    debugPrint(buffer);

    return _modemStream->print(buffer);
}

size_t OI4GNode::modemPrint(const char buffer[])
{
    writeProlog();
    debugPrint(buffer);

    return _modemStream->print(buffer);
}

size_t OI4GNode::modemPrint(char value)
{
    writeProlog();
    debugPrint(value);

    return _modemStream->print(value);
};

size_t OI4GNode::modemPrint(unsigned char value, int base)
{
    writeProlog();
    debugPrint(value, base);

    return _modemStream->print(value, base);
};

size_t OI4GNode::modemPrint(int value, int base)
{
    writeProlog();
    debugPrint(value, base);

    return _modemStream->print(value, base);
};

size_t OI4GNode::modemPrint(unsigned int value, int base)
{
    writeProlog();
    debugPrint(value, base);

    return _modemStream->print(value, base);
};

size_t OI4GNode::modemPrint(long value, int base)
{
    writeProlog();
    debugPrint(value, base);

    return _modemStream->print(value, base);
};

size_t OI4GNode::modemPrint(unsigned long value, int base)
{
    writeProlog();
    debugPrint(value, base);

    return _modemStream->print(value, base);
};


size_t OI4GNode::modemPrintln(const String &s)
{
    return modemPrint(s) + modemPrintln();
}

size_t OI4GNode::modemPrintln(const char c[])
{
    return modemPrint(c) + modemPrintln();
}

size_t OI4GNode::modemPrintln(char c)
{
    return modemPrint(c) + modemPrintln();
}

size_t OI4GNode::modemPrintln(unsigned char b, int base)
{
    return modemPrint(b, base) + modemPrintln();
}

size_t OI4GNode::modemPrintln(int num, int base)
{
    return modemPrint(num, base) + modemPrintln();
}

size_t OI4GNode::modemPrintln(unsigned int num, int base)
{
    return modemPrint(num, base) + modemPrintln();
}

size_t OI4GNode::modemPrintln(long num, int base)
{
    return modemPrint(num, base) + modemPrintln();
}

size_t OI4GNode::modemPrintln(unsigned long num, int base)
{
    return modemPrint(num, base) + modemPrintln();
}

size_t OI4GNode::modemPrintln(double num, int digits)
{
    writeProlog();
    debugPrint(num, digits);

    return _modemStream->println(num, digits);
}

size_t OI4GNode::modemPrintln()
{
    debugPrintln();
    size_t i = modemPrint(CR);
    _appendCommand = false;
    return i;
}

// Initializes the input buffer and makes sure it is only initialized once.
// Safe to call multiple times.
void OI4GNode::initBuffer()
{
    debugPrintln("[initBuffer]");

    // make sure the buffers are only initialized once
    if (!_isBufferInitialized) {
        _inputBuffer = static_cast<char*>(malloc(_inputBufferSize));
        _currentUrat = static_cast<char*>(malloc(2));
        _isBufferInitialized = true;
    }
}

// Sets the modem stream.
void OI4GNode::setModemStream(Stream& stream)
{
    _modemStream = &stream;
}

// Returns a character from the modem stream if read within _timeout ms or -1 otherwise.
int OI4GNode::timedRead(uint32_t timeout) const
{
    uint32_t _startMillis = millis();

    do {
        int c = _modemStream->read();

        if (c >= 0) {
            return c;
        }
    } while (millis() - _startMillis < timeout);

    return -1; // -1 indicates timeout
}

// Fills the given "buffer" with characters read from the modem stream up to "length"
// maximum characters and until the "terminator" character is found or a character read
// times out (whichever happens first).
// The buffer does not contain the "terminator" character or a null terminator explicitly.
// Returns the number of characters written to the buffer, not including null terminator.
size_t OI4GNode::readBytesUntil(char terminator, char* buffer, size_t length, uint32_t timeout)
{
    if (length < 1) {
        return 0;
    }

    size_t index = 0;

    while (index < length) {
        int c = timedRead(timeout);

        if (c < 0 || c == terminator) {
            break;
        }

        *buffer++ = static_cast<char>(c);
        index++;
    }
    if (index < length) {
        *buffer = '\0';
    }

    return index; // return number of characters, not including null terminator
}

// Fills the given "buffer" with up to "length" characters read from the modem stream.
// It stops when a character read times out or "length" characters have been read.
// Returns the number of characters written to the buffer.
size_t OI4GNode::readBytes(uint8_t* buffer, size_t length, uint32_t timeout)
{
    size_t count = 0;

    while (count < length) {
        int c = timedRead(timeout);

        if (c < 0) {
            break;
        }

        *buffer++ = static_cast<uint8_t>(c);
        count++;
    }

    return count;
}

// Reads a line (up to the OI4G_GSM_TERMINATOR) from the modem stream into the "buffer".
// The buffer is terminated with null.
// Returns the number of bytes read, not including the null terminator.
size_t OI4GNode::readLn(char* buffer, size_t size, uint32_t timeout)
{
    // Use size-1 to leave room for a string terminator
    size_t len = readBytesUntil(OI4G_GSM_TERMINATOR[OI4G_GSM_TERMINATOR_LEN - 1], buffer, size - 1, timeout);

    // check if the terminator is more than 1 characters, then check if the first character of it exists
    // in the calculated position and terminate the string there
    if ((OI4G_GSM_TERMINATOR_LEN > 1) && (buffer[len - (OI4G_GSM_TERMINATOR_LEN - 1)] == OI4G_GSM_TERMINATOR[0])) {
        len -= OI4G_GSM_TERMINATOR_LEN - 1;
    }

    // terminate string, there should always be room for it (see size-1 above)
    buffer[len] = '\0';

    return len;
}
