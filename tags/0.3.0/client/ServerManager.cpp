#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <list>

#include "EventChannel.h"
#include "ServerManager.h"
#include "Log.h"

using namespace std;

extern Log gLog;

list<struct ServerInfo*> gServerList;

ServerManager::ServerManager() 
{
	gLog.log("ServerManager create %p", this );
	TimerThread::AddNode( this, 10, 2 );
}

ServerManager *ServerManager::GetInstance()
{
	static ServerManager *manager = NULL;
	if( manager == NULL )
		manager = new ServerManager;
	return manager;
}

void ServerManager::Add(const char *host, int port, ConnectServer *(*param)())
{
	ServerInfo *pServer = new ServerInfo;
	strcpy( pServer->mHost, host );
	pServer->mPort = port;
	pServer->mCreate = param;
	pServer->mConnected = false;
	gServerList.push_back( pServer );
}

void ServerManager::ResetConnect( ConnectServer *connect )
{
	struct ServerInfo *server;
	list<struct ServerInfo*>::iterator ilist;
	for( ilist = gServerList.begin(); ilist != gServerList.end(); ilist++ )
	{
		server = (*ilist);
		if( server->mConnect == connect )
		{
			server->mConnected = false;
			break;
		}
	}
}

void ServerManager::Connected( ConnectServer *connect )
{
	struct ServerInfo *server;
	list<struct ServerInfo*>::iterator ilist;
	for( ilist = gServerList.begin(); ilist != gServerList.end(); ilist++ )
	{
		server = (*ilist);
		if( server->mConnect == connect )
		{
			server->mConnected = true;
			break;
		}
	}
}

void ServerManager::TimerCallback( int mEvent )
{
	struct ServerInfo *pServer;
	list<struct ServerInfo*>::iterator ilist;
	for( ilist = gServerList.begin(); ilist != gServerList.end(); ilist++ )
	{
		pServer = (*ilist);
		if( pServer->mConnected == false )
		{
			pServer->mConnect = pServer->mCreate();
			if( EventChannel::Connect( pServer->mHost, pServer->mPort, (event_connect *)pServer->mConnect ) == false )
				;
		}
	}
}

