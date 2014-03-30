#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "Defines.h"
#include "OmniPacket.h"
#include "Log.h"
extern Log gLog;

OmniPacket::OmniPacket()
{
	mBuffer = (char*)malloc(BUFLEN);
	bzero( mCmd, CMDLEN+1 );
	mBody = NULL;
}

OmniPacket::~OmniPacket()
{
	if( mBody != NULL )
	{
		free( mBody );
	}
}

int OmniPacket::Parse()
{
	char *tagTrid, *tagSize;

	Clear();
	gLog.log("mBuffer[%s]", mBuffer );

	// search newline
	char *newline;
	newline = strchr( mBuffer, '\r' );
	if( newline == NULL )
	{
		newline = strchr( mBuffer, '\n' );
		if( newline == NULL )
		{
			gLog.log("no new line");
			return FALSE;
		}
	}
	mHeaderLen = newline-mBuffer;
	mbNewLineOK = TRUE;

	// search cmd
	if( strlen( mBuffer ) <= CMDLEN )
	{
		gLog.log("length under 4");
		return FALSE;
	}
	memcpy( mCmd, mBuffer, CMDLEN );

	// search trid
	char trid[TRIDLEN];
	bzero( trid, TRIDLEN );
	tagTrid = strchr( mBuffer+CMDLEN+1, ' ' );
	if( tagTrid == NULL )
	{
		gLog.log("no trid");
		return FALSE;
	}
	mTridLen = tagTrid-mBuffer-CMDLEN-1;
	memcpy( trid, mBuffer+CMDLEN+1, mTridLen );
	mTrid = atol( trid );

	// search length
	mBodySize = -1;
	char length[TRIDLEN];
	bzero( length, TRIDLEN );
	for( int i=2; i< TRIDLEN; i++ )
	{
		if( (newline-i) <= tagTrid )
		{
			gLog.log("no size");
			return FALSE;
		}
		if( *(newline-i) == ' ' )
		{
			mBodySize = atol( newline-i+1 );
			tagSize = newline-i;
			mSizeLen = i-1;
			break;
		}
	}
	if( mBodySize == -1 )
	{
		gLog.log("no size");
		return FALSE;
	}
	if( mBodySize > 0 )
	{
		mBody = (char*)malloc(mBodySize+1);
		memcpy( mBody, newline+2, mBodySize );
		mBody[mBodySize] = '\0';
	}

	// search header
	memcpy( mHeader, tagTrid+1, mHeaderLen-mTridLen-mSizeLen-CMDLEN-3 );
	return TRUE;
}

int OmniPacket::Serialize()
{
	bzero( mBuffer, BUFLEN );
	sprintf( mBuffer, "%s %lu %s %lu\r\n",
			mCmd, mTrid, mHeader, mBodySize );
	return TRUE;
}

void OmniPacket::Clear()
{
	bzero( mCmd, CMDLEN );
	bzero( mHeader, BUFLEN );
	mTrid = -1;
	mBodySize = -1;
	mHeaderLen = -1;
	mTridLen = -1;
	mSizeLen = -1;
	mbNewLineOK = FALSE;
}
