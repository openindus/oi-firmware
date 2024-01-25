#include "OI4GSms.h"

#ifdef DEBUG_SMS
#warning "OI4GSMS Debug mode is ON"
#define debugPrint(...)   { if (_diagStream) _diagStream->print(__VA_ARGS__); }
#define debugPrintln(...) { if (_diagStream) _diagStream->println(__VA_ARGS__); }
#else
#define debugPrint(...)
#define debugPrintln(...)
#endif


static const char* SIMStatus[] = {
    "REC UNREAD", // 1 
    "REC READ", // 2 
    "STO UNSENT", // 3 
    "STO SENT", // 4 
    "ALL", // 5 
};


OI4GSms::OI4GSms() {

    _lastMsgIndex = 0;


}

void OI4GSms::begin(OI4GNode* client) {

    _client=client;
    _diagStream=_client->getDiagStream();
    //_client->checkRegistration();


}

bool OI4GSms::selectMessageService(bool service) { //acknowledge or not an incoming message

    _client->modemPrint("AT+CSMS=");
    _client->modemPrintln(service ? '1' : '0');

    return (_client->readResponse()==OI4GResponseOK);


}

bool OI4GSms::getMessageService(char* buffer, size_t size) {

    if(buffer == NULL || size < 14 + 1) {
        return false;
    }

    return (_client->execCommand("AT+CSMS?", DEFAULT_READ_MS, buffer, size) == OI4GResponseOK) && (strlen(buffer) > 0) && (atoll(buffer) > 0);

}

//use storage define in header : ME, SM, MT, BM, SR
bool OI4GSms::selectMessageStorage(const char* read_delete_storage, const char * write_send_storage, const char * receive_storage) {

    _client->modemPrint("AT+CPMS=\""); 
    _client->modemPrint(read_delete_storage);
    _client->modemPrint("\",\"");
    _client->modemPrint(write_send_storage);
    _client->modemPrint("\",\"");
    _client->modemPrint(receive_storage);
    _client->modemPrintln("\"");

    return (_client->readResponse()==OI4GResponseOK);

}

bool OI4GSms::checkMessageStorage() {
    _client->modemPrintln("AT+CPMS?");

    return  (_client->readResponse()==OI4GResponseOK);
}

bool OI4GSms::selectMessageFormat(bool mode) { //1: text mode, 0: PDU mode

    _client->modemPrint("AT+CMGF=");
    _client->modemPrintln(mode ? '1' : '0');

    return (_client->readResponse()==OI4GResponseOK);

}

bool OI4GSms::getMessageFormat(char* buffer, size_t size) {

    if(buffer == NULL || size < 8 + 1) {
        return false;
    }

    return (_client->execCommand("AT+CMGF?", DEFAULT_READ_MS, buffer, size) == OI4GResponseOK) && (strlen(buffer) > 0) && (atoll(buffer) > 0);

}

bool OI4GSms::saveSettings(uint8_t profile) { //save settings in non volatile memory, default profile : 0

    _client->modemPrint("AT+CSAS=");
    _client->modemPrintln(profile);

    return (_client->readResponse()==OI4GResponseOK);


}


bool OI4GSms::restoreSettings(uint8_t profile) { //restore settings from non volatile memory, default profile : 0

    _client->modemPrint("AT+CRES=");
    _client->modemPrintln(profile);

    return (_client->readResponse()==OI4GResponseOK);


}

bool OI4GSms::showDetailedHeader(bool enable) { //when reading a message enable the print of the header that contains specific informations

    _client->modemPrint("AT+CSDH=");
    _client->modemPrintln(enable ? '1' : '0');

    return (_client->readResponse()==OI4GResponseOK);
}

//modes: 0,1,2,3
bool OI4GSms::incomeMessageAlertMode(uint8_t receive_mode, uint8_t notify_mode) { //choose how to be alert when a new sms is received

    if(receive_mode > 2 || notify_mode > 3) {
        return false;
    }


    _client->modemPrint("AT+CNMI="); 
    _client->modemPrint(receive_mode);
    _client->modemPrint(",");
    _client->modemPrintln(notify_mode);


    return (_client->readResponse()==OI4GResponseOK);

}

