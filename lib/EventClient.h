#pragma once
#include "Defines.h"
#include "TimerNode.h"

enum OBJECT_TYPE {
	OBJECT_TYPE_ACCEPT = 0,
	OBJECT_TYPE_CONNECT
};

struct client {
	int fd;
	OBJECT_TYPE type;
	void *buffer_event;
};

class event_client : public client, public TimerObject
{
public:
	int bOneLine;
	event_client(int one = true){
		type = OBJECT_TYPE_ACCEPT;
		bOneLine = one;
	};
	virtual ~event_client(){};

	void SetOneLine( int val ) { bOneLine = val;};
	virtual void ReadlineCallback( char *buf, void * ){};
	virtual void ReadCallback( char *buf, size_t len, void * ){};
	virtual void WriteCallback( void * ){};
	virtual void CloseCallback( void * ){};
	virtual void ErrorCallback( void * ){};

};

class event_connect : public client, public TimerObject
{
public:
	int bOneLine;
	char mHost[HOSTLEN];
	int mPort;

	event_connect(int one = true){
		type = OBJECT_TYPE_CONNECT;
		bOneLine = one;
	};
	virtual ~event_connect(){};

	void SetOneLine( int val ) { bOneLine = val;};
	virtual void ReadlineCallback( char *buf, void * ){};
	virtual void ReadCallback( char *buf, size_t len, void * ){};
	virtual void WriteCallback( void * ){};
	virtual void CloseCallback( void * ){};
	virtual void ErrorCallback( void * ){};
	virtual void ConnectCallback( void * ){};

};


