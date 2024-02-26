
#ifndef _OI4G_h
#define _OI4G_h

#include "Arduino.h"
#include "OI4GGlobal.h"


#define CR '\r'
#define LF '\n'
#define NOW (uint32_t)millis()

static inline bool is_timedout(uint32_t from, uint32_t nr_ms) __attribute__((always_inline));
static inline bool is_timedout(uint32_t from, uint32_t nr_ms) { return (millis() - from) > nr_ms; }

//#define SARA_ENABLE      (49u) //to define on ESP, which pin will drive the enable? 
#define SARA_R4XX_TOGGLE 21
#define SARA_VINT        (2u)
#define RX 20
#define TX 19

#define MODEM_STREAM Serial2
//#define SARA_TX_ENABLE   (51u)

#define DEFAULT_READ_MS            5000
#define MAX_SEND_MESSAGE_SIZE      512
#define OI4G_DEFAULT_CID           1
#define OI4G_DEFAULT_READ_TIMOUT   15000
#define OI4G_MAX_SOCKET_BUFFER     1024

#define OI4G_LTEM_URAT             "7"
#define OI4G_2G_URAT               "9"
#define OI4G_LTEM_2G_URAT          "7,9"

#define DEFAULT_URAT               OI4G_LTEM_URAT
#define AUTOMATIC_OPERATOR         "0"
#define BAND_MASK_UNCHANGED         0

#define DEBUG_STR_ERROR            "[ERROR]: "
#define STR_AT                     "AT"
#define STR_RESPONSE_OK            "OK"
#define STR_RESPONSE_ERROR         "ERROR"
#define STR_RESPONSE_CME_ERROR     "+CME ERROR:"
#define STR_RESPONSE_CMS_ERROR     "+CMS ERROR:"
#define STR_RESPONSE_SOCKET_PROMPT '@'
#define STR_RESPONSE_FILE_PROMPT   '>'

#define SOCKET_COUNT 7
//URC define

#define HTTP_OK 10
#define HTTP_ERROR 11

#define MQTT_LOG_OK 20
#define MQTT_LOG_ERROR 21
#define MQTT_SUB_OK 22
#define MQTT_SUB_ERROR 23
#define MQTT_PNDMSG_OK 24
#define MQTT_PNDMSG_ERROR 25

#define SOCKET_PDNMSG_OK 30
#define SOCKET_PDNMSG_ERROR 31
#define SOCKET_CLOSE_OK 32
#define SOCKET_CLOSE_ERROR 33


#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define NIBBLE_TO_HEX_CHAR(i)  ((i <= 9) ? ('0' + i) : ('A' - 10 + i))
#define HIGH_NIBBLE(i)         ((i >> 4) & 0x0F)
#define LOW_NIBBLE(i)          (i & 0x0F)
#define HEX_CHAR_TO_NIBBLE(c)  ((c >= 'A') ? (c - 'A' + 0x0A) : (c - '0'))
#define HEX_PAIR_TO_BYTE(h, l) ((HEX_CHAR_TO_NIBBLE(h) << 4) + HEX_CHAR_TO_NIBBLE(l))


enum OI4GResponseTypes {
    OI4GResponseNotFound = 0,
    OI4GResponseOK = 1,
    OI4GResponseError = 2,
    OI4GResponsePrompt = 3,
    OI4GResponseTimeout = 4,
    OI4GResponseEmpty = 5
};

enum MNOProfiles {
    SWD_DEFAULT     = 0,
    SIM_ICCID       = 1,
    ATT             = 2,
    VERIZON         = 3,
    TELSTRA         = 4,
    T_MOBILE_US     = 5,
    CHINA_TELECOM   = 6,
    VODAFONE        = 19,
    STANDARD_EUROPE = 100
};

enum SimStatus {
    SimStatusUnknown = 0,
    SimMissing,
    SimNeedsPin,
    SimReady
};


