#include <string.h>
#include <strings.h>
#include "HttpReqPacket.h"
#include "Log.h"
extern Log gLog;

HttpReqPacket::HttpReqPacket()
{
	bzero( mBody, BODY_MAX_SIZE );
}

int HttpReqPacket::AddHeader( char *buf )
{
	return 0;
}

int HttpReqPacket::AddHeaderLine( const char *buf )
{
	ONode *node = new ONode( buf );
	mListHeader.insert( node );
	return 0;
}

int HttpReqPacket::AddBody( char *buf )
{
	strcat( mBody, buf );
	return 0;
}

int HttpReqPacket::Parse()
{
	// method
	ONode *node = mListHeader.getHead();
	char *data = node->DataChar();
	char *pos = strchr( data, ' ' );
	int len = pos - data;
	if( len > HEADER_LINE_SIZE )
		return -1;
	strncpy( mMethod, data, len );

	// request URI
	data = pos+1;
	pos = strchr( data, ' ' );
	len = pos - data;
	if( len > HEADER_LINE_SIZE )
		return -1;
	strncpy( mRequestURI, data, len );

	while( node->Next() != NULL )
	{
		node = node->Next();
		data = node->DataChar();
		if( !strncasecmp( data, ContentLength, strlen(ContentLength) ) )
		{
			strcpy( mContentLength, data+strlen(ContentLength) );
		}
		else if( !strncasecmp( data, ContentType, strlen(ContentType) ) )
		{
			strcpy( mContentType, data+strlen(ContentType) );
		}
		else if( !strncasecmp( data, Date, strlen(Date) ) )
		{
			strcpy( mDate, data+strlen(Date) );
		}
		else if( !strncasecmp( data, UserAgent, strlen(UserAgent) ) )
		{
			strcpy( mUserAgent, data+strlen(UserAgent) );
		}
	}

	return 0;
}

void HttpReqPacket::Clear()
{
	mListHeader.clear();

}

char *HttpReqPacket::GetMethod()
{
	return mMethod;
}

char *HttpReqPacket::GetRequestURI()
{
	return mRequestURI;
}

char *HttpReqPacket::GetContentLength()
{
	return mContentLength;
}

char *HttpReqPacket::GetContentType()
{
	return mContentType;
}

char *HttpReqPacket::GetDate()
{
	return mUserAgent;
}

char *HttpReqPacket::GetUserAgent()
{
	return NULL;
}

void HttpReqPacket::printHeader()
{
	mListHeader.printList();
	gLog.log("Method [%s]", mMethod );
	gLog.log("RequestURI [%s]", mRequestURI );
	gLog.log("ContentLength [%s]", mContentLength );
	gLog.log("ContentType [%s]", mContentType );
	gLog.log("Date [%s]", mDate );
	gLog.log("UserAgent [%s]", mUserAgent );
}

char *HttpReqPacket::GetHeader()
{
	return NULL;
}
