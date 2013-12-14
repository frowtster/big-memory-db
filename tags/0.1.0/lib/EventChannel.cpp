#include <stdio.h>
#include <event.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "EventClient.h"
#include "EventChannel.h"

#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include "Log.h"
extern Log gLog;

extern event_client *(*createInstance)( int );
extern struct event_base *base;

EventChannel::EventChannel()
{
}

int EventChannel::SetNonblock( int fd )
{
	int flags;

	flags = fcntl(fd, F_GETFL);
	flags |= O_NONBLOCK;
	fcntl(fd, F_SETFL, flags);
	return 0;
}

void EventChannel::ReadCallback( struct bufferevent *evbuf, void *arg )
{
	struct event_client *client = (struct event_client *)arg;
	if( client->bOneLine == true )
	{
		char *req;
		req = evbuffer_readline(evbuf->input);
		if (req == NULL)
			return;
		client->ReadlineCallback(req, (void *)evbuf );
		free(req);
	}
	else
	{
		unsigned char *buf;
		struct evbuffer *src;
		size_t len;
		src = evbuf->input;
		len = evbuffer_get_length(src);
		if( len <= 0 )
			return;
		buf = evbuffer_pullup( src, len );
		buf[len] = '\0';
		client->ReadCallback( (char*)buf, len, (void *)evbuf );
		evbuffer_drain( src, len );
	}
}

void EventChannel::WriteCallback( struct bufferevent *evbuf, void *arg )
{
	struct event_client *client = (struct event_client *)arg;
	client->WriteCallback( (void *)evbuf );
}

void EventChannel::EventCallback( struct bufferevent *evbuf, short what, void *arg )
{
	gLog.log("EventChannel::EventCallback %d", what);
	if( what & BEV_EVENT_CONNECTED )
	{
		struct event_connect *server = (struct event_connect *)arg;
		server->ConnectCallback( evbuf );
	}
	else if( what & BEV_EVENT_EOF )
	{
		struct event_connect *server = (struct event_connect *)arg;
		server->CloseCallback( evbuf );
		bufferevent_free(evbuf);
		close(server->fd);
		delete server;
	}
	else if( what & BEV_EVENT_ERROR )
	{
		struct event_client *client = (struct event_client *)arg;
		client->ErrorCallback( (void *)evbuf );
		//bufferevent_free(evbuf);
		//close(client->fd);
		delete client;
	}
}

void EventChannel::AcceptCallback( struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *address, int socklen, void *ctx )
{
	int *listenport = (int*)ctx;
	struct event_client *client;
	client = createInstance( *listenport );
	client->fd = fd;
	base = evconnlistener_get_base(listener);
	client->buffer_event = bufferevent_socket_new(
			base, fd, BEV_OPT_CLOSE_ON_FREE);

	bufferevent_setcb((struct bufferevent*)client->buffer_event, ReadCallback, WriteCallback, EventCallback, client);

	bufferevent_enable((struct bufferevent*)client->buffer_event, EV_READ|EV_WRITE);
}

int EventChannel::Connect( const char *host, int port, event_connect *server )
{
	gLog.log("EventChannel Connect");
	struct sockaddr_in server_addr;
	struct bufferevent *bev;

	strcpy( server->mHost, host);
	server->mPort = port;

	bzero( &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(host);
	server_addr.sin_port = htons(port);

	bev = (struct bufferevent *)server->buffer_event;
	bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
	bufferevent_setcb(bev, ReadCallback, WriteCallback, EventCallback, server);
	bufferevent_enable(bev, EV_READ|EV_WRITE);

	if (bufferevent_socket_connect(bev,
			(struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		/* Error starting connection */
		bufferevent_free(bev);
		return false;
	}
	server->fd = bufferevent_getfd( bev );
	return true;
}
