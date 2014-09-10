#pragma once

class EventFactory
{
public:
	static event_client* createInstance( int )
	{
		return NULL;
	}
};

class ConnectFactory
{
public:
	static event_connect* createInstance()
	{
		return NULL;
	}
};

