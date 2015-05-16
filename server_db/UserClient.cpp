#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "UserClient.h"
#include "OmniClient.h"
#include "EventChannel.h"

#include "Table.h"
#include "Log.h"
#include "ErrorCode.h"
#include "Defines.h"

extern Log gLog;

UserClient::UserClient( int one ):OmniClient(one)
{   
	bzero( mUserID, NODELEN );
}   
		
void UserClient::DumpMap()
{           
	map<string, UserClient*>::iterator iter = mHashMap.begin();
	while( iter != mHashMap.end() )
	{
		gLog.log("[%s][%p]", iter->first.c_str(), iter->second );
		iter++;
	}
	gLog.log("map count %ld", mHashMap.size() );
}

void UserClient::ReadPacket( OmniPacket *packet, char *buf, size_t len )
{
	gLog.log("UserClient ReadPacket [%s]", packet->mCmd);
	int errCode = ERROR_OK;

	DISPATCH_COMMAND_BEGIN( _CmdERROR );
	DISPATCH_COMMAND_NEXT( "LSIN", _CmdLSIN );
	DISPATCH_COMMAND_NEXT( "LOUT", _CmdLOUT );
	DISPATCH_COMMAND_NEXT( "CAPA", _CmdCAPA );
	DISPATCH_COMMAND_NEXT( "LTAB", _CmdLTAB );
	DISPATCH_COMMAND_NEXT( "STAB", _CmdSTAB );
	DISPATCH_COMMAND_NEXT( "CTAB", _CmdCTAB );
	DISPATCH_COMMAND_NEXT( "DTAB", _CmdDTAB );
	DISPATCH_COMMAND_NEXT( "UTAB", _CmdUTAB );
	DISPATCH_COMMAND_NEXT( "IROW", _CmdIROW );
	DISPATCH_COMMAND_NEXT( "SROW", _CmdSROW );
	DISPATCH_COMMAND_NEXT( "UROW", _CmdUROW );
	DISPATCH_COMMAND_NEXT( "DROW", _CmdDROW );
	DISPATCH_COMMAND_NEXT( "BKUP", _CmdBKUP );
	DISPATCH_COMMAND_NEXT( "RSTO", _CmdRSTO );
	DISPATCH_COMMAND_END( _CmdERROR );

}

void UserClient::CloseCallback( void *arg )
{
	gLog.log("UserClient Close by Remote.");
	OmniClient::CloseCallback( arg );
}

map<string, UserClient*> UserClient::mHashMap;

int UserClient::_CmdERROR( OmniPacket *packet )
{
	gLog.log("error. invalid protocol");
	int errCode = ERROR_OK;
	char ret[BUFLEN];
	errCode = ERROR_INVALID_COMMAND;
	sprintf( ret, "%s %lu %d 0\r\n", packet->mCmd, packet->mTrid, errCode );
	WritePacket( ret, strlen(ret) );
	return errCode;
}

int UserClient::_CmdLSIN( OmniPacket *packet )
{
	char userid[NODELEN];
	char passwd[NODELEN];
	int errCode = ERROR_OK;

	PARSE_TAG_INIT( packet->mHeader );
	PARSE_TAG_NEXT( packet->mHeader, userid );
	PARSE_TAG_NEXT( packet->mHeader, passwd );
	gLog.log("userid[%s] passwd[%s]", userid, passwd );

	if( errCode == 0 )
	{
		strcpy( mUserID, userid );
		mHashMap.insert( pair<const char*, UserClient*>(userid, this) );
	}
	DumpMap();

	char ret[BUFLEN];
	bzero( ret, BUFLEN );
	sprintf( ret, "LSIN %lu %d 0\r\n", packet->mTrid, errCode );
	WritePacket( ret, strlen(ret) );
	return errCode;
}

