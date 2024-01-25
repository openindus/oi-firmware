#include "OI4GFtp.h"

#ifdef DEBUG_FTP
#warning "OI4GFtp Debug mode is ON"
#define debugPrint(...)   { if (_diagStream) _diagStream->print(__VA_ARGS__); }
#define debugPrintln(...) { if (_diagStream) _diagStream->println(__VA_ARGS__); }
#else
#define debugPrint(...)
#define debugPrintln(...)
#endif

OI4GFtp::OI4GFtp() {

    _ftpLoginResult =-1;
    memset(_bufferInfo,0,sizeof(_bufferInfo));
    strcpy(_bufferInfo,"");
}

void OI4GFtp::begin(OI4GNode* client) {
    _client = client;
    _diagStream=_client->getDiagStream();

}

bool OI4GFtp::ftpSetPort(uint16_t port) {
    _client->modemPrint("AT+UFTP=7,");
    _client->modemPrintln(port);

    return (_client->readResponse()==OI4GResponseOK);
}


bool OI4GFtp::ftpSetServer(const char * server) {
    _client->modemPrint("AT+UFTP=1,\"");
    _client->modemPrint(server);
    _client->modemPrintln("\"");
    return  (_client->readResponse()==OI4GResponseOK); 
        
}

bool OI4GFtp::ftpSetServerIP(const char * ip) {
    _client->modemPrint("AT+UFTP=1,\"");
    _client->modemPrint(ip);
    _client->modemPrintln("\"");

    return (_client->readResponse()==OI4GResponseOK);

}

bool OI4GFtp::ftpSetName(const char * name) {
    _client->modemPrint("AT+UFTP=2,\"");
    _client->modemPrint(name);
    _client->modemPrintln("\"");

    return (_client->readResponse()==OI4GResponseOK);
}

bool OI4GFtp::ftpSetPw(const char * pw) {
    _client->modemPrint("AT+UFTP=3,\"");
    _client->modemPrint(pw);
    _client->modemPrintln("\"");

    return (_client->readResponse()==OI4GResponseOK);
}


bool OI4GFtp::ftptSetAuth(const char* name, const char* pw) {

    _client->modemPrint("AT+UFTP=2,\"");
    _client->modemPrint(name);
    _client->modemPrintln("\"");

    if(_client->readResponse()==OI4GResponseOK) {
        _client->modemPrint("AT+UFTP=3,\"");
        _client->modemPrint(pw);
        _client->modemPrintln("\"");
    }

    return (_client->readResponse()==OI4GResponseOK);

}

bool OI4GFtp::ftpSetAccount(const char * account) {
    _client->modemPrint("AT+UFTP=4,\"");
    _client->modemPrint(account);
    _client->modemPrintln("\"");

    return (_client->readResponse()==OI4GResponseOK);

}


bool OI4GFtp::ftpSetInactivityTimeout(uint16_t timeout) {

    _client->modemPrint("AT+UFTP=5,");
    _client->modemPrintln(timeout);

    return (_client->readResponse()==OI4GResponseOK);


}

bool OI4GFtp::ftpSetMode(bool mode) {
    
    _client->modemPrint("AT+UFTP=6,");
    _client->modemPrintln(mode ? '1' : '0');

    return (_client->readResponse()==OI4GResponseOK);

}

bool OI4GFtp::ftpSetSecureOption(bool enable, int8_t profile) {

    _client->modemPrint("AT+UFTP=8,");
    _client->modemPrintln(enable ? '1' : '0');

    if (profile >= 0) {
        _client->modemPrint(',');
        _client->modemPrintln(profile);
    }


    return (_client->readResponse()==OI4GResponseOK);
}

bool OI4GFtp::ftpLogin(uint32_t timeout) {
    char buffer[16];
    uint32_t startTime = millis();
    _ftpLoginResult = -1;
    strcpy(_bufferInfo, "");
    while ((_ftpLoginResult == -1) && !is_timedout(startTime, timeout)) {

        _client->modemPrintln("AT+UFTPC=1");
        if(_client->readResponse()==OI4GResponseOK) {

            if (_client->readResponse(buffer, sizeof(buffer), "+UUFTPCR: ", 6000)) {

                if (_client->startsWith("1,1", buffer)) {
                    _ftpLoginResult = 0;
                    debugPrint("Login response : ");
                    debugPrintln(buffer);
                }

                else {
                    _ftpLoginResult = 1;
                    debugPrint("Error in login response : ");
                    debugPrintln(buffer);
                }
                
            }

            else {
                debugPrintln("Login error...");
                delay(5000);
            }

        }
        


    }



    return (_ftpLoginResult ==0);
}

bool OI4GFtp::ftpResolveIp(const char* server, char* buffer) {

        return _client->resolveIp(server, buffer);
    
}

bool OI4GFtp::ftpLogout() {
    _ftpLoginResult = -1;
    char buffer[20];

    _client->modemPrintln("AT+UFTPC=0");
    if (_client->readResponse()==OI4GResponseOK) {
        return(_client->readResponse(buffer, sizeof(buffer), "+UUFTPCR: "));
    }

    else {
        return false;
    }
}

bool OI4GFtp::ftpDeleteFile(const char * filename) {
    char buffer[20];
    _client->modemPrint("AT+UFTPC=2,\"");
    _client->modemPrint(filename);
    _client->modemPrintln("\"");

    if (_client->readResponse()==OI4GResponseOK) {
        return(_client->readResponse(buffer, sizeof(buffer), "+UUFTPCR: "));
    }

    else {
        return false;
    }

}

