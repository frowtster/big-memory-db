#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "UserClient.h"
#include "OmniClient.h"
#include "EventChannel.h"

#include "Table.h"
#include "Log.h"
#include "ErrorCode.h"

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

	if( !strncmp(packet->mCmd, "LSIN", 4 ) )
	{
		char userid[NODELEN];
		char passwd[NODELEN];

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
	}
	else if( !strncmp(packet->mCmd, "CAPA", 4 ) )
	{
		/*
		{
			Table *tab1, *tab2;
			ColumnInfo colinfo;
			Row row;

			// create table1
			colinfo.AddColumn("col1",1,1);
			colinfo.AddColumn("col2",2,2);
			colinfo.AddColumn("col3",4,5);
			tab1 = Table::CreateTable("table1", &colinfo);
			colinfo.Clear();

			row.AddVal("col1", "1");
			row.AddVal("col2", "12");
			row.AddVal("col3", "432");
			tab1->AddRow( &row );
			row.Clear();
			row.AddVal("col1", "2");
			row.AddVal("col2", "23");
			row.AddVal("col3", "234");
			tab1->AddRow( &row );

			// create table2
			colinfo.AddColumn("col2-1",1,2);
			colinfo.AddColumn("col2-2",4,5);
			tab2 = Table::CreateTable("table2", &colinfo);
			colinfo.Clear();

			row.Clear();
			row.AddVal("col2-1", "1");
			row.AddVal("col2-2", "45");
			tab2->AddRow( &row );
			row.Clear();
			row.AddVal("col2-1", "2");
			row.AddVal("col2-2", "56");
			tab2->AddRow( &row );
		}
		*/
	}
	else if( !strncmp(packet->mCmd, "LOUT", 4 ) )
	{
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
	}
	else if( !strncmp(packet->mCmd, "LTAB", 4 ) )
	{
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
	}
	else if( !strncmp(packet->mCmd, "STAB", 4 ) )
	{
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
	}
	else if( !strncmp(packet->mCmd, "CTAB", 4 ) )
	{
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
			return;
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
		tab = Table::CreateTable( param, &colinfo );
		if( tab == NULL )
			errCode = ERROR_MEMALOCK_FAIL;

		sprintf( ret, "CTAB %lu %d 0\r\n", packet->mTrid, errCode );
		WritePacket( ret, strlen(ret) );
	}
	else if( !strncmp(packet->mCmd, "DTAB", 4 ) )
	{
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
			return;
		}

		table = param;
		Table::DeleteTable( table );
		sprintf( ret, "DTAB %lu %d 0\r\n", packet->mTrid, errCode );
		WritePacket( ret, strlen(ret) );
	}
	else if( !strncmp(packet->mCmd, "UTAB", 4 ) )
	{
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
			return;
		}

		mTable = table;

		sprintf( ret, "UTAB %lu %d 0\r\n", packet->mTrid, errCode );
		WritePacket( ret, strlen(ret) );
	}
	else if( !strncmp(packet->mCmd, "IROW", 4 ) )
	{
		char ret[BUFLEN];
		char param[NODELEN];

		PARSE_TAG_INIT( packet->mHeader );
		PARSE_TAG_NEXT( packet->mHeader, param );
		gLog.log("Insert Row timeout %s", param );

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
			return;
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
		mTable->AddRow( &row, atoi(param) );
		Table::Dump();

		sprintf( ret, "IROW %lu %d 0\r\n", packet->mTrid, errCode );
		WritePacket( ret, strlen(ret) );
	}
	else if( !strncmp(packet->mCmd, "SROW", 4 ) )
	{
		char ret[BUFLEN];
		char param1[NODELEN];
		char param2[NODELEN];
		char *value;

		if( mTable == NULL )
		{
			errCode = ERROR_NOT_SELECT_TABLE;
			sprintf( ret, "IROW %lu %d 0\r\n", packet->mTrid, errCode );
			WritePacket( ret, strlen(ret) );
			return;
		}

		PARSE_TAG_INIT( packet->mHeader );
		PARSE_TAG_NEXT( packet->mHeader, param1 );
		PARSE_TAG_NEXT( packet->mHeader, param2 );
		gLog.log("Select Row" );

		value = mTable->GetRow( param1, param2 );

		sprintf( ret, "SROW %lu %d %lu\r\n", packet->mTrid, errCode, strlen(value) );
		WritePacket( ret, strlen(ret) );
		WritePacket( value, strlen(value) );
	}
	else if( !strncmp(packet->mCmd, "UROW", 4 ) )
	{
		char ret[BUFLEN];
		char param1[NODELEN];
		char param2[NODELEN];

		if( mTable == NULL )
		{
			errCode = ERROR_NOT_SELECT_TABLE;
			sprintf( ret, "IROW %lu %d 0\r\n", packet->mTrid, errCode );
			WritePacket( ret, strlen(ret) );
			return;
		}

		PARSE_TAG_INIT( packet->mHeader );
		PARSE_TAG_NEXT( packet->mHeader, param1 );
		PARSE_TAG_NEXT( packet->mHeader, param2 );
		gLog.log("Update Row [%s][%s][%s]", param1, param2, packet->mBody );

		mTable->UpdateRow( param1, param2, packet->mBody );

		Table::Dump();

		sprintf( ret, "UROW %lu %d 0\r\n", packet->mTrid, errCode );
		WritePacket( ret, strlen(ret) );
	}
	else if( !strncmp(packet->mCmd, "DROW", 4 ) )
	{
		char ret[BUFLEN];
		char param[NODELEN];

		if( mTable == NULL )
		{
			errCode = ERROR_NOT_SELECT_TABLE;
			sprintf( ret, "IROW %lu %d 0\r\n", packet->mTrid, errCode );
			WritePacket( ret, strlen(ret) );
			return;
		}

		PARSE_TAG_INIT( packet->mHeader );
		PARSE_TAG_NEXT( packet->mHeader, param );
		gLog.log("Delete Row" );

		mTable->DelRow( param );
		Table::Dump();

		sprintf( ret, "DROW %lu %d 0\r\n", packet->mTrid, errCode );
		WritePacket( ret, strlen(ret) );
	}
	else
	{
		gLog.log("error. invalid protocol");
		char ret[BUFLEN];
		errCode = ERROR_INVALID_COMMAND;
		sprintf( ret, "%s %lu %d 0\r\n", packet->mCmd, packet->mTrid, errCode );
		WritePacket( ret, strlen(ret) );
	}
}
void UserClient::CloseCallback( void *arg )
{
	gLog.log("UserClient Close by Remote.");
	OmniClient::CloseCallback( arg );
}

map<string, UserClient*> UserClient::mHashMap;

