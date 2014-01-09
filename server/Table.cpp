#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <stdint.h>

#include <map>
#include <list>

#include "Table.h"
#include "Log.h"
#include "ErrorCode.h"
#include "FileIni.h"
#include "util.h"
#include "TimeoutThread.h"

extern int gUseSwap;
extern Log gLog;
extern FileIni inifile;

Table::Table()
{
	char value[100];
	FileIni::GetPrivateProfileStr( "TABLE", "INIT_COUNT", "1000", value, 100, "./server.ini" );
	mInitCount = atoi(value);
	FileIni::GetPrivateProfileStr( "TABLE", "INCLEMENT_COUNT", "1000", value, 100, "./server.ini" );
	mIncCount = atoi(value);
	FileIni::GetPrivateProfileStr( "TABLE", "INCLEMENT_PERCENT", "90", value, 100, "./server.ini" );
	mIncPercent = atoi(value);
}

Table *Table::CreateTable( string name, ColumnInfo *colinfo )
{
	Table *table = new Table;
	if( table == NULL )
	{
		gLog.log("new Table error" );
		return NULL;
	}
	strcpy( table->mTableName, name.c_str() );
	mTableMap[name] = table;

	if( gUseSwap == 0 )
	{
		int res = mlockall(MCL_CURRENT | MCL_FUTURE);
		if( res != 0 )
			gLog.log("mlockall fail!.");
	}

	Column *col;
	Column *newcol;
	list<Column*>::iterator iter2 = colinfo->mColList.begin();
	while( iter2 != colinfo->mColList.end() )
	{
		newcol = new Column;
		col = *iter2;
		strcpy( newcol->mColName, col->mColName );
		newcol->mType = col->mType;
		newcol->mSize = col->mSize;
		table->mColInfo.mColList.push_back( newcol );
		iter2++;
	}

	if( table->Refresh() != ERROR_OK )
		return NULL;
	return table;
}

int Table::DeleteTable( string name )
{
	Table *table;
	
	map<string, Table*>::iterator iter = Table::mTableMap.find( name );
	if( iter != Table::mTableMap.end() )
	{
		table = iter->second;
		Table::mTableMap.erase( iter );
		TimeoutThread::DelNode( table );
		delete table;
	}

	return 0;
}

Table *Table::GetTable( string name )
{
	Table *table;
	table = mTableMap[name];
	return table;
}

int Table::AddRow( Row *row )
{
	return AddRow( row, 0 );
}

int Table::AddRow( Row *row, unsigned long timeout )
{
	char *prow;
	Node *node;
	const char *key;
	
	list<Node*>::iterator iter = row->mNodeList.begin();

	if( row->mNodeList.size() == 0 )
	{
		gLog.log("insert row is invalid.");
		return ERROR_ROW_NOT_FOUND;
	}

	// get key node
	node = *iter;
	key = node->mValue;

	// get extra node
	if( mExtraRow.size() == 0 )
	{
		gLog.log("ExtraRow empty!");
		return ERROR_ROW_NOT_FOUND;
	}
	prow = mExtraRow.front();
	mExtraRow.pop_front();
	int pos = 0;

	// set key to buffer
	iter = row->mNodeList.begin();
	node = *iter;
	strcpy( prow, node->mValue );
	pos += KEY_NAME_SIZE;
	
	// set value to buffer
	Column *col;
	uint64_t timehost;
	if( timeout != 0 )
	{
		timehost = htonll( time(0) + timeout );
		memcpy( prow + pos, &timehost, TIMEOUT_VALUE_SIZE );
		//memcpy( prow + pos, &timehost, 4 );
		//memset( prow + pos+4, 0x00, 4 );
	}
	pos += TIMEOUT_VALUE_SIZE;

	// set timeout
	if( timeout > 0 )
		TimeoutThread::AddNode( this, key, timeout );

	list<Column*>::iterator iter2 = mColInfo.mColList.begin();
	iter2++;
	while( iter2 != mColInfo.mColList.end() )
	{
		col = *iter2;
		iter = row->mNodeList.begin();
		while( iter != row->mNodeList.end() )
		{
			node = *iter;
			if( !strcmp( col->mColName, node->mNodeName ) )
			{
				memcpy( prow + pos, node->mValue, col->mSize );
				prow[pos+col->mSize] = '\0';
				break;
			}
			iter++;
		}
		pos += col->mSize;
		pos ++;
		iter2++;
	}

	// set to map
	mRowMap[prow] = prow;
	return 0;
}

