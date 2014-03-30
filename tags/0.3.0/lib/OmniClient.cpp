#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "OmniClient.h"
#include "EventMain.h"
#include "util.h"
#include "Log.h"
extern Log gLog;

OmniClient::OmniClient( int one ):event_client(one)
{
	mRecvLen = 0;
	mRecvBuffer = (char*)malloc(BUFLEN);
}
OmniClient::~OmniClient()
{
	free( mRecvBuffer );
}

void OmniClient::Flush()
{
	gLog.log("flush[%s][%d]", mRecvBuffer, mRecvLen );
	// line flush
	if( mRecvPacket.mbNewLineOK == TRUE )
	{
		int newlinesize = NewLineSize(mRecvBuffer);
//		bzero( mRecvBuffer, mRecvLen );
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

void OmniClient::ReadCallback( char *buf, size_t len, void *arg )
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

void OmniClient::ReadPacket( OmniPacket *packet, char *buf, size_t len )
{
}

void OmniClient::WritePacket( OmniPacket *packet )
{
	packet->Serialize();

	EventWrite( mArg, packet->mBuffer, strlen(packet->mBuffer) );
}

void OmniClient::WritePacket( char *buf, size_t len )
{
	EventWrite( mArg, buf, len );
}

void OmniClient::WriteCallback( void *arg )
{
	gLog.log("write callback");
}
void OmniClient::ErrorCallback( void *arg )
{
	gLog.log("error callback. logout.");
}

