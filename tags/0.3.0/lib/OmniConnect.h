#pragma once

#include "EventClient.h"
#include "OmniPacket.h"

class OmniConnect : public event_connect
{   
protected:
	char *mRecvBuffer;
	unsigned int mRecvLen;
	OmniPacket mRecvPacket;

	virtual void Flush();
public: 
	OmniConnect( int one );
	virtual ~OmniConnect();

	void *mArg;

	virtual void ReadCallback( char *buf, size_t len, void *arg );
	virtual void ReadPacket( OmniPacket *packet, char *buf, size_t len );
	virtual void WritePacket( OmniPacket *packet );
	virtual void WritePacket( char *buf, size_t len );
	virtual void WriteCallback( void *arg );
	virtual void ErrorCallback( void *arg );
	virtual void ConnectCallback( void *arg );
	virtual void Reconnect();
};

