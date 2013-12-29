#include <map>
#include <string>

#include "OmniClient.h"

using namespace std;

class Table;

class UserClient : public OmniClient
{
	Table *mTable;
protected:
	static map<string, UserClient*> mHashMap;

	char mUserID[NODELEN];
	unsigned long mUserCode;

public: 
    UserClient( int one );
    void DumpMap();
    virtual void ReadPacket( OmniPacket *packet, char *buf, size_t len );
    virtual void CloseCallback( void *arg );
};

