#include "Packet.h"
#include <stdlib.h>

Packet::Packet()
{
	mBuffer = NULL;
}

char *Packet::GetBuffer()
{
	return mBuffer;
}

void Packet::Release()
{
	if( mBuffer )
		free( mBuffer );
}


