#include "OI4GNode.h"

#define REC_UNREAD 1
#define REC_READ 2
#define STO_UNSENT 3
#define STO_SENT 4 
#define ALL 5

#define MOBILE_EQUIPMT_STOR "ME" //mobile equipment storage
#define SIM_MSG_STOR        "SM" //sim message storage
#define MOBILE_TERM_STOR    "MT" //mobile terminated storage: ME+SM storage (ME preferred)
#define BRDCST_MSG_STOR     "BM" //broadcast message storage
#define STATUS_REP_STOR     "SR" //status report storage

#define MAX_BUFFER 1024
#define MIN_RECV_BUFFER 512


class OI4GSms {

    public:

    OI4GSms();

    void begin(OI4GNode* client);
    bool selectMessageService(bool service);
    bool getMessageService(char* buffer, size_t size);
    bool selectMessageStorage(const char* read_delete_storage, const char * write_send_storage, const char * receive_storage);
    bool checkMessageStorage();
    bool selectMessageFormat(bool mode);
    bool getMessageFormat(char * buffer, size_t size);
    bool saveSettings(uint8_t profile =0);
    bool restoreSettings(uint8_t profile =0);

    bool showDetailedHeader(bool enable);

    bool incomeMessageAlertMode(uint8_t receive_mode, uint8_t notify_mode); //TODO : add optionnal parameters

    bool readMessage(uint16_t index,char * buffer, size_t size);

    bool messageAck(); // use if selectMessageService(1) has been set

    bool listMessages(uint8_t status, char* buffer, size_t size); 

    bool sendTextMessage(const char* dest, const char* text);

    bool sendPDUMessage(uint8_t length, const char* text);

    bool writeTextToMemory(const char* dest, const char* text, char* buffer, size_t size);

    bool writePDUToMemory(uint8_t length, const char* text, char* buffer, size_t size);

    bool sendFromStorage(uint16_t index);

    bool deleteMessage(uint16_t index, uint8_t flag=0);

    bool setServiceCenterAddr(const char* service_addr);
    bool getServiceCenterAddr(char* buffer, size_t size);

    bool readConcatenedMessage(uint16_t index, char* buffer,size_t size);
    bool sendConcatenedMessage(const char* dest,const char* text, uint8_t seq, uint8_t max, uint8_t iei, uint16_t ref);
    bool writeConcatenedToMemory(const char* dest,const char* text, uint8_t seq, uint8_t max, uint8_t iei, uint16_t ref);
    bool listConcatenedMessage(uint8_t status, char* buffer, size_t size);

    int getLastMsgIndex();
    void smsLoop();

    private:

    OI4GNode* _client;
    Stream* _diagStream;
    int     _lastMsgIndex;

};