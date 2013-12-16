#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "TimerThread.h"
#include "Log.h"
extern Log gLog;

pthread_t TimerThread::mTimerThread;
pthread_mutex_t TimerThread::mTimerMutex;
list<TimerNode*> TimerThread::mList;
time_t TimerThread::mLastTime = 0;

int TimerThread::CreateInstance()
{
	pthread_mutexattr_t mutexattr;
	pthread_mutex_init(&mTimerMutex, &mutexattr);

	return pthread_create( &mTimerThread, NULL, TimerThread::TimerLoop, NULL );
}

void TimerThread::Destroy()
{
	TimerNode *node;
 	list<TimerNode*>::iterator iList;
	pthread_mutex_lock(&mTimerMutex);
	iList = TimerThread::mList.begin();
	while( iList != TimerThread::mList.end() )
	{
		node = *iList;
		iList = TimerThread::mList.erase( iList );
		delete node;
	}
	pthread_mutex_unlock(&mTimerMutex);
	pthread_mutex_destroy( &mTimerMutex );
}

void *TimerThread::TimerLoop( void * )
{
	TimerNode *node, *node2;
 	list<TimerNode*>::iterator iList;
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
//		gLog.log("now %d node %d", (int)now, (int)node->mNextTime);
		if( node->mNextTime <= now )
		{
			//gLog.log("next %d interval %d event %d mClient %p", (int)node->mNextTime, (int)node->mInteval, node->mEvent, node->mClient );

			pthread_mutex_lock(&mTimerMutex);
			mList.pop_front();
			//node->mNextTime += node->mInteval;
			node->mNextTime = now + node->mInteval;
			_AddNode( node );
			pthread_mutex_unlock(&mTimerMutex);

			node->mClient->TimerCallback( node->mEvent );
			continue;
		}

		sleep(1);
		now = time(0);
	}

	return NULL;
}

int TimerThread::_AddNode( TimerNode *node )
{
 	list<TimerNode*>::iterator iList;

	if( node->mNextTime >= mLastTime )
	{
		TimerThread::mList.push_back( node );
		mLastTime = node->mNextTime;
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
			//gLog.log("iList %d node %d", (int)(*iList)->mNextTime, (int)node->mNextTime);
			if( (*iList)->mNextTime <= node->mNextTime )
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

int TimerThread::AddNode( TimerObject *client, int event, int inteval )
{
	//gLog.log("AddNode before size %ld", mList.size() );
 	list<TimerNode*>::iterator iList;
	time_t now = time(0);
	struct TimerNode *node = new TimerNode;
	node->mNextTime = now+inteval;
	node->mInteval = inteval;
	node->mEvent = event;
	node->mClient = client;

	pthread_mutex_lock(&mTimerMutex);
	_AddNode( node );
	pthread_mutex_unlock(&mTimerMutex);
	//gLog.log("AddNode after size %ld", mList.size() );
	return 0;
}

int TimerThread::DelNode( TimerObject *client, int event )
{
	//gLog.log("DelNode before size %ld", mList.size() );
	TimerNode *node;
 	list<TimerNode*>::iterator iList;
	pthread_mutex_lock(&mTimerMutex);
	iList = TimerThread::mList.begin();
	while( iList != TimerThread::mList.end() )
	{
		if( (*iList)->mClient == client && (*iList)->mEvent == event )
		{
			node = *iList;
			iList = TimerThread::mList.erase( iList );
			delete node;
		}
		else
			iList ++;
	}
	pthread_mutex_unlock(&mTimerMutex);
	//gLog.log("DelNode after size %ld", mList.size() );
	return 0;
}

int TimerThread::DelNode( TimerObject *client )
{
	//gLog.log("DelNode before size %ld", mList.size() );
	TimerNode *node;
 	list<TimerNode*>::iterator iList;
	pthread_mutex_lock(&mTimerMutex);
	iList = TimerThread::mList.begin();
	while( iList != TimerThread::mList.end() )
	{
		//gLog.log("mClient %p client %p", (*iList)->mClient, client );
		if( (*iList)->mClient == client )
		{
			node = *iList;
			iList = TimerThread::mList.erase( iList );
			delete node;
		}
		else
			iList ++;
	}
	pthread_mutex_unlock(&mTimerMutex);
	//gLog.log("DelNode after size %ld", mList.size() );
	return 0;
}

size_t TimerThread::Size()
{
	return TimerThread::mList.size();
}

void TimerThread::dump()
{
	gLog.log("dump start");
 	list<TimerNode*>::iterator iList;
	pthread_mutex_lock(&mTimerMutex);
	for( iList = TimerThread::mList.begin(); iList != TimerThread::mList.end(); iList++ )
	{
		gLog.log("%d %d %d %p", (int)(*iList)->mNextTime, (int)(*iList)->mInteval, (*iList)->mEvent, (*iList)->mClient );
	}
	pthread_mutex_unlock(&mTimerMutex);
	gLog.log("dump end");
}

