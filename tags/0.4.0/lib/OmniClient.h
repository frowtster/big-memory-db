#pragma once

#include "EventClient.h"
#include "OmniPacket.h"

class OmniClient : public event_client
{   
protected:
	char *mRecvBuffer;
	unsigned int mRecvLen;
	OmniPacket mRecvPacket;

	virtual void Flush();
public: 
	OmniClient( int one );
	~OmniClient();
	void *mArg;
	virtual void ReadCallback( char *buf, size_t len, void *arg );
	virtual void ReadPacket( OmniPacket *packet, char *buf, size_t len );
	virtual void WritePacket( OmniPacket *packet );
	virtual void WritePacket( char *buf, size_t len );
	virtual void WriteCallback( void *arg );
	virtual void ErrorCallback( void *arg );
};

