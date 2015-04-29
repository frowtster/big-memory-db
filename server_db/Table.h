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
	size_t mSingleColDataSize;

public:
	ColumnInfo mColInfo;
	static map<string, Table*> mTableMap;
public:
	char mTableName[TABLE_NAME_SIZE];

	Table();
	~Table() {
		Destroy();
	};

	static Table *CreateTable( string name, size_t nsize );
	static Table *CreateTable( string name, ColumnInfo *colinfo );
	static int DeleteTable( string name );
	static Table *GetTable( string name );
	static void Dump();

	static int Backup( string name, char *filename );
	static int Restore( string name );

	int RowSize();
	int RowCount();
	int ReserveCount();

	void Clear();
	void Destroy();
	int Refresh();

	int AddRow( const char *key, const char *value );
	int AddRow( const char *key, const char *value, unsigned long timeout );
	int AddRow( Row *row );
	int AddRow( Row *row, unsigned long timeout );
	int DelRow( const char * key );
	int UpdateRow( const char * key, const char *value );
	int UpdateRow( const char * key, const char *value, int valsize );
	int UpdateRow( const char * key, const char *col, const char *value );
	int UpdateRow( const char * key, const char *col, const char *value, int valsize );
	char *GetRow( const char * key );
	char *GetRow( const char * key, const char *col );

private:
	int _getColPos(const char *col);
	int _getRowLen();
	int _incleaseExtraRow( int rowcount );
};
