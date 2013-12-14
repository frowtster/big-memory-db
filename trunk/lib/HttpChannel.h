
#define HTTP_RESPONSE \
"HTTP/1.1 200 OK\r\n" \
"Connection: Close\r\n" \
"Content-Length: 0\r\n" \
"Content-Type: text/plain\r\n" \
"Server: localhost\r\n" \
"\r\n"

class HttpChannel : public EventChannel
{
public:
	HttpChannel();

	static void ReadCallback( struct bufferevent *incoming, void *arg );
	static void WriteCallback( struct bufferevent *bev, void *arg );
	static void EventCallback( struct bufferevent *bev, short what, void *arg );
	static void AcceptCallback( int fd, short ev, void *arg );
};


