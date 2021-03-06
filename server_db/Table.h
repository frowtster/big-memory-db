#include <cstring>
#include <string>
#include <map>
#include <list>

#include "Row.h"
#include "Column.h"
#include "TableContainer.h"

#pragma once

using namespace std;

class Table : public TableContainer {
	struct cmp_str
	{
		bool operator()(const char *a, const char *b) const
		{
			return std::strcmp(a, b) < 0;
		}
	};

	map<const char *, char*, cmp_str> mRowMap;
	list<char*> mExtraRow;

	int mInitCount;
	int mIncCount;
	int mIncPercent;
	int mUseSwap;

	bool mIsMultiCol;
	bool mIsMultiValue;
	int mSingleColDataType;
	size_t mSingleColDataSize;

	char lastSelKey[KEY_NAME_SIZE];
	char *lastSelRow;

	char mFetchName[KEY_NAME_SIZE];
	char mFetchValue[KEY_NAME_SIZE];
	char mFetchOp[3];

public:
	ColumnInfo mColInfo;
	static map<string, Table*> mTableMap;
public:
	char mTableName[TABLE_NAME_SIZE];

	Table();
	~Table() {
		Destroy();
	};

	static Table *CreateTable( string name, int type );
	static int DeleteTable( string name );
	static int DeleteTable( Table * );

	static Table *GetTable( string name );
	static void Dump();

	static int Backup( string name, char *filename );
	static int Restore( string name );

	int SetColumn( int type, size_t nsize );
	int SetColumn( ColumnInfo *colinfo );

	int RowSize();
	int RowCount();
	int ReserveCount();

	void Clear();
	void Destroy();
	int Refresh();

	// single column
	int AddRow( const char *key, const char *value );
	int AddRow( const char *key, const char *value, unsigned long timeout );
	int AddRow( const char *key, long value );
	int AddRow( const char *key, long value, unsigned long timeout );
	int UpdateRow( const char * key, const char *value );
	int UpdateRow( const char * key, const char *value, int valsize );
	int UpdateRow( const char * key, long value );
	int GetRow( const char * key, char *retval );
	int GetRow( const char * key, long *retval );
	int IncreaseValue( const char * key );
	int DecreaseValue( const char * key );

	// multi column
	int AddRow( const char *key, Row *row );
	int AddRow( const char *key, Row *row, unsigned long timeout );
	int UpdateRow( const char * key, const char *col, const char *value );
	int UpdateRow( const char * key, const char *col, const char *value, int valsize );
	int GetRow( const char * key, const char *col, char *retval );
	int GetRow( const char * key, const char *col, long *retval );
	int IncreaseValue( const char * key, const char *col );
	int DecreaseValue( const char * key, const char *col );

	// common
	int DelRow( const char * key );
	int FetchConditionSet( const char * cond );

private:
	int _getColPos(const char *col);
	int _getRowLen();
	int _incleaseExtraRow( int rowcount );

	int _fetchConditionCheck( const char *col, long value );

};