bool OI4GFtp::ftpRenameFile(const char* filename, const char * new_name) {
    char buffer[20];
    if(sizeof(filename)<=0 || sizeof(new_name) <=0) {
        debugPrintln("Empty filename or new name");
        return false;
    }

    _client->modemPrint("AT+UFTPC=3,\"");
    _client->modemPrint(filename);
    _client->modemPrint("\",\"");
    _client->modemPrint(new_name);
    _client->modemPrintln("\"");

    
    if (_client->readResponse()==OI4GResponseOK) {
        return(_client->readResponse(buffer, sizeof(buffer), "+UUFTPCR: "));
    }

    else {
        return false;
    }

}

bool OI4GFtp::ftpRetrieveFile(const char* remote_file, const char* local_file_name, bool retrieve_mode) {

    char buffer[20];

    if(sizeof(remote_file)<=0 || sizeof(local_file_name) <=0) {
        debugPrintln("Empty remote file name or local name");
        return false;
    }

    _client->modemPrint("AT+UFTPC=4,\"");
    _client->modemPrint(remote_file);
    _client->modemPrint("\",\"");
    _client->modemPrint(local_file_name);
    _client->modemPrint("\",");
    _client->modemPrintln(retrieve_mode ? '1' : '0');

    if (_client->readResponse()==OI4GResponseOK) {
        return(_client->readResponse(buffer, sizeof(buffer), "+UUFTPCR: "));
    }

    else {
        return false;
    }

}

bool OI4GFtp::ftpStoreFile(const char * local_file_name, const char * remote_file, uint32_t offset) {
    char buffer[20];
    if(sizeof(remote_file)<=0 || sizeof(local_file_name) <=0) {
        debugPrintln("Empty remote file name or local name");
        return false;
    }

    _client->modemPrint("AT+UFTPC=5,\"");
    _client->modemPrint(local_file_name);
    _client->modemPrint("\",\"");
    _client->modemPrint(remote_file);
    _client->modemPrint("\",");
    _client->modemPrintln(offset);
    
    if (_client->readResponse()==OI4GResponseOK) {
        return(_client->readResponse(buffer, sizeof(buffer), "+UUFTPCR: "));
    }

    else {
        return false;
    }
}

    bool OI4GFtp::ftpChangeWorkingDirectory(const char * directory_name) {
    char buffer[20];
    _client->modemPrint("AT+UFTPC=8,\"");
    _client->modemPrint(directory_name);
    _client->modemPrintln("\"");

    if (_client->readResponse()==OI4GResponseOK) {
        return(_client->readResponse(buffer, sizeof(buffer), "+UUFTPCR: "));
    }

    else {
        return false;
    }

}

bool OI4GFtp::ftpCreateDirectory(const char * directory_name) {
    char buffer[20];
    _client->modemPrint("AT+UFTPC=10,\"");
    _client->modemPrint(directory_name);
    _client->modemPrintln("\"");

    if (_client->readResponse()==OI4GResponseOK) {
        return(_client->readResponse(buffer, sizeof(buffer), "+UUFTPCR: "));
    }

    else {
        return false;
    }
}

bool OI4GFtp::ftpRemoveDirectory(const char * directory_name) {
    char buffer[20];
    _client->modemPrint("AT+UFTPC=11,\"");
    _client->modemPrint(directory_name);
    _client->modemPrintln("\"");

    if (_client->readResponse()==OI4GResponseOK) {
        return(_client->readResponse(buffer, sizeof(buffer), "+UUFTPCR: "));
    }

    else {
        return false;
    }

}

bool OI4GFtp::ftpGetInfos(const char * file_directory_name) {

    uint32_t timeout = 6000;
    uint32_t startTime = millis();
    strcpy(_bufferInfo, "");
    
   if(file_directory_name != NULL) {
        _client->modemPrint("AT+UFTPC=13,\"");
        _client->modemPrint(file_directory_name);
        _client->modemPrintln("\"");
    }
    
   else {
       _client->modemPrintln("AT+UFTPC=13");
   }
    if(_client->readResponse()!=OI4GResponseOK) {
        debugPrintln("Issue with FTP request");
        return false;
    }

    /*while(!is_timedout(startTime, timeout)) {

        _client->checkURC(_client->getInputBuffer());
        strcpy(_bufferInfo, _client->_urcBuffer);
    }
    */
    if(_client->readResponse()==OI4GResponseOK) {
        if (_client->readResponse(_bufferInfo, sizeof(_bufferInfo), "+UUFTPCD: ", 6000)) {
        debugPrintln(_bufferInfo);
        }
    }
    

    if(sizeof(_bufferInfo) < 2) {
        return false;
    }

    return true;

}

bool OI4GFtp::ftpGetFilesList(const char * directory_name) {

    uint32_t timeout = 3 * 60 * 1000;
    uint32_t startTime = millis();
    strcpy(_bufferInfo, "");

   
    if(directory_name != NULL) {
        _client->modemPrint("AT+UFTPC=14,\"");
        _client->modemPrint(directory_name);
        _client->modemPrintln("\"");
    }

    else {
         _client->modemPrintln("AT+UFTPC=14");
    }
    
    

    if(_client->readResponse()!=OI4GResponseOK) {
        return false;
    }

    while(_bufferInfo==0 &&!is_timedout(startTime, timeout)) {

        _client->checkURC(_client->getInputBuffer());
        strcpy(_bufferInfo, _client->_urcBuffer);
    }

    if(_bufferInfo==0) {
        return false;
    }

    return true;

}