int Table::DelRow( const char *key )
{
	char *prow;
	map<const char *, char*, cmp_str>::iterator iter;
	iter = mRowMap.find( key );
	if( iter == mRowMap.end() )
	{
		gLog.log("Not Found key.");
		return ERROR_KEY_NOT_FOUND;
	}
	prow = iter->second;
	mExtraRow.push_back( prow );
	mRowMap.erase( iter );

	return 0;
}

char *Table::GetRow( const char *key, const char *col )
{
	char *prow;
	Node *node;
	map<const char *, char*, cmp_str>::iterator iter;
	list<Node*>::iterator iter2;

	iter = mRowMap.find( key );
	if( iter == mRowMap.end() )
	{
		gLog.log("Not Found key.");
		return NULL;
	}
	prow = iter->second;
	int pos = _getColPos( col );
	if( pos >= 0 )
		return prow+pos;
	return NULL;
}

int Table::UpdateRow( const char *key, const char *col, const char *value )
{
	return UpdateRow( key, col, value, strlen(value) );
}

int Table::UpdateRow( const char *key, const char *col, const char *value, int valsize )
{
	char *prow;
	Node *node;
	map<const char *, char*, cmp_str>::iterator iter;
	list<Node*>::iterator iter2;

	iter = mRowMap.find( key );
	if( iter == mRowMap.end() )
	{
		gLog.log("Not Found key.");
		return ERROR_KEY_NOT_FOUND;
	}
	prow = iter->second;
	int pos = _getColPos( col );
	if( pos > 0 )
	{
		memcpy( prow + pos, value, valsize );
		return 0;
	}

	return ERROR_COLUMN_NOT_FOUND;
}

int Table::RowSize()
{
	return _getRowLen();
}

int Table::RowCount()
{
	return mRowMap.size();
}

int Table::ReserveCount()
{
	return mExtraRow.size();
}

int Table::Refresh()
{
	int ret = ERROR_OK;
	if( ReserveCount() == 0 )
		ret = _incleaseExtraRow( mInitCount );
	// use percent
	int per = (100 * RowCount()) / (RowCount()+ReserveCount());
	if( per > mIncPercent )
		ret = _incleaseExtraRow( mIncCount );

	return ret;
}

void Table::Clear()
{
	// delete column infomation.
	mColInfo.Delete();

	// delete rows.
	char *prow;
	map<const char *, char*, cmp_str>::iterator iter = mRowMap.begin();
	while( iter != mRowMap.end() )
	{
		prow = iter->second;
		iter++;
		mExtraRow.push_back( prow );
	}
}

int Table::Backup( string name, char *filename )
{
	Table *table = GetTable(name);
	if( table == NULL )
		return ERROR_TABLE_NOT_FOUND;
	char value[100];
	FileIni::GetPrivateProfileStr( "BACKUP", "BACKUP_DIR", "backup", value, 100, "./server.ini" );

	FILE *fp;
	time_t now = time(0);
	char fname[256];
	struct tm *ptm;
	ptm = localtime(&now);

	sprintf( fname, "%s/%s_%04d%02d%02d_%02d%02d%02d.dmp", value, name.c_str(), 
			ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
			ptm->tm_hour, ptm->tm_min, ptm->tm_sec );
	if( (fp = fopen( fname, "a")) == NULL )
		return ERROR_FILE_NOT_CREATE;

	fprintf( fp, "TABLE %s\r\n", name.c_str() );

	Column *col;
	list<Column*>::iterator iter2;
	iter2 = table->mColInfo.mColList.begin();
	while( iter2 != table->mColInfo.mColList.end() )
	{
		col = *iter2;
		fprintf( fp, "COLUMN %s %d %d\r\n", col->mColName, col->mType, col->mSize );
		iter2++;
	}

	fprintf( fp, "DATA %d %d\r\n", table->RowCount(), table->RowSize() );

	char *prow;
	int i;
	map<const char *, char*, cmp_str>::iterator iter3;
	iter3 = table->mRowMap.begin();
	while( iter3 != table->mRowMap.end() )
	{
		prow = iter3->second;
		for( i=0; i< table->RowSize(); i++ )
			fprintf( fp, "%.02X ", (unsigned char)prow[i] );
		fprintf( fp, "\r\n");
		iter3 ++;
	}

	fprintf( fp, "END\r\n" );

	fclose( fp );

	strcpy( filename, fname );
	return ERROR_OK;
}

