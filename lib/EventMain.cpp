#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <event.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <list>

#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include "Defines.h"
#include "EventClient.h"
#include "EventChannel.h"
#include "PlainChannel.h"
#include "HttpChannel.h"
#include "Log.h"
extern Log gLog;

using namespace std;
struct event_base *base;

event_client *(*createInstance)( int );

void printEventBase()
{
	base = event_base_new();
	if( base == NULL )
	{
		gLog.log("event base error");
		return;
	}
	gLog.log("Event Base %p[%s]", base, event_base_get_method( base ));
}

int openServerSocket( int port )
{
	int socketlisten;
	struct sockaddr_in addresslisten;
	int reuse = 1;

	socketlisten = socket(AF_INET, SOCK_STREAM, 0);

	if (socketlisten < 0)
	{
		fprintf(stderr,"Failed to create listen socket");
		return -1;
	}

	memset(&addresslisten, 0, sizeof(addresslisten));

	addresslisten.sin_family = AF_INET;
	addresslisten.sin_addr.s_addr = inet_addr("0.0.0.0");
	addresslisten.sin_port = htons(port);

	if (bind(socketlisten,
				(struct sockaddr *)&addresslisten,
				sizeof(addresslisten)) < 0)
	{
		fprintf(stderr,"Failed to bind\n");
		return -1;
	}

	if (listen(socketlisten, 5) < 0)
	{
		fprintf(stderr,"Failed to listen to socket");
		return -1;
	}

	setsockopt(socketlisten,
			SOL_SOCKET,
			SO_REUSEADDR,
			&reuse,
			sizeof(reuse));

	return socketlisten;
}

void EventExit()
{
	event_loopexit( NULL );
}

struct sListen
{
	int mPort;
	int mSocket;
	struct event accept_event;
};

list<struct sListen*> gPortList;

void EventInit( event_client*(*param)(int) )
{
	gPortList.clear();
	createInstance = param;
}

void EventAdd( int port )
{
	struct sListen *listen= new sListen;
	listen->mPort = port;
	gPortList.push_back( listen );
}

int EventDispatch()
{
	struct sListen *listen;
	struct evconnlistener *listener;
	struct sockaddr_in sin;

	list<struct sListen*>::iterator ilist;
	for( ilist = gPortList.begin(); ilist != gPortList.end(); ilist++ )
	{
		listen = (*ilist);
		memset(&sin, 0, sizeof(sin));
		/* This is an INET address */
		sin.sin_family = AF_INET;
		/* Listen on 0.0.0.0 */
		sin.sin_addr.s_addr = htonl(0);
		/* Listen on the given port. */
		sin.sin_port = htons(listen->mPort);

		listener = evconnlistener_new_bind( 
				base, EventChannel::AcceptCallback, 
				(void*)&(listen->mPort), LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE, -1, 
				(struct sockaddr*)&sin, sizeof(sin) );
		if( !listener )
		{
			perror("Couldn't create listener");
			return -1;
		}
/*
		listen->mSocket = openServerSocket( listen->mPort );
		if( listen->mSocket == -1 )
			return -1;

		EventChannel::SetNonblock(listen->mSocket);

		event_set(&(listen->accept_event),
				listen->mSocket,
				EV_READ|EV_PERSIST,
				EventChannel::AcceptCallback,
				(void*)&(listen->mPort));

		event_add(&(listen->accept_event),
				NULL);
*/

	}
	while( true )
	{
		struct timeval timev;
		timev.tv_sec = 0;
		timev.tv_usec = 1000;
		event_base_loopexit(base, &timev);
		event_base_dispatch(base);
	}
	return 0;
}

void EventClose()
{
	struct sListen *listen;
	list<struct sListen*>::iterator ilist;
	for( ilist = gPortList.begin(); ilist != gPortList.end(); ilist++ )
	{
		listen = (*ilist);
		close(listen->mSocket);
		delete listen;
	}
	gPortList.clear();
}

void EventWrite( void *arg, char *buf, size_t len )
{
	struct bufferevent *evbuf = (struct bufferevent *)arg;
	struct evbuffer *evreturn;
	evreturn = evbuffer_new();
	evbuffer_add(evreturn, buf, len);
	bufferevent_write_buffer(evbuf,evreturn);
	evbuffer_free(evreturn);
}

char * EventReadline( void *arg )
{
	char *buf;
	struct bufferevent *evbuf = (struct bufferevent *)arg;
	buf = evbuffer_readline(evbuf->input);
	return buf;
}

char * EventRead( void *arg )
{
	char *buf;
	struct bufferevent *evbuf = (struct bufferevent *)arg;
	buf = evbuffer_readline(evbuf->input);
	return buf;
}