class OI4GNode 
{

public: 

bool      _socketClosedBit[SOCKET_COUNT];
size_t    _socketPendingBytes[SOCKET_COUNT];
int16_t   _pendingMessages;
char      _urcBuffer[MAX_SEND_MESSAGE_SIZE];
int        _lastMsg;

OI4GNode();
void init(Stream& stream = MODEM_STREAM,uint8_t cid = OI4G_DEFAULT_CID);

void wdtSafeDelay(uint32_t ms);

bool on();
bool isOn() { return _onoff; };
bool off();

bool setPSM(bool enable, const char* sleep_time, const char* active_time);
bool getPSM(char * buffer, size_t size);

bool isAlive();
bool checkIPv4();
bool checkRegistration();
bool isValidIPv4(const char* str);
bool isAttached();
bool isConnected();

bool getCCID(char* buffer, size_t size);
bool getIMSI(char* buffer, size_t size);
bool getOperatorInfo(uint16_t* mcc, uint16_t* mnc);
bool getOperatorInfoString(char* buffer, size_t size);
bool getCellInfo(uint16_t* tac, uint32_t* cid, uint16_t* urat);
bool getEpoch(uint32_t* epoch);
bool getFirmwareVersion(char* buffer, size_t size);
bool getFirmwareRevision(char* buffer, size_t size);
bool getIMEI(char* buffer, size_t size);
SimStatus getSimStatus();

void clearAllResponsesRead();
bool setApn(const char* apn);
void setPin(const char* pin);
bool setRadioActive(bool on);
bool setVerboseErrors(bool on);

bool attachNode(uint32_t timeout = 10L * 60L * 1000);
bool connect(const char* apn, const char* uratSelect, uint8_t mnoProfile, 
        const char* operatorSelect = AUTOMATIC_OPERATOR, const char* bandMaskLTE = BAND_MASK_UNCHANGED, 
        const char* bandMaskNB = BAND_MASK_UNCHANGED);
bool resolveIp(const char* server, char* buffer = NULL);
bool disconnect();

int8_t  convertCSQ2RSSI(uint8_t csq) const;
uint8_t convertRSSI2CSQ(int8_t rssi) const;

uint8_t getCSQtime()  const { return _CSQtime; }
int8_t  getLastRSSI() const { return _lastRSSI; }
int8_t  getMinRSSI()  const { return _minRSSI; }
uint32_t getBaudrate() { return 115200; };
Stream * getDiagStream() { return _diagStream;}
Stream * getModemStream() {return _modemStream;}
char getUrcFlag() { return _urcFlag;}
char* getInputBuffer() {return _inputBuffer;}
size_t getInputBufferSize() {return _inputBufferSize;}

bool    getRSSIAndBER(int8_t* rssi, uint8_t* ber);

void    setMinCSQ(int csq) { _minRSSI = convertCSQ2RSSI(csq); }
void    setMinRSSI(int rssi) { _minRSSI = rssi; }

void setDebug(Stream &stream) { _diagStream = &stream; }
bool execCommand(const char* command, uint32_t timeout = DEFAULT_READ_MS, char* buffer = NULL, size_t size = 0);

bool   deleteFile(const char* filename);
bool   getFileSize(const char* filename, uint32_t& size);
bool   listFiles();
size_t readFile(const char* filename, uint8_t* buffer, size_t size);
size_t readFilePartial(const char* filename, uint8_t* buffer, size_t size, uint32_t offset);

// If the file already exists, the data will be appended to the file already stored in the file system.
bool   writeFile(const char* filename, const uint8_t* buffer, size_t size);

int timedRead(uint32_t timeout = 1000) const;

size_t readBytesUntil(char terminator, char* buffer, size_t length, uint32_t timeout = 1000);

size_t readBytes(uint8_t* buffer, size_t length, uint32_t timeout = 1000);

size_t readLn(char* buffer, size_t size, uint32_t timeout = 1000);

size_t readLn() { return readLn(_inputBuffer, _inputBufferSize); };

size_t writeByte(uint8_t value);

void writeProlog();

size_t modemPrint(const String &);
size_t modemPrint(const char[]);
size_t modemPrint(char);
size_t modemPrint(unsigned char, int = DEC);
size_t modemPrint(int, int = DEC);
size_t modemPrint(unsigned int, int = DEC);
size_t modemPrint(long, int = DEC);
size_t modemPrint(unsigned long, int = DEC);
size_t modemPrint(double, int = 2);

size_t modemPrintln(const String &s);
size_t modemPrintln(const char[]);
size_t modemPrintln(char);
size_t modemPrintln(unsigned char, int = DEC);
size_t modemPrintln(int, int = DEC);
size_t modemPrintln(unsigned int, int = DEC);
size_t modemPrintln(long, int = DEC);
size_t modemPrintln(unsigned long, int = DEC);
size_t modemPrintln(double, int = 2);
size_t modemPrintln(void);

static bool startsWith(const char* pre, const char* str);
bool   checkURC(char* buffer);
OI4GResponseTypes readResponse(char* outBuffer = NULL, size_t outMaxSize = 0, const char* prefix = NULL,
                                  uint32_t timeout = DEFAULT_READ_MS);


private: 

char* _pin;
uint8_t _cid;

int8_t checkApn(const char* requiredAPN); // -1: error, 0: ip not valid => need attach, 1: valid ip
bool   checkBandMasks(const char* bandMaskLTE, const char* bandMaskNB);
bool   checkCFUN();
bool   checkCOPS(const char* requiredOperator, const char* requiredURAT);
bool   checkProfile(const uint8_t requiredProfile);
bool   checkUrat(const char* requiredURAT);
bool   doSIMcheck();

void   reboot();
bool   setSimPin(const char* simPin);
bool   waitForSignalQuality(uint32_t timeout = 5L * 60L * 1000);


static uint32_t dateTimeToEpoch(int y, int m, int d, int h, int min, int sec);

bool _onoff; 
char*  _currentUrat;
char _urcFlag;
// The stream that communicates with the device.
Stream* _modemStream;
Stream* _diagStream;

// The size of the input buffer. Equals GSM_MODEM_DEFAULT_INPUT_BUFFER_SIZE
// by default or (optionally) a user-defined value when using USE_DYNAMIC_BUFFER.
size_t _inputBufferSize;

// Flag to make sure the buffers are not allocated more than once.
bool _isBufferInitialized;

// The buffer used when reading from the modem. The space is allocated during init() via initBuffer().
char* _inputBuffer;

// This flag keeps track if the next write is the continuation of the current command
// A Carriage Return will reset this flag.
bool _appendCommand;

// This is the value of the most recent CSQ
// Notice that CSQ is somewhat standard. SIM800/SIM900 and Ublox
// compute to comparable numbers. With minor deviations.
// For example SIM800
//   1              -111 dBm
//   2...30         -110... -54 dBm
// For example UBlox
//   1              -111 dBm
//   2..30          -109 to -53 dBm
int8_t _lastRSSI;   // 0 not known or not detectable

uint8_t _CSQtime;

int _minRSSI;

uint32_t _startOn;


void initBuffer();


void setModemStream(Stream& stream);


};


#endif