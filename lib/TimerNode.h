#pragma once

#include <time.h>
#include "EventClient.h"

class event_client;

struct TimerNode {
	time_t mNextTime;
	int mInteval;
	int mEvent;
	TimerObject *mClient;
};

