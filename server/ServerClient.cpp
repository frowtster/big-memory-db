#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "ServerClient.h"
#include "OmniClient.h"
#include "EventChannel.h"

#include "Table.h"
#include "Log.h"
#include "ErrorCode.h"
#include "Defines.h"

extern Log gLog;

ServerClient::ServerClient( int one ):OmniClient(one)
{   
	bzero( mServerID, NODELEN );
}   
		
void ServerClient::DumpMap()
{           
	map<string, ServerClient*>::iterator iter = mHashMap.begin();
	while( iter != mHashMap.end() )
	{
		gLog.log("[%s][%p]", iter->first.c_str(), iter->second );
		iter++;
	}
	gLog.log("map count %ld", mHashMap.size() );
}

void ServerClient::ReadPacket( OmniPacket *packet, char *buf, size_t len )
{
	gLog.log("ServerClient ReadPacket [%s]", packet->mCmd);
	int errCode = ERROR_OK;

}

void ServerClient::CloseCallback( void *arg )
{
	gLog.log("ServerClient Close by Remote.");
	OmniClient::CloseCallback( arg );
}

map<string, ServerClient*> ServerClient::mHashMap;

