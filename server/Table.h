#include <cstring>
#include <string>
#include <map>
#include <list>

#include "Row.h"
#include "Column.h"

#pragma once

using namespace std;

class Table {
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
public:
	ColumnInfo mColInfo;
	static map<string, Table*> mTableMap;
public:
	string mTableName;

	Table();
	~Table() {
		Clear();
	};

	static Table *CreateTable( string name, ColumnInfo *colinfo );
	static int DeleteTable( string name );
	static Table *GetTable( string name );
	static void Dump();

	int RowSize();
	int RowCount();
	int ReserveCount();

	void Clear();
	int Refresh();

	int AddRow( Row *row );
	int DelRow( const char * key );
	int UpdateRow( const char * key, const char *col, const char *value );
	int UpdateRow( const char * key, const char *col, const char *value, int valsize );
	char *GetRow( const char * key, const char *col );

private:
	int _getColPos(const char *col);
	int _getRowLen();
	int _incleaseExtraRow( int rowcount );
};
