#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <assert.h>

#include "EventChannel.h"
#include "ServerManager.h"
#include "ConnectServer.h"
#include "Log.h"
extern Log gLog;

ConnectServer::ConnectServer():OmniConnect(false)
{
}

ConnectServer::~ConnectServer()
{
	gLog.log("ConnectServer destructor");
	TimerThread::DelNode( this );
}

void ConnectServer::ReadPacket( OmniPacket *packet, char *buf, size_t len )
{
	gLog.log("ConnectServer::ReadPacket [%s]", packet->mCmd);
}

void ConnectServer::CloseCallback( void *arg )
{
	gLog.log("ConnectServer::Close by Remote.");

	// manager reconnect
	ServerManager::ResetConnect( this );
}

void ConnectServer::ConnectCallback( void *arg )
{
	gLog.log("ConnectServer::ConnectCallback.");

	// manager reconnect
	ServerManager::Connected( this );

	TimerThread::AddNode( this, 10, 60 );       // ping

	mArg = arg;
}
void ConnectServer::TimerCallback( int mEvent )
{
	gLog.log("TimerCallback.");
	if( mEvent == 10 )
	{
		// send ping
	}
}

void ConnectServer::TableList()
{
	char ret[BUFLEN];
	bzero( ret, BUFLEN );
	sprintf( ret, "LTAB %lu 0 0\r\n", 0l );
	WritePacket( ret, strlen(ret) );
}

void ConnectServer::TableCreate(const char *tname, const char *param)
{
	char ret[BUFLEN];
	bzero( ret, BUFLEN );
	sprintf( ret, "CTAB %lu %s %ld\r\n%s", 0l, tname, strlen(param), param );
	WritePacket( ret, strlen(ret) );
}

void ConnectServer::TableDelete(const char *tname)
{
	char ret[BUFLEN];
	bzero( ret, BUFLEN );
	sprintf( ret, "DTAB %lu %s 0\r\n", 0l, tname );
	WritePacket( ret, strlen(ret) );
}

void ConnectServer::TableUse(const char *tname)
{
	char ret[BUFLEN];
	bzero( ret, BUFLEN );
	sprintf( ret, "UTAB %lu %s 0\r\n", 0l, tname );
	WritePacket( ret, strlen(ret) );
}

void ConnectServer::RowInsert(const char *param)
{
	char ret[BUFLEN];
	bzero( ret, BUFLEN );
	sprintf( ret, "IROW %lu 0 %ld\r\n%s", 0l, strlen(param), param );
	WritePacket( ret, strlen(ret) );
}

void ConnectServer::RowSelect(const char *key, const char *col)
{
	char ret[BUFLEN];
	bzero( ret, BUFLEN );
	sprintf( ret, "SROW %lu %s %s 0\r\n", 0l, key, col );
	WritePacket( ret, strlen(ret) );
}

void ConnectServer::RowUpdate(const char *key, const char *col, const char *val)
{
	char ret[BUFLEN];
	bzero( ret, BUFLEN );
	sprintf( ret, "UROW %lu %s %s %ld\r\n%s", 0l, key, col, strlen(val), val );
	WritePacket( ret, strlen(ret) );
}

void ConnectServer::RowDelete(const char *key)
{
	char ret[BUFLEN];
	bzero( ret, BUFLEN );
	sprintf( ret, "DROW %lu %s 0\r\n", 0l, key );
	WritePacket( ret, strlen(ret) );
}

