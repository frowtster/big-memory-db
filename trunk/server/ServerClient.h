#include <map>
#include <string>

#include "OmniClient.h"

using namespace std;

class Table;

class ServerClient : public OmniClient
{
	Table *mTable;
protected:
	static map<string, ServerClient*> mHashMap;

	char mServerID[NODELEN];

public: 
    ServerClient( int one );
    void DumpMap();
    virtual void ReadPacket( OmniPacket *packet, char *buf, size_t len );
    virtual void CloseCallback( void *arg );

};