bool OI4GSms::readMessage(uint16_t index, char* buffer, size_t size) {

    if(buffer == NULL || size < MIN_RECV_BUFFER || index > 350 || index ==0) {
        return false;
    }

    _client->modemPrint("AT+CMGR="); 
    _client->modemPrint(index);

    return (_client->readResponse(buffer, size, NULL) == OI4GResponseOK);


}

bool OI4GSms::messageAck() {
    
    char buffer[14];
    int param1;
    getMessageFormat(buffer, sizeof(buffer));
    if(sscanf(buffer, "+CMGF: %d", &param1) == 1) {
        if(param1) {
            _client->modemPrintln("AT+CNMA");
        }

        else {
            _client->modemPrintln("AT+CNMA=0");
        }
    }
    

     return (_client->readResponse()==OI4GResponseOK);

}

//status: REC UNREAD, REC READ, STO UNSENT, STO SENT, ALL
bool OI4GSms::listMessages(uint8_t status, char* buffer, size_t size) {

    if(buffer == NULL || size < MIN_RECV_BUFFER) {
        return false;
    }

    char format_buffer[14];
    int param1;
    getMessageFormat(format_buffer, sizeof(format_buffer));
    if(sscanf(format_buffer, "+CMGF: %d", &param1) == 1) {
        _client->modemPrint("AT+CMGL=\"");

        if(param1) {
            _client->modemPrint(SIMStatus[status - 1]);
            _client->modemPrintln("\"");
        }

        else {
            _client->modemPrintln(status);
        }
    }

    return (_client->readResponse(buffer, size, NULL) == OI4GResponseOK);
}

bool OI4GSms::sendTextMessage(const char* dest, const char* text) {

    char buffer[16];
    _client->modemPrint("AT+CMGS=\"");
    _client->modemPrint(dest);
    _client->modemPrint("\"");
    _client->modemPrint(CR);

    //if(_client->readResponse()==OI4GResponseOK) {
        if(_client->readResponse()==OI4GResponsePrompt) {
            _client->modemPrint('"');
            _client->modemPrint(text);
            _client->modemPrint('"');
            _client->modemPrintln((char)26);
        //_client->writeByte(0x1A); //CTRL-Z doesn't work
        }
    //}

    
    return (_client->readResponse(buffer, sizeof(buffer), "+CMGS: ")==OI4GResponseOK);

}

//In PDU mode, the dest adress is in the text variable, the length variable indicate the length of the payload 
//and so allow the modem to determine the bytes that correspond to the dest addr
bool OI4GSms::sendPDUMessage(uint8_t length, const char* text) {
    _client->modemPrint("AT+CMGS=");
    _client->modemPrintln(length);

    _client->modemPrint(text);
    _client->modemPrintln((char)26);
    //_client->writeByte(0x1A);

    return (_client->readResponse()==OI4GResponseOK);
}

//the buffer contain the index corresponding to the message in the memory
bool OI4GSms::writeTextToMemory(const char* dest, const char* text, char* buffer, size_t size) {

    if(buffer==NULL || size < 15+1) {
        return false;
    }
    
    _client->modemPrint("AT+CMGW=\"");
    _client->modemPrint(dest);
    _client->modemPrint("\"");
    _client->modemPrint(CR);

    if(_client->readResponse()==OI4GResponsePrompt) {

        _client->modemPrint(text);
        _client->modemPrintln((char)26);

    }

    return (_client->readResponse(buffer, size, NULL) == OI4GResponseOK);

}

bool OI4GSms::writePDUToMemory(uint8_t length, const char* text, char* buffer, size_t size) {
   
    if(buffer==NULL || size < 15+1) {
        return false;
    }
   
    _client->modemPrint("AT+CMGW=");
    _client->modemPrintln(length);

    if(_client->readResponse()==OI4GResponsePrompt) {

        _client->modemPrint(text);
        _client->modemPrintln((char)26);
    }

    return (_client->readResponse(buffer, size, NULL) == OI4GResponseOK);

}

