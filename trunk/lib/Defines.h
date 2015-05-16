#pragma once

#ifndef TRUE
#define TRUE -1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define BUFLEN 1024
#define CMDLEN 4
#define TRIDLEN 10
#define HOSTLEN 128
#define NODELEN 256
#define QUERYLEN 512
#define CHATKEYLEN 10

#define TYPE_SINGLE_COLUMN	0x0
#define TYPE_MULTI_COLUMN	0x1

#define TYPE_SINGLE_VALUE	0x00
#define TYPE_MULTI_VALUE	0x10

#define TYPE_STRING	0x000
#define TYPE_NUMBER	0x100

#define TYPE_SELECT_NORMAL		0x0
#define TYPE_SELECT_CONTINUE	0x1
#define TYPE_SELECT_ROUND		0x2

#define PARSE_TAG_INIT( buffer )	\
	int nPos = 0;					\
	char *cTag;						\
	int taglen;						\
	int headlen = strlen( buffer );	

#define PARSE_TAG_NEXT( buffer, cNode )				\
do													\
{													\
	if( nPos >= headlen )							\
		break;										\
	cTag = strchr( buffer+nPos, ' ' );				\
	if( cTag == NULL )								\
	{												\
		cTag = strchr( buffer+nPos, '\r' );			\
		if( cTag == NULL )							\
		{											\
			cTag = strchr( buffer+nPos, '\n' );		\
			if( cTag == NULL );						\
			{										\
				cTag = strchr( buffer+nPos, '\0' );	\
				if( cTag == NULL )					\
					break;							\
			}										\
		}											\
	}												\
	taglen = cTag-(buffer+nPos);					\
	strncpy( cNode, buffer+nPos, taglen );			\
	*(cNode+taglen) = '\0';							\
	nPos += (taglen+1);								\
} while(0)

#define DISPATCH_COMMAND_BEGIN( func )				\
	if( packet->mCmd[0] == '\0' )	errCode = func( packet )
#define DISPATCH_COMMAND_NEXT( cmd, func )			\
	else if( !strncmp(packet->mCmd, cmd, 4 ) )	errCode = func( packet )
#define DISPATCH_COMMAND_END( func )				\
	else errCode = func( packet )
