#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <event2/event.h>

#include "OmniConnect.h"
#include "EventMain.h"
#include "EventChannel.h"
#include "util.h"
#include "Log.h"
extern Log gLog;

OmniConnect::OmniConnect( int one ):event_connect(one)
{
	mRecvLen = 0;
	mRecvBuffer = (char*)malloc(BUFLEN);
}
OmniConnect::~OmniConnect()
{
	free( mRecvBuffer );
}

void OmniConnect::Flush()
{
	gLog.log("flush[%s][%d]", mRecvBuffer, mRecvLen );
	// line flush
	if( mRecvPacket.mbNewLineOK == TRUE )
	{
		int newlinesize = NewLineSize(mRecvBuffer);
		bzero( mRecvBuffer, mRecvLen );
		mRecvLen -= (mRecvPacket.mHeaderLen + newlinesize);
		if( mRecvLen > 0 )
		{
			memmove( mRecvBuffer, mRecvBuffer + mRecvPacket.mHeaderLen + newlinesize,
				mRecvLen );
		}
	}
    // body flush
    // todo : store recv state and casing.
    if( mRecvPacket.mBodySize > 0 )
    {
        if(mRecvLen >= mRecvPacket.mBodySize )
        {
            mRecvLen -= mRecvPacket.mBodySize;
            if( mRecvLen > 0 )
            {
                memmove( mRecvBuffer, mRecvBuffer + mRecvPacket.mBodySize, mRecvLen );
            }
        }
    }

	gLog.log("flush[%s][%d]", mRecvBuffer, mRecvLen );
}

void OmniConnect::ReadCallback( char *buf, size_t len, void *arg )
{
	gLog.log("read callback");
	mArg = arg;

	if( (mRecvLen+len) >= BUFLEN )
	{
		gLog.log("buffer overflow");
		return;
	}
	memcpy( mRecvBuffer+mRecvLen, buf, len );
	mRecvLen += len;
	*(mRecvBuffer+mRecvLen) = '\0';

	mRecvPacket.mBuffer = mRecvBuffer;

	// not yet receive
	if( mRecvPacket.Parse() != TRUE )
	{
		gLog.log("parse fail");
		Flush();
		return;
	}
	ReadPacket( &mRecvPacket, buf, len );
	Flush();
}

void OmniConnect::ReadPacket( OmniPacket *packet, char *buf, size_t len )
{
}

void OmniConnect::WritePacket( OmniPacket *packet )
{
	packet->Serialize();

	EventWrite( mArg, packet->mBuffer, strlen(packet->mBuffer) );
}

void OmniConnect::WritePacket( char *buf, size_t len )
{
	EventWrite( mArg, buf, len );
}

void OmniConnect::WriteCallback( void *arg )
{
//	gLog.log("write callback");
}
void OmniConnect::ErrorCallback( void *arg )
{
//	gLog.log("error callback. logout.");
}

void OmniConnect::ConnectCallback( void *arg )
{
//	gLog.log("connect callback. logout.");
}

void OmniConnect::Reconnect()
{
	EventChannel::Connect( mHost, mPort, this );
}