bool OI4GSms::sendFromStorage(uint16_t index) {
    char buffer[20];
    if ( index > 350 || index < 0) {
        return false;
    }

    _client->modemPrint("AT+CMSS=");
    _client->modemPrintln(index);

    return (_client->readResponse(buffer, sizeof(buffer), "+CMSS: ", 10000U)==OI4GResponseOK);

}

bool OI4GSms::deleteMessage(uint16_t index, uint8_t flag) {
    if (flag > 4 || index > 350 || index ==0) {
        return false;
    }

    _client->modemPrint("AT+CMDG=");
    if (flag >0) {
        _client->modemPrint(index);
        _client->modemPrint(",");
        _client->modemPrintln(flag);

    }

    else {
        _client->modemPrintln(index);
    }

    return (_client->readResponse()==OI4GResponseOK);

}

bool OI4GSms::setServiceCenterAddr(const char* service_addr) {

    _client->modemPrint("AT+CSCA=\"");
    _client->modemPrint(service_addr);
    _client->modemPrintln("\"");

    return (_client->readResponse()==OI4GResponseOK);

}

bool OI4GSms::getServiceCenterAddr(char* buffer, size_t size) {

    if(buffer == NULL || size < 25 + 1) {
        return false;
    }

    return (_client->execCommand("AT+CSCA?", DEFAULT_READ_MS, buffer, size) == OI4GResponseOK) && (strlen(buffer) > 0) && (atoll(buffer) > 0);

}

bool OI4GSms::readConcatenedMessage(uint16_t index, char* buffer, size_t size) {

    if(buffer == NULL || size < MIN_RECV_BUFFER || index > 350 || index ==0) {
        return false;
    }

    _client->modemPrint("AT+UCMGR="); 
    _client->modemPrint(index);

    return (_client->readResponse(buffer, size, NULL) == OI4GResponseOK);


}

bool OI4GSms::listConcatenedMessage(uint8_t status, char* buffer, size_t size) {

    if(buffer == NULL || size < MIN_RECV_BUFFER ) {
        return false;
    }

    
    _client->modemPrint("AT+UCMGL=");

        
    _client->modemPrintln(SIMStatus[status - 1]);
    _client->modemPrintln("\"");

    return (_client->readResponse(buffer, size, NULL) == OI4GResponseOK);
}

bool OI4GSms::sendConcatenedMessage(const char* dest, const char* text, uint8_t seq, uint8_t max, uint8_t iei, uint16_t ref) {

    _client->modemPrint("AT+UCMGS=\"");
    _client->modemPrint(dest);
    _client->modemPrint("\",,");
    _client->modemPrint(seq);
    _client->modemPrint(",");
    _client->modemPrint(max);
    _client->modemPrint(",");
    _client->modemPrint(iei);
    _client->modemPrint(",");
    _client->modemPrintln(ref);

    _client->modemPrint(text);
    _client->modemPrintln((char)26);

     return (_client->readResponse()==OI4GResponseOK);

}

bool OI4GSms::writeConcatenedToMemory(const char* dest, const char* text, uint8_t seq, uint8_t max, uint8_t iei, uint16_t ref) {

    _client->modemPrint("AT+UCMGW=\"");
    _client->modemPrint(dest);
    _client->modemPrint("\",,");
    _client->modemPrint(seq);
    _client->modemPrint(",");
    _client->modemPrint(max);
    _client->modemPrint(",");
    _client->modemPrint(iei);
    _client->modemPrint(",");
    _client->modemPrintln(ref);

    _client->modemPrint(text);
    _client->modemPrintln((char)26);

     return (_client->readResponse()==OI4GResponseOK);

}

int OI4GSms::getLastMsgIndex() {
    _lastMsgIndex = _client->_lastMsg;

    return _lastMsgIndex;
}

void OI4GSms::smsLoop()
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
