#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "TimeoutThread.h"
#include "Log.h"
extern Log gLog;

pthread_t TimeoutThread::mTimeoutThread;
pthread_mutex_t TimeoutThread::mTimeoutMutex;
//list<TimeoutNode*> TimeoutThread::mList;
OList TimeoutThread::mList;
time_t TimeoutThread::mLastTime = 0;

int TimeoutThread::CreateInstance()
{
	pthread_mutex_init(&mTimeoutMutex, NULL);

	int status, rc;
	rc = pthread_create( &mTimeoutThread, NULL, TimeoutThread::TimeoutLoop, NULL );
	if( rc )
	{
		gLog.log("pthread_create return rc %d", rc);
		return rc;
	}
	/*
	rc = pthread_join( mTimeoutThread, (void **)&status);
	if( rc )
		gLog.log("pthread_join return rc %d", rc);
	*/
	return rc;
}

void TimeoutThread::Destroy()
{
	int ret;
	ret = pthread_cancel( mTimeoutThread );
	if( ret != 0 )
		gLog.log("pthread_cancel return %d", ret );

	pthread_mutex_lock(&mTimeoutMutex);
/*
	TimeoutNode *node;
 	list<TimeoutNode*>::iterator iList;
	iList = TimeoutThread::mList.begin();
	while( iList != TimeoutThread::mList.end() )
	{
		node = *iList;
		iList = TimeoutThread::mList.erase( iList );
		delete node;
	}
*/
	TimeoutNode *node;
	ONode *tnode;
	tnode = TimeoutThread::mList.GetHead();
	while( tnode != NULL )
		tnode = TimeoutThread::mList.RemoveAt(tnode);

	mList.Clear();

	pthread_mutex_unlock(&mTimeoutMutex);
	ret = pthread_mutex_destroy( &mTimeoutMutex );
	if( ret != 0 )
		gLog.log("pthread_mutex_destroy return %d", ret );
}

void *TimeoutThread::TimeoutLoop( void * )
{
	TimeoutNode *node, *node2;
// 	list<TimeoutNode*>::iterator iList;
	time_t now = time(0);

	while (true )
	{
		if( mList.GetCount() == 0 )
		{
			sleep(1);
			now = time(0);
			continue;
		}

		//node = mList.front();
		node = (TimeoutNode*)(mList.GetHead()->DataBuf());
//		gLog.log("now %d node %d", (int)now, (int)node->mTime);
		if( node->mTime <= now )
		{
			gLog.log("mTime %d mClient %p", (int)node->mTime, node->mTable );

			pthread_mutex_lock(&mTimeoutMutex);
			//mList.pop_front();
			mList.RemoveHead();
			node->mTable->DelRow( node->mKey );
			pthread_mutex_unlock(&mTimeoutMutex);

			continue;
		}

		sleep(1);
		now = time(0);
	}

	return NULL;
}

int TimeoutThread::_AddNode( TimeoutNode *node )
{
 	//list<TimeoutNode*>::iterator iList;
 	ONode *pNode = new ONode( node, sizeof( class TimeoutNode ) );
	ONode *tnode;

	if( node->mTime >= mLastTime )
	{
		//TimeoutThread::mList.push_back( node );
		TimeoutThread::mList.InsertBack( pNode );
		mLastTime = node->mTime;
	}
	else
	{
		//gLog.log("search pos count %ld", mList.size());
		/*
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
		*/
		tnode = mList.GetTail();
		do {
			if( tnode == mList.GetHead() )
			{
				mList.InsertBefore( tnode, pNode );
				break;
			}
			tnode = tnode->Prev();
			if( ((TimeoutNode*)tnode->DataBuf())->mTime <= node->mTime )
			{
				tnode = tnode->Next();
				mList.InsertBefore( tnode, pNode );
				break;
			}
		} while( tnode != NULL );

		if( tnode == NULL )
		{
			mList.InsertBefore( mList.GetHead(), pNode );
		}
	}
	//gLog.log("after _AddNode count %ld", mList.size());
	return 0;
}

int TimeoutThread::AddNode( TableContainer *table, const char *key, time_t timeout )
{
	//gLog.log("AddNode before size %ld", mList.size() );
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
	ONode *tnode;
 	//list<TimeoutNode*>::iterator iList;
	pthread_mutex_lock(&mTimeoutMutex);
	/*
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
	*/
	tnode = TimeoutThread::mList.GetHead();
	while( tnode != NULL )
	{
		node = (TimeoutNode*)(tnode->DataBuf());
		if( node->mTable == table && !strcmp(node->mKey, key) )
		{
			tnode = TimeoutThread::mList.RemoveAt(tnode);
		}
		else
			tnode = tnode->Next();
	}
	pthread_mutex_unlock(&mTimeoutMutex);
	//gLog.log("DelNode after size %ld", mList.size() );
	return 0;
}

int TimeoutThread::DelNode( TableContainer *table )
{
	//gLog.log("DelNode before size %ld", mList.size() );
	TimeoutNode *node;
	ONode *tnode;
 	//list<TimeoutNode*>::iterator iList;
	pthread_mutex_lock(&mTimeoutMutex);
	/*
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
	*/
	tnode = TimeoutThread::mList.GetHead();
	while( tnode != NULL )
	{
		node = (TimeoutNode*)(tnode->DataBuf());
		if( node->mTable == table )
		{
			tnode = TimeoutThread::mList.RemoveAt(tnode);
		}
		else
			tnode = tnode->Next();
	}

	pthread_mutex_unlock(&mTimeoutMutex);
	//gLog.log("DelNode after size %ld", mList.size() );
	return 0;
}

size_t TimeoutThread::Size()
{
	return TimeoutThread::mList.GetCount();
}

void TimeoutThread::dump()
{
	gLog.log("dump start");
 	//list<TimeoutNode*>::iterator iList;
	ONode *tnode;
	TimeoutNode *node;
	pthread_mutex_lock(&mTimeoutMutex);
	/*
	for( iList = TimeoutThread::mList.begin(); iList != TimeoutThread::mList.end(); iList++ )
	{
		gLog.log("%d %p", (int)(*iList)->mTime, (*iList)->mTable );
	}
	*/
	for( tnode = TimeoutThread::mList.GetHead(); tnode != NULL; tnode = tnode->Next() )
	{
		node = (TimeoutNode*)(tnode->DataBuf());
		gLog.log("%d %p", (int)node->mTime, node->mTable );
	}
	pthread_mutex_unlock(&mTimeoutMutex);
	gLog.log("dump end");
}

