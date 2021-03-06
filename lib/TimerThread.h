#pragma once

#include <pthread.h>
#include "LinkedList.h"
#include "TimerNode.h"

class TimerThread
{
	//static list<TimerNode*> mList;
	static OList mList;
public:
	static pthread_t mTimerThread;
	static pthread_mutex_t mTimerMutex;
	static time_t mLastTime;

	static int CreateInstance();
	static void Destroy();
	static void *TimerLoop( void * );
	static int _AddNode( TimerNode *);
	static int AddNode( TimerObject *, int event, int inteval);
	static int DelNode( TimerObject *, int event);
	static int DelNode( TimerObject * );

	static size_t Size();

	static void dump();
};