int Table::Restore( string name )
{
	FILE *fp;
	char buff[MAX_LINE_LENGTH];
	char table_name[COLUMN_NAME_SIZE+1];
	unsigned long count, size;
	unsigned long i, j;
	int pos;
	time_t now;
	uint64_t timehost;
	unsigned long timeval;

	char param1[COLUMN_NAME_SIZE+1];
	char param2[COLUMN_NAME_SIZE+1];
	char param3[COLUMN_NAME_SIZE+1];
	char param4[COLUMN_NAME_SIZE+1];

	list<Column*>::iterator iter2;
	now = time(0);

	if( (fp = fopen( name.c_str(), "r")) == NULL )
		return ERROR_FILE_NOT_OPEN;

	// TABLE name
	if( fgets( buff, MAX_LINE_LENGTH-1, fp ) == NULL )
		return ERROR_FILE_NOT_READ;
	
	if( sscanf( buff, "%s %s", param1, param2 ) != 2 )
		return ERROR_FILE_NOT_READ;

	gLog.log( "table : %s", param2 );
	strcpy( table_name, param2 );

	Table *table;
	ColumnInfo colinfo;
	Column *col;
	Row row;
	char *prow;

	table = Table::GetTable( param2 );
	if( table != NULL )
	{
		gLog.log("Restore Table Exist");
		return ERROR_TABLE_EXIST;
	}

	colinfo.Clear();
	// column info
	while( true )
	{
		if( fgets( buff, MAX_LINE_LENGTH-1, fp ) == NULL )
			return ERROR_FILE_NOT_READ;

		if( sscanf( buff, "%s %s %s %s", param1, param2, param3, param4 ) == 0 )
			return ERROR_FILE_NOT_READ;

		if( !strcmp( param1, "COLUMN") )
		{
			gLog.log( "column : %s %s %s", param2, param3, param4 );
			colinfo.AddColumn( param2, atoi(param3), atoi(param4) );
		}
		else if( !strcmp( param1, "DATA") )
		{
			gLog.log( "data : %s %s", param2, param3 );
			count = atol(param2);
			size = atol(param3);
			break;
		}
		else
		{
			fclose( fp );
			return ERROR_FILE_FORMAT;
		}
	}
	table = Table::CreateTable( table_name, &colinfo );
	prow = (char*)malloc(size+1);

	// data
	for( i=0; i< count; i++ )
	{
		row.Clear();
		pos = 0;

		bzero( prow, size+1 );
		for( j=0; j< size; j++ )
		{
			bzero( buff, 10 );
			fread( buff, 3, 1, fp );
			sscanf( buff, "%X", (unsigned int*)&prow[j] );
		}
		fread( buff, 2, 1, fp );

		iter2 = table->mColInfo.mColList.begin();
		col = *iter2;
		row.AddVal(col->mColName, prow );
		pos += KEY_NAME_SIZE;
		
		// timer
		char *tmp1 = prow+pos;
		time_t *tmp2 = (time_t*)tmp1;
		//timehost = ntohll( *tmp2 );
		timehost = ntohll( *(time_t*)tmp1 );
		timeval = 0;
		if( timehost != 0l )
		{
			if( timehost < now )
			{
				gLog.log("Data timeout : key:%s", prow );
				continue;
			}
			timeval = timehost - now;
		}

		pos += TIMEOUT_VALUE_SIZE;
		iter2 ++;
		while( iter2 != table->mColInfo.mColList.end() )
		{
			col = *iter2;
			row.AddVal( col->mColName, prow+pos );
			pos += col->mSize;
			pos ++;
			iter2 ++;
		}

		table->AddRow( &row, timeval );
	}
	
	// END
	if( fgets( buff, MAX_LINE_LENGTH-1, fp ) == NULL )
		return ERROR_FILE_NOT_READ;
	
	if( sscanf( buff, "%s", param1 ) != 1 )
		return ERROR_FILE_NOT_READ;

	if( strcmp( param1, "END" ) )
		gLog.log("Restore FAIL");
	else
		gLog.log("Restore SUCCESS");

	fclose( fp );
	return ERROR_OK;
}

