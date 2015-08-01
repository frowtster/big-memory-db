#include <string.h>
#include <stdlib.h>
#include "Constants.h"
#include "util.h"
#include "ErrorCode.h"

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

void _setNext( char *row, char *addr )
{
	int rowlen = KEY_NAME_SIZE + TIMEOUT_VALUE_SIZE + sizeof(char *);
	long *buf = (long*)(row+rowlen);
	*buf = (long)addr;
}

void _setPrev( char *row, char *addr )
{
	int rowlen = KEY_NAME_SIZE + TIMEOUT_VALUE_SIZE;
	long *buf = (long*)(row+rowlen);
	*buf = (long)addr;
}

char *_getNext( char *row )
{
	int rowlen = KEY_NAME_SIZE + TIMEOUT_VALUE_SIZE + sizeof(char *);
	long *buf = (long*)(row+rowlen);
	return (char*)*buf;
}

char *_getPrev( char *row )
{
	int rowlen = KEY_NAME_SIZE + TIMEOUT_VALUE_SIZE;
	long *buf = (long*)(row+rowlen);
	return (char*)*buf;
}

char *rtrim( char *s )
{
	size_t len = strlen( s );
	for( size_t i=len; i>0; i-- )
		if( s[i] == ' ' || s[i] == '\t' )
			s[i] = '\0';
	return s;
}

char *ltrim( char *s )
{
	size_t len = strlen( s );
	size_t pos = 0;
	for( size_t i=0; i<len; i++ )
	{
		if( s[i] == ' ' || s[i] == '\t' )
			pos ++;
		else
			break;
	}
	if( pos == 0 )
		return s;

	for( size_t i=pos; i<len; i++ )
		s[i-pos] = s[i];
	s[len-pos] = '\0';
	return s;
}

char *trim( char *s )
{
	return ltrim(rtrim( s ) );
}

// cond : "param1 > 3"
int parseFetchCondition( const char *cond, char *fetchParam1, char *fetchParam2, char *fetchOperator )
{
	fetchParam1[0] = '\0';
	fetchParam2[0] = '\0';
	fetchOperator[0] = '\0';
	int count = sizeof(OPERATOR)/sizeof(OPERATOR[0]);
	char *op_pos = NULL;
	for( int i=0; i< count; i++ )
	{
		op_pos = strstr( cond, OPERATOR[i] );
		if( op_pos == NULL )
			continue;

		strcpy( fetchOperator, OPERATOR[i] );
		strncpy( fetchParam1, cond, (op_pos - cond) );
		strcpy( fetchParam2, (op_pos + strlen(OPERATOR[i])) );
		break;
	}

	if( strlen( fetchOperator ) == 0 )
		return ERROR_PARAMETER;

	trim(fetchParam1);
	trim(fetchParam2);

	if( strcmp( fetchOperator, "=<" ) == 0 )
		strcpy( fetchOperator, "<=" );
	if( strcmp( fetchOperator, "=>" ) == 0 )
		strcpy( fetchOperator, ">=" );

	return ERROR_OK;
}

bool checkConditionSingleNum( const char *value, const char *fetchValue, const char *fetchOp )
{
	if( fetchValue[0] == '\0' || fetchOp[0] == '\0' )
		return false;

	long lval, fval;
	memcpy( &lval, value, sizeof(uint64_t) );
	fval = atol( fetchValue );

	if( lval == fval )
	{
		if( fetchOp[0] == '=' )
			return true;
	}
	else if( lval > fval )
	{
		if( fetchOp[0] == '>' )
			return true;
	}
	else if( lval < fval )
	{
		if( fetchOp[0] == '<' )
			return true;
	}
	return false;
}

char *invertOperator( char *s )
{
	char *pos = strchr( s, '<' );
	if( pos != NULL )
		*pos = '>';

	pos = strchr( s, '>' );
	if( pos != NULL )
		*pos = '<';

	return s;
}

