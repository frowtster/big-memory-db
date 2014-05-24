#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

#include <map>
#include <string>

#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include "UserClient.h"
#include "EventFactory.h"
#include "EventMain.h"

#include "Table.h"
#include "Version.h"
#include "Log.h"
#include "FileIni.h"
#include "TimeoutThread.h"

using namespace std;
int gUseSwap;
Log gLog;
FileIni inifile;

int serviceport, serverport, manageport;

class UserFactory : public EventFactory
{
	public:
		static event_client* createInstance( int port )
		{
			gLog.log("create instance [%d]", port );
			if( port == serviceport )
				return new UserClient(false);
			return NULL;
		}
};

map<string, Table*> Table::mTableMap;

int main()
{
	char value[100];
	FileIni::GetPrivateProfileStr( "GLOBAL", "SERVER_PORT", "8080", value, 100, "./server.ini" );
	serverport = atoi(value);
	FileIni::GetPrivateProfileStr( "GLOBAL", "MANAGE_PORT", "8081", value, 100, "./server.ini" );
	manageport = atoi(value);
	FileIni::GetPrivateProfileStr( "GLOBAL", "USE_SWAP", "0", value, 100, "./server.ini" );
	gUseSwap = atoi(value);

	printf("Start Big Memory DB.\n");
	printf("Version : %s\n", BMDB_VERSION);
	gLog.init( "log", "BMDB", Log::REOPEN_DD, Log::LEVEL_TRACE );

	// create timeout thread
	TimeoutThread::CreateInstance();

	// init server
	srandom( time(NULL) );
	printEventBase();
	printf("Listen port is %d.\n", serverport );
	printf("Manage port is %d.\n", manageport );

	// init event
	EventInit(UserFactory::createInstance);
	EventAdd( serverport );
	EventAdd( manageport );

	// loop
	EventDispatch();

	// finalize
	EventClose();

	printf("End Big Memory DB.\n");

	return 0;
}