int UserClient::_CmdLOUT( OmniPacket *packet )
{
	int errCode = ERROR_OK;
	map<string, UserClient*>::iterator iter = mHashMap.find( mUserID );
	if( iter == mHashMap.end() )
	{
		gLog.log("No user info");
	}
	else
	{
		mHashMap.erase( iter );
	}

	DumpMap();

	char ret[BUFLEN];
	bzero( ret, BUFLEN );
	sprintf( ret, "LOUT %lu %d 0\r\n", packet->mTrid, errCode );
	WritePacket( ret, strlen(ret) );

	EventChannel::EventCallback( (struct bufferevent*)buffer_event, 0, this );
	return errCode;
}

int UserClient::_CmdCAPA( OmniPacket *packet )
{
	int errCode = ERROR_OK;
	return errCode;
}

int UserClient::_CmdLTAB( OmniPacket *packet )
{
	int errCode = ERROR_OK;
	char ret[BUFLEN];
	bzero( ret, BUFLEN );
	char body[BUFLEN*10];
	bzero( body, BUFLEN*10 );

	gLog.log("List Table" );

	Table *tab;
	map<string, Table*>::iterator iter = Table::mTableMap.begin();
	while( iter != Table::mTableMap.end() )
	{
		tab = iter->second;
		strcat( body, iter->first.c_str() );
		strcat( body, "\r\n");
		iter ++;
	}

	sprintf( ret, "LTAB %lu %d %lu\r\n", packet->mTrid, errCode, strlen(body) );
	WritePacket( ret, strlen(ret) );
	WritePacket( body, strlen(body) );
	return errCode;
}

int UserClient::_CmdSTAB( OmniPacket *packet )
{
	int errCode = ERROR_OK;
	char ret[BUFLEN];
	bzero( ret, BUFLEN );
	char body[BUFLEN*10];
	bzero( body, BUFLEN*10 );
	char param[NODELEN];
	string table;

	PARSE_TAG_INIT( packet->mHeader );
	PARSE_TAG_NEXT( packet->mHeader, param );
	gLog.log("Show Table [%s]", param );

	table = param;
	Table *tab;
	Column *col;
	map<string, Table*>::iterator iter = Table::mTableMap.find(table);
	if( iter == Table::mTableMap.end() )
	{
		errCode = ERROR_TABLE_NOT_FOUND;
		sprintf( ret, "STAB %lu %d 0\r\n", packet->mTrid, errCode );
		WritePacket( ret, strlen(ret) );
	}
	else
	{
		tab = iter->second;
		list<Column*>::iterator iter2 = tab->mColInfo.mColList.begin();
		while( iter2 != tab->mColInfo.mColList.end() )
		{
			col = *iter2;
			sprintf( ret, "%s %d %d\r\n", col->mColName, col->mType, col->mSize );
			iter2++;
			strcat( body, ret );
		}
		sprintf( ret, "STAB %lu %d %lu\r\n", packet->mTrid, errCode, strlen(body) );
		WritePacket( ret, strlen(ret) );
		WritePacket( body, strlen(body) );
	}
	return errCode;
}

int UserClient::_CmdCTAB( OmniPacket *packet )
{
	int errCode = ERROR_OK;
	char ret[BUFLEN];
	char param[NODELEN];
	string table;
	Table *tab;

	PARSE_TAG_INIT( packet->mHeader );
	PARSE_TAG_NEXT( packet->mHeader, param );
	gLog.log("Create Table [%s]", param );

	table = param;
	gLog.log("[%s]", packet->mBody );

	tab = Table::GetTable( param );
	if( tab != NULL )
	{
		errCode = ERROR_TABLE_EXIST;
		sprintf( ret, "CTAB %lu %d 0\r\n", packet->mTrid, errCode );
		WritePacket( ret, strlen(ret) );
		return errCode;
	}

	ColumnInfo colinfo;
	char *buf = packet->mBody;
	char *next;
	char colname[BUFLEN];
	int coltype;
	int collen;

	while( true )
	{
		bzero( ret, BUFLEN );
		next = strstr( buf, "\r\n" );
		if( next == NULL )
			break;
		if( (next-buf) >= BUFLEN )
			break;
		if( (next-buf) < 0 )
			break;

		sscanf( buf, "%s %d %d", colname, &coltype, &collen );
		gLog.log("[%s] [%d] [%d]", colname, coltype, collen );
		colinfo.AddColumn( colname, coltype, collen );
		buf = next+2;
	}
	tab = Table::CreateTable( param, TYPE_MULTI_COLUMN );
	tab->SetColumn( &colinfo );
	if( tab == NULL )
		errCode = ERROR_MEMALOCK_FAIL;

	sprintf( ret, "CTAB %lu %d 0\r\n", packet->mTrid, errCode );
	WritePacket( ret, strlen(ret) );
	return errCode;
}

