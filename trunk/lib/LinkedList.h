#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class ONode{
	friend class OList;
private:
	ONode *next;
	ONode *prev;
	void *data;
public:
	ONode() {
		data=NULL;
		next=NULL;
		prev=NULL;
	};
	~ONode() {
		if( data != NULL )
		{
			free( (void*)data );
			data = NULL;
		}
	}
	ONode(const char *param) {
		int len = strlen(param);
		char *buf = (char*)data;
		data = malloc(len+1);
		strcpy( buf, param );
		buf[len] = '\0';

		next=NULL;
		prev=NULL;
	}
	ONode( const void *param, size_t len )
	{
		data = malloc(len+1);
		memcpy( data, param, len );

		next=NULL;
		prev=NULL;
	}
	char *DataChar()
	{
		return (char*)data;
	}
	void *DataBuf()
	{
		return data;
	}
	ONode *Next()
	{
		return next;
	}
	ONode *Prev()
	{
		return prev;
	}
};

class OList {
private:
	ONode *head;	
	ONode *tail;	
	int mCount;
public:
	OList() {
		mCount = 0;
		head=NULL;
		tail=NULL;
	};
	~OList() {
		Clear();
	};
	void InsertBack(ONode *insertNode)
	{
		if(head==NULL){
			head=insertNode;
		}
		else{
			tail->next = insertNode;
		}
		insertNode->prev = tail;
		insertNode->next = NULL;
		tail=insertNode;
		mCount ++;
	}
	void InsertBefore(ONode *base, ONode *pNode)
	{
		if( base == NULL )
			return InsertBack( pNode );
		if( pNode == NULL )
			return;
		ONode *pPrev = base->prev;
		if( pPrev != NULL )
			pPrev->next = pNode;
		pNode->prev = pPrev;
		pNode->next = base;
		base->prev = pNode;
		mCount ++;
	}
	void PrintList()
	{
		if(head==NULL){
			printf("List is Empty\n");
		}
		else{
			for(ONode *ptr=head;ptr!=NULL;ptr=ptr->next){
				printf("[%s]\n", (char*)ptr->data );
			}
		}
	}
	void Clear()
	{
		mCount = 0;
		ONode *ptr = head;
		ONode *ptr2;
		while( ptr != NULL )
		{
			ptr2 = ptr;
			ptr = ptr->next;
			delete ptr2;
		}
		head = NULL;
		tail = NULL;
	}

	ONode *GetHead()
	{
		return head;
	}

	ONode *GetTail()
	{
		return tail;
	}

	ONode *GetNext( ONode *node )
	{
		return node->next;
	}

	ONode *GetPrev( ONode *node )
	{
		return node->prev;
	}

	int GetCount()
	{
		return mCount;
	}

	void RemoveHead()
	{
		ONode *ptr = head;
		head = head->next;
		if( head != NULL )
			head->prev = NULL;
		delete ptr;
		mCount --;
		if( head == NULL )
		{
			mCount = 0;
			tail = NULL;
		}
	}
	ONode *RemoveAt(ONode *pnode)
	{
		ONode *ret = pnode->next;
		if( pnode->prev != NULL )
			pnode->prev->next = pnode->next;
		if( pnode->next != NULL )
			pnode->next->prev = pnode->prev;
		if( pnode == head )
			head = pnode->next;
		if( pnode == tail )
			tail = pnode->prev;
		delete pnode;
		mCount --;
		return ret;
	}
};
