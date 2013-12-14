#include "EventClient.h"

void printEventBase();
int openServerSocket( int );

void EventExit();

void EventInit( event_client*(*param)( int ) );
void EventAdd( int );
void EventDispatch();
void EventClose();

void EventWrite( void *arg, char *buf, size_t len );
char * EventReadline( void *arg );
char * EventRead( void *arg );
