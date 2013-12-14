#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class Node{
	friend class List;
public:
	Node(){
		data=NULL;
		next=NULL;
	};
	~Node(){
		if( data != NULL )
		{
			free( (void*)data );
			data = NULL;
		}
	}
	Node(const char *i){
		int len = strlen(i);
		data = (char*)malloc(len+1);
		strcpy( data, i );
		data[len] = '\0';

		next=NULL;
	}
	char *Data()
	{
		return data;
	}
	Node *Next()
	{
		return next;
	}
private:
	Node *next;
	char *data;
};

class List{
private:
	Node *head;	
	Node *tail;	
public:
	List(){
		head=NULL;
		tail=NULL;
	};
	void insert(Node *insertNode)
	{
		if(head==NULL){
			head=insertNode;
		}
		else{
			tail->next = insertNode;
		}
		insertNode->next = NULL;
		tail=insertNode;
	}
	void printList()
	{
		if(head==NULL){
			printf("List is Empty\n");
		}
		else{
			for(Node *ptr=head;ptr!=NULL;ptr=ptr->next){
				printf("[%s]\n", ptr->data );
			}
		}
	}
	void clear()
	{
		Node *ptr = head;
		Node *ptr2;
		while( ptr != NULL )
		{
			ptr2 = ptr;
			ptr = ptr->next;
			delete ptr2;
		}
		head = NULL;
		tail = NULL;
	}

	Node *getHead()
	{
		return head;
	}

	void remove_head()
	{
		Node *ptr = head;
		head = head->next;
		delete ptr;
	}
};
