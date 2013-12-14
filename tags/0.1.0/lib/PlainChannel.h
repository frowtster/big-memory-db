
class PlainChannel : public EventChannel
{
public:
	PlainChannel();

	static void ReadCallback( struct bufferevent *incoming, void *arg );
	static void WriteCallback( struct bufferevent *bev, void *arg );
	static void EventCallback( struct bufferevent *bev, short what, void *arg );
	static void AcceptCallback( int fd, short ev, void *arg );
};

