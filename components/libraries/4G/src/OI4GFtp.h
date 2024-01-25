#include "OI4GNode.h"


class OI4GFtp {

public:


OI4GFtp();
void begin(OI4GNode* client);

bool ftpSetServer(const char* server);
bool ftpSetServerIP(const char* ip);
bool ftpSetPw(const char * pw) ;
bool ftpSetName(const char * name);
bool ftptSetAuth(const char* name, const char* pw);
bool ftpSetAccount(const char* account);
bool ftpSetInactivityTimeout(uint16_t timeout);
bool ftpSetMode(bool mode);
bool ftpSetPort(uint16_t port);
bool ftpSetSecureOption(bool enable, int8_t profile = -1);
bool ftpGetBufferInfos() {return _bufferInfo;}

bool ftpLogin(uint32_t timeout = 10000);
bool ftpResolveIp(const char* server, char* buffer);
bool ftpLogout();
bool ftpDeleteFile(const char* filename);
bool ftpRenameFile(const char* filename, const char* new_name);
bool ftpRetrieveFile(const char* remote_file, const char* local_file_name,bool retrieve_mode=0);
bool ftpStoreFile(const char* local_file_name, const char * remote_file_name, uint32_t offset);
bool ftpChangeWorkingDirectory(const char* directory_name);
bool ftpCreateDirectory(const char* directory_name);
bool ftpRemoveDirectory(const char* directory_name);
bool ftpGetInfos(const char* file_directory_name= NULL);
bool ftpGetFilesList(const char* directory_name = NULL);




private:

Stream * _diagStream;
OI4GNode* _client;
int8_t    _ftpLoginResult;
char     _bufferInfo[200];

};