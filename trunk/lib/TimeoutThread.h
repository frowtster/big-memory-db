#pragma once

#include <pthread.h>
#include <time.h>
#include "TimeoutNode.h"
#include "TableContainer.h"
#include "LinkedList.h"

class TimeoutThread
{
	//static list<TimeoutNode*> mList;
	static OList mList;
public:
	static pthread_t mTimeoutThread;
	static pthread_mutex_t mTimeoutMutex;
	static time_t mLastTime;

	static int CreateInstance();
	static void Destroy();
	static void *TimeoutLoop( void * );
	static int _AddNode( TimeoutNode *node );
	static int AddNode( TableContainer *table, const char *key, time_t timeout);
	static int DelNode( TableContainer *table, const char *key );
	static int DelNode( TableContainer *table );

	static size_t Size();

	static void dump();
};
