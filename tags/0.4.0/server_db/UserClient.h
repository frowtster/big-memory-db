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

	int _CmdERROR( OmniPacket *packet );
	int _CmdLSIN( OmniPacket *packet );
	int _CmdLOUT( OmniPacket *packet );
	int _CmdCAPA( OmniPacket *packet );
	int _CmdLTAB( OmniPacket *packet );
	int _CmdSTAB( OmniPacket *packet );
	int _CmdCTAB( OmniPacket *packet );
	int _CmdDTAB( OmniPacket *packet );
	int _CmdUTAB( OmniPacket *packet );
	int _CmdIROW( OmniPacket *packet );
	int _CmdSROW( OmniPacket *packet );
	int _CmdUROW( OmniPacket *packet );
	int _CmdDROW( OmniPacket *packet );
	int _CmdBKUP( OmniPacket *packet );
	int _CmdRSTO( OmniPacket *packet );

};

