#include <list>
#include "Constants.h"

#pragma once

using namespace std;

class Column {
public:
	char mColName[COLUMN_NAME_SIZE + 1];
	int mType;
	int mSize;
};

class ColumnInfo {
public:
	list<Column*> mColList;
public:
	ColumnInfo(){};
	~ColumnInfo(){
		Delete();
	};

	int AddColumn( const char *name, int type, int size) {
		Column *col = new Column;
		strcpy( col->mColName, name);
		col->mType = type;
		col->mSize = size;
		mColList.push_back( col );
		return 0;
	}
	void Delete() {
		Column *col;
		list<Column*>::iterator iter = mColList.begin();
		while( iter != mColList.end() )
		{
			col = *iter;
			iter++;
			delete col;
		}
		mColList.clear();
	}

	void Clear() {
		Delete();
	}
};

