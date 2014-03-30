#include <string.h>
#include <stdlib.h>

int NewLineSize(char *buf)
{
	return 2;
    int len = strlen(buf);
    if( *(buf+len-1) != '\n' )
        return 0;
    if( *(buf+len-2) != '\r' )
        return 1;
    return 2;
}

static const char alphanum[] =
	"0123456789"
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz";

int stringLength = sizeof(alphanum) - 1;

void RandomString( char *buf, int length )
{
	for( int i=0; i< length; i++ )
	{
		buf[i] = alphanum[random() % stringLength];
	}
	buf[length] = '\0';
}

