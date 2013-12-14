#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "EventMain.h"
#include "TimerThread.h"

#include "Version.h"
#include "ServerManager.h"
#include "ConnectServer.h"
#include "Log.h"

#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT 9090

Log gLog;

class MyManager : public ServerManager
{
};

class MyServer : public ConnectServer
{
    virtual void ReadPacket( OmniPacket *packet, char *buf, size_t len )
	{
		if( !strncmp( packet->mCmd, "CTAB", 4 ) )
		{
			TableUse("table1");
		}
		else if( !strncmp( packet->mCmd, "UTAB", 4 ) )
		{
			RowInsert("col1 1\r\ncol2 234\r\n");
		}
		else if( !strncmp( packet->mCmd, "IROW", 4 ) )
		{
			RowUpdate("1","col2","123");
		}
		else if( !strncmp( packet->mCmd, "UROW", 4 ) )
		{
			RowSelect("1","col2");
		}
		else if( !strncmp( packet->mCmd, "SROW", 4 ) )
		{
			assert( !strncmp(packet->mBody, "123", 3 ) );
			RowDelete("1");
		}
	}
    virtual void ConnectCallback( void *arg )
	{
		ConnectServer::ConnectCallback( arg );
		TableCreate("table1", "col1 1 2\r\ncol2 1 5\r\n");
	}
};

class MyFactory : ServerFactory
{
public:
    static ConnectServer* createInstance()
    {
        gLog.log("create instance" );
        return new MyServer;
    }
};

int main()
{
	printf("Start Big Memory client.\n");
	printf("Version : %s\n", BMDB_VERSION);
	gLog.init( "log", "client", Log::REOPEN_DD, Log::LEVEL_TRACE );

	// init server
	printEventBase();
	printf("Connect to %s:%d.\n", SERVER_HOST, SERVER_PORT );

	// int timer
	TimerThread::CreateInstance();
	
	// connect to server
	MyManager::GetInstance()->Add( SERVER_HOST, SERVER_PORT, MyFactory::createInstance );

	// loop
	EventDispatch();

	// finalize
	EventClose();

	printf("End Big Memory DB.\n");

	return 0;
}

