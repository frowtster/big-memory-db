#pragma once

#include <time.h>

class TimerObject {
public:
	virtual void TimerCallback( int mEvent ){};
};

class event_client;

struct TimerNode {
	time_t mNextTime;
	int mInteval;
	int mEvent;
	TimerObject *mClient;
};