int UserClient::_CmdDTAB( OmniPacket *packet )
{
	int errCode = ERROR_OK;
	char ret[BUFLEN];
	bzero( ret, BUFLEN );
	char param[NODELEN];
	string table;
	Table *tab;

	PARSE_TAG_INIT( packet->mHeader );
	PARSE_TAG_NEXT( packet->mHeader, param );
	gLog.log("Delete Table [%s]", param );

	tab = Table::GetTable( param );
	if( tab == NULL )
	{
		errCode = ERROR_TABLE_NOT_FOUND;
		sprintf( ret, "DTAB %lu %d 0\r\n", packet->mTrid, errCode );
		WritePacket( ret, strlen(ret) );
		return errCode;
	}

	table = param;
	Table::DeleteTable( table );
	sprintf( ret, "DTAB %lu %d 0\r\n", packet->mTrid, errCode );
	WritePacket( ret, strlen(ret) );
	return errCode;
}

int UserClient::_CmdUTAB( OmniPacket *packet )
{
	int errCode = ERROR_OK;
	char ret[BUFLEN];
	bzero( ret, BUFLEN );
	char param[NODELEN];
	Table *table;

	PARSE_TAG_INIT( packet->mHeader );
	PARSE_TAG_NEXT( packet->mHeader, param );
	gLog.log("Use Table [%s]", param );

	table = Table::GetTable( param );
	if( table == NULL )
	{
		errCode = ERROR_TABLE_NOT_FOUND;
		sprintf( ret, "UTAB %lu %d 0\r\n", packet->mTrid, errCode );
		WritePacket( ret, strlen(ret) );
		return errCode;
	}

	mTable = table;

	sprintf( ret, "UTAB %lu %d 0\r\n", packet->mTrid, errCode );
	WritePacket( ret, strlen(ret) );
	return errCode;
}

int UserClient::_CmdIROW( OmniPacket *packet )
{
	int errCode = ERROR_OK;
	char ret[BUFLEN];
	char param1[NODELEN];
	char param2[NODELEN];

	PARSE_TAG_INIT( packet->mHeader );
	PARSE_TAG_NEXT( packet->mHeader, param1 );
	PARSE_TAG_NEXT( packet->mHeader, param2 );
	gLog.log("Insert Row key:%s timeout %s", param1, param2 );

	Row row;
	char *buf = packet->mBody;
	char *next;
	char colname[BUFLEN];
	char colvalue[BUFLEN];

	if( mTable == NULL )
	{
		errCode = ERROR_NOT_SELECT_TABLE;
		sprintf( ret, "IROW %lu %d 0\r\n", packet->mTrid, errCode );
		WritePacket( ret, strlen(ret) );
		return errCode;
	}

	while( true )
	{
		bzero( ret, BUFLEN );
		next = strstr( buf, "\r\n" );
		if( next == NULL )
			break;
		if( (next-buf) >= BUFLEN )
			break;
		if( (next-buf) < 0 )
			break;

		sscanf( buf, "%s %s", colname, colvalue );
		gLog.log("[%s] [%s]", colname, colvalue );
		row.AddVal( colname, colvalue );
		buf = next+2;
	}
	mTable->AddRow( param1, &row, atoi(param2) );
	Table::Dump();

	sprintf( ret, "IROW %lu %d 0\r\n", packet->mTrid, errCode );
	WritePacket( ret, strlen(ret) );

	return errCode;
}

