
#include <stdio.h>
#include <event.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#include "Defines.h"
#include "EventClient.h"
#include "EventChannel.h"
#include "HttpChannel.h"
#include "Log.h"
extern Log gLog;

extern unsigned long gPacketCount;
extern unsigned long gPoll0;
extern unsigned long gPoll1;
extern unsigned long gPoll2;

extern unsigned long gMinuteCount;

extern FILE *Poll0, *Poll1, *Poll2;

void HttpChannel::ReadCallback( struct bufferevent *evbuf, void *arg )
{
	gLog.log("read callback");
	struct evbuffer *evreturn;
	char *req;
	struct client *client = (struct client *)arg;
	char buffer[512];
	memset( buffer, 0, 512);

	while( true )
	{
		req = evbuffer_readline(evbuf->input);
		if (req == NULL)
			return;

		strcpy( buffer, req );
		free( req );

		if( !strncmp( buffer, "POLL ", 5 ) )
			break;

	}

	strcat( buffer, "\r\n");
	gPacketCount ++;
	gMinuteCount ++;

	if( buffer[5] == '0' )
	{
		gPoll0 ++;
		fwrite( buffer, strlen(buffer), 1, Poll0 );
	}
	else if( buffer[5] == '1' )
	{
		gPoll1 ++;
		fwrite( buffer, strlen(buffer), 1, Poll1 );
	}
	else if( buffer[5] == '2' )
	{
		gPoll2 ++;
		fwrite( buffer, strlen(buffer), 1, Poll2 );
	}
		
	fflush( Poll0 );
	fflush( Poll1 );
	fflush( Poll2 );

	evreturn = evbuffer_new();
	evbuffer_add_printf(evreturn, HTTP_RESPONSE );
	bufferevent_write_buffer(evbuf,evreturn);
	evbuffer_free(evreturn);

}

void HttpChannel::WriteCallback( struct bufferevent *bev, void *arg )
{
	gLog.log("write callback");
	struct client *client = (struct client *)arg;
}

void HttpChannel::EventCallback( struct bufferevent *bev, short what, void *arg )
{
	gLog.log("error callback");
	struct client *client = (struct client *)arg;
	bufferevent_free((struct bufferevent *)client->buffer_event);
	close(client->fd);
	free(client);
}

void HttpChannel::AcceptCallback( int fd, short ev, void *arg )
{
	int client_fd;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	struct client *client;

	client_fd = accept(fd,
			(struct sockaddr *)&client_addr,
			&client_len);
	gLog.log("accept callback fd[%d]", client_fd);
	if (client_fd < 0)
	{
		gLog.log("Client: accept() failed");
		return;
	}

	SetNonblock(client_fd);

	client = (struct client*)calloc(1, sizeof(*client));
	if (client == NULL)
		gLog.log("malloc failed");
	client->fd = client_fd;

	client->buffer_event = bufferevent_new(client_fd,
			ReadCallback,
			WriteCallback,
			EventCallback,
			client);

	bufferevent_enable((struct bufferevent *)client->buffer_event, EV_READ);

}

