#include <event2/bufferevent.h>
#include "EventClient.h"

class EventChannel
{
public:
	EventChannel();
	static int SetNonblock( int fd );

	static void ReadCallback( struct bufferevent *incoming, void *arg );
	static void WriteCallback( struct bufferevent *bev, void *arg );
	static void EventCallback( struct bufferevent *bev, short what, void *arg );
	//static void AcceptCallback( int fd, short ev, void *arg );
	static void AcceptCallback( struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *address, int socklen, void *ctx );
	static int Connect( const char *host, int port, event_connect* );
};