int UserClient::_CmdSROW( OmniPacket *packet )
{
	int errCode = ERROR_OK;
	char ret[BUFLEN];
	char param1[NODELEN];
	char param2[NODELEN];
	char *value;

	if( mTable == NULL )
	{
		errCode = ERROR_NOT_SELECT_TABLE;
		sprintf( ret, "IROW %lu %d 0\r\n", packet->mTrid, errCode );
		WritePacket( ret, strlen(ret) );
		return errCode;
	}

	PARSE_TAG_INIT( packet->mHeader );
	PARSE_TAG_NEXT( packet->mHeader, param1 );
	PARSE_TAG_NEXT( packet->mHeader, param2 );
	gLog.log("Select Row" );

	mTable->GetRow( param1, param2, value );

	sprintf( ret, "SROW %lu %d %lu\r\n", packet->mTrid, errCode, strlen(value) );
	WritePacket( ret, strlen(ret) );
	WritePacket( value, strlen(value) );

	return errCode;
}

int UserClient::_CmdUROW( OmniPacket *packet )
{
	int errCode = ERROR_OK;
	char ret[BUFLEN];
	char param1[NODELEN];
	char param2[NODELEN];

	if( mTable == NULL )
	{
		errCode = ERROR_NOT_SELECT_TABLE;
		sprintf( ret, "IROW %lu %d 0\r\n", packet->mTrid, errCode );
		WritePacket( ret, strlen(ret) );
		return errCode;
	}

	PARSE_TAG_INIT( packet->mHeader );
	PARSE_TAG_NEXT( packet->mHeader, param1 );
	PARSE_TAG_NEXT( packet->mHeader, param2 );
	gLog.log("Update Row [%s][%s][%s]", param1, param2, packet->mBody );

	mTable->UpdateRow( param1, param2, packet->mBody );

	Table::Dump();

	sprintf( ret, "UROW %lu %d 0\r\n", packet->mTrid, errCode );
	WritePacket( ret, strlen(ret) );

	return errCode;
}

int UserClient::_CmdDROW( OmniPacket *packet )
{
	int errCode = ERROR_OK;
	char ret[BUFLEN];
	char param[NODELEN];

	if( mTable == NULL )
	{
		errCode = ERROR_NOT_SELECT_TABLE;
		sprintf( ret, "IROW %lu %d 0\r\n", packet->mTrid, errCode );
		WritePacket( ret, strlen(ret) );
		return errCode;
	}

	PARSE_TAG_INIT( packet->mHeader );
	PARSE_TAG_NEXT( packet->mHeader, param );
	gLog.log("Delete Row" );

	mTable->DelRow( param );
	Table::Dump();

	sprintf( ret, "DROW %lu %d 0\r\n", packet->mTrid, errCode );
	WritePacket( ret, strlen(ret) );

	return errCode;
}

int UserClient::_CmdBKUP( OmniPacket *packet )
{
	int errCode = ERROR_OK;
	char ret[BUFLEN];
	bzero( ret, BUFLEN );
	char param[NODELEN];
	string table;
	Table *tab;
	char filename[256];

	PARSE_TAG_INIT( packet->mHeader );
	PARSE_TAG_NEXT( packet->mHeader, param );
	gLog.log("Backup Table [%s]", param );

	tab = Table::GetTable( param );
	if( tab == NULL )
	{
		errCode = ERROR_TABLE_NOT_FOUND;
		sprintf( ret, "BKUP %lu %d NULL 0\r\n", packet->mTrid, errCode );
		WritePacket( ret, strlen(ret) );
		return errCode;
	}

	table = param;
	errCode = Table::Backup( table, filename );
	sprintf( ret, "BKUP %lu %d %s 0\r\n", packet->mTrid, errCode, filename );
	WritePacket( ret, strlen(ret) );

	return errCode;
}

int UserClient::_CmdRSTO( OmniPacket *packet )
{
	int errCode = ERROR_OK;
	char ret[BUFLEN];
	bzero( ret, BUFLEN );
	char param[NODELEN];

	PARSE_TAG_INIT( packet->mHeader );
	PARSE_TAG_NEXT( packet->mHeader, param );
	gLog.log("Restore Table [%s]", param );

	errCode = Table::Restore( param );
	sprintf( ret, "RSTO %lu %d 0\r\n", packet->mTrid, errCode );
	WritePacket( ret, strlen(ret) );

	return errCode;
}

