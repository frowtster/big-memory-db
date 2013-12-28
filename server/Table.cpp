#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>

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
	table->mTableName = name;
	mTableMap[name] = table;

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
	timehost = htonll( time(0) + timeout );
	memcpy( prow + pos, &timehost, TIMEOUT_VALUE_SIZE );
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

		if( gUseSwap == 0 )
		{
			if( mlock( buf, rowlen ) )
			{
				gLog.log("mlock fail!. no free memory!");
				return ERROR_MEMALOCK_FAIL;
			}
		}
		mExtraRow.push_back( buf );
	}

	return 0;
}

