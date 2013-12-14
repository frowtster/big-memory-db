#include <stdlib.h>
#include <list>

#include "Constants.h"

#pragma once

using namespace std;

class Node {
public:
	char mNodeName[COLUMN_NAME_SIZE + 1];
	char *mValue;
};

class Row {
public:
	list<Node*> mNodeList;
public:
	Row(){};
	~Row(){
		Delete();
	};

	int AddVal( const char *name, const char *value) {
		return this->AddVal( name, value, strlen(value) );
	}
	int AddVal( const char *name, const char *value, int size) {
		Node *node = new Node;
		node->mValue = NULL;
		strcpy( node->mNodeName, name );
		node->mValue = (char *)calloc(1,size);
		strncpy( node->mValue, value, size );
		mNodeList.push_back( node );
		return 0;
	}
	void Delete() {
		Node *node;
		list<Node*>::iterator iter = mNodeList.begin();
		while( iter != mNodeList.end() )
		{
			node = *iter;
			if( node->mValue )
				free( node->mValue );
			iter++;
			delete node;
		}
		this->Clear();
	}

	void Clear() {
		mNodeList.clear();
	}
};