void Table::Dump()
{
	Table *tab;
	Column *col;
	char *prow;
	Node *node;
	map<string, Table*>::iterator iter = Table::mTableMap.begin();
	list<Column*>::iterator iter2;
	map<const char *, char*, cmp_str>::iterator iter3;
	list<Node*>::iterator iter4;
	char logstr[1000];

	// table list
	while( iter != Table::mTableMap.end() )
	{
		tab = iter->second;
		gLog.log("[%s][%p]", iter->first.c_str(), tab );
		iter++;

		// column info
		iter2 = tab->mColInfo.mColList.begin();
		while( iter2 != tab->mColInfo.mColList.end() )
		{
			col = *iter2;
			gLog.log("  [%s][%d][%d]", col->mColName, col->mType, col->mSize);
			iter2++;
		}

		// row
		iter3 = tab->mRowMap.begin();
		while( iter3 != tab->mRowMap.end() )
		{
			prow = iter3->second;
			sprintf( logstr, " [%s] [", iter3->first);
			iter3++;

			for( int i=0; i< tab->RowSize(); i++ )
			{
				if( prow[i] == '\0' )
					strcat( logstr, "_");
				else
					strncat( logstr, prow+i, 1);
			}
			strcat( logstr, "]");
			gLog.log( logstr );
		}
		gLog.log("RowSize %d", tab->RowSize() );
		gLog.log("RowCount %d", tab->RowCount() );
		gLog.log("ReserveCount %d", tab->ReserveCount() );
	}
}

int Table::_getColPos(const char *name)
{
	int pos = 0;
	
	Column *col;
	list<Column*>::iterator iter = mColInfo.mColList.begin();
	col = *iter;
	if( !strcmp( col->mColName, name ) )
		return pos;

	iter ++;
	pos += KEY_NAME_SIZE + TIMEOUT_VALUE_SIZE;
	while( iter != mColInfo.mColList.end() )
	{
		col = *iter;
		if( !strcmp( col->mColName, name ) )
			return pos;
		pos += col->mSize;
		pos ++;
		iter ++;
	}
	return -1;
}

int Table::_getRowLen()
{
	int pos = KEY_NAME_SIZE + TIMEOUT_VALUE_SIZE;
	Column *col;
	list<Column*>::iterator iter = mColInfo.mColList.begin();
	iter ++;
	while( iter != mColInfo.mColList.end() )
	{
		col = *iter;
		pos += col->mSize;
		pos ++;
		iter ++;
	}
	return pos;
}

int Table::_incleaseExtraRow( int rowcount)
{
	// make extra rows
	// get row size
	Column *col;
	int rowlen = KEY_NAME_SIZE + TIMEOUT_VALUE_SIZE;
	list<Column*>::iterator iter = mColInfo.mColList.begin();
	iter ++;
	while( iter != mColInfo.mColList.end() )
	{
		col = *iter;
		rowlen += col->mSize + 1;	// value + delimiter
		iter ++;
	}

	// malloc
	char *buf;
	for( int i=0; i<rowcount; i++ )
	{
		buf = (char*)malloc(rowlen);
		if( buf == NULL )
		{
			gLog.log("malloc fail!. no free memory!");
			return ERROR_MEMALOCK_FAIL;
		}

		/*
		if( gUseSwap == 0 )
		{
			if( mlock( buf, rowlen ) )
			{
				gLog.log("mlock fail!. no free memory!");
				//return ERROR_MEMALOCK_FAIL;
			}
		}
		*/
		mExtraRow.push_back( buf );
	}

	return 0;
}

