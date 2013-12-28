#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "TimeoutThread.h"
#include "Log.h"
extern Log gLog;

pthread_t TimeoutThread::mTimeoutThread;
pthread_mutex_t TimeoutThread::mTimeoutMutex;
list<TimeoutNode*> TimeoutThread::mList;
time_t TimeoutThread::mLastTime = 0;

int TimeoutThread::CreateInstance()
{
	pthread_mutexattr_t mutexattr;
	pthread_mutex_init(&mTimeoutMutex, &mutexattr);

	return pthread_create( &mTimeoutThread, NULL, TimeoutThread::TimeoutLoop, NULL );
}

void TimeoutThread::Destroy()
{
	TimeoutNode *node;
 	list<TimeoutNode*>::iterator iList;
	pthread_mutex_lock(&mTimeoutMutex);
	iList = TimeoutThread::mList.begin();
	while( iList != TimeoutThread::mList.end() )
	{
		node = *iList;
		iList = TimeoutThread::mList.erase( iList );
		delete node;
	}
	pthread_mutex_unlock(&mTimeoutMutex);
	pthread_mutex_destroy( &mTimeoutMutex );
}

void *TimeoutThread::TimeoutLoop( void * )
{
	TimeoutNode *node, *node2;
 	list<TimeoutNode*>::iterator iList;
	time_t now = time(0);

	while (true )
	{
		if( mList.size() == 0 )
		{
			sleep(1);
			now = time(0);
			continue;
		}

		node = mList.front();
//		gLog.log("now %d node %d", (int)now, (int)node->mTime);
		if( node->mTime <= now )
		{
			gLog.log("mTime %d mClient %p", (int)node->mTime, node->mTable );

			pthread_mutex_lock(&mTimeoutMutex);
			mList.pop_front();
			pthread_mutex_unlock(&mTimeoutMutex);

			node->mTable->DelRow( node->mKey );
			continue;
		}

		sleep(1);
		now = time(0);
	}

	return NULL;
}

int TimeoutThread::_AddNode( TimeoutNode *node )
{
 	list<TimeoutNode*>::iterator iList;

	if( node->mTime >= mLastTime )
	{
		TimeoutThread::mList.push_back( node );
		mLastTime = node->mTime;
	}
	else
	{
		//gLog.log("search pos count %ld", mList.size());
		iList = mList.end();
		do {
			if( iList == mList.begin() )
			{
				mList.insert( iList, node );
				break;
			}
			iList --;
			//gLog.log("iList %d node %d", (int)(*iList)->mTime, (int)node->mTime);
			if( (*iList)->mTime <= node->mTime )
			{
				iList ++;
				mList.insert( iList, node );
				break;
			}
		} while( iList != mList.begin() );

		if( iList == mList.begin() )
		{
			//gLog.log("insert to front");
			mList.push_front( node );
		}
	}
	//gLog.log("after _AddNode count %ld", mList.size());
	return 0;
}

int TimeoutThread::AddNode( TableContainer *table, const char *key, time_t timeout )
{
	//gLog.log("AddNode before size %ld", mList.size() );
 	list<TimeoutNode*>::iterator iList;
	time_t now = time(0);
	struct TimeoutNode *node = new TimeoutNode;
	node->mTable = table;
	strcpy( node->mKey, key );
	node->mTime = now + timeout;

	pthread_mutex_lock(&mTimeoutMutex);
	_AddNode( node );
	pthread_mutex_unlock(&mTimeoutMutex);
	//gLog.log("AddNode after size %ld", mList.size() );
	return 0;
}

int TimeoutThread::DelNode( TableContainer *table, const char *key )
{
	//gLog.log("DelNode before size %ld", mList.size() );
	TimeoutNode *node;
 	list<TimeoutNode*>::iterator iList;
	pthread_mutex_lock(&mTimeoutMutex);
	iList = TimeoutThread::mList.begin();
	while( iList != TimeoutThread::mList.end() )
	{
		//gLog.log("mClient %p client %p", (*iList)->mClient, client );
		if( ((*iList)->mTable == table) && !strcmp((*iList)->mKey, key) )
		{
			node = *iList;
			iList = TimeoutThread::mList.erase( iList );
			delete node;
		}
		else
			iList ++;
	}
	pthread_mutex_unlock(&mTimeoutMutex);
	//gLog.log("DelNode after size %ld", mList.size() );
	return 0;
}

int TimeoutThread::DelNode( TableContainer *table )
{
	//gLog.log("DelNode before size %ld", mList.size() );
	TimeoutNode *node;
 	list<TimeoutNode*>::iterator iList;
	pthread_mutex_lock(&mTimeoutMutex);
	iList = TimeoutThread::mList.begin();
	while( iList != TimeoutThread::mList.end() )
	{
		//gLog.log("mClient %p client %p", (*iList)->mClient, client );
		if( (*iList)->mTable == table )
		{
			node = *iList;
			iList = TimeoutThread::mList.erase( iList );
			delete node;
		}
		else
			iList ++;
	}
	pthread_mutex_unlock(&mTimeoutMutex);
	//gLog.log("DelNode after size %ld", mList.size() );
	return 0;
}

size_t TimeoutThread::Size()
{
	return TimeoutThread::mList.size();
}

void TimeoutThread::dump()
{
	gLog.log("dump start");
 	list<TimeoutNode*>::iterator iList;
	pthread_mutex_lock(&mTimeoutMutex);
	for( iList = TimeoutThread::mList.begin(); iList != TimeoutThread::mList.end(); iList++ )
	{
		gLog.log("%d %p", (int)(*iList)->mTime, (*iList)->mTable );
	}
	pthread_mutex_unlock(&mTimeoutMutex);
	gLog.log("dump end");
}

