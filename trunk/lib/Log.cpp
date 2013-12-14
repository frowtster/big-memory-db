/*
 * log.cpp
 */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include "Log.h"
#include "Defines.h"

int Log::init(const char * path, const char * head, Log::reopen_t reopen, Log::loglevel_t level = LEVEL_INFO )
{
	strcpy(m_path, path);
	strcpy(m_head, head);
	m_reopen = reopen;
	mLoglevel = level;
	return TRUE;
}

void Log::changeLogLevel( Log::loglevel_t level = LEVEL_INFO )
{
	mLoglevel = level;
}

int Log::reopen(time_t now)
{
	char fname[256];
	struct tm * ptm;
	ptm = localtime(&now);

	if(m_fp != NULL)
		fclose(m_fp);

	if(m_reopen == REOPEN_MM)
	{
		sprintf(fname, "%s/%s.%04d_%02d_%02d_%02d_%02d.log",
				m_path, m_head, ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min);
	}
	if(m_reopen == REOPEN_HH)
	{
		sprintf(fname, "%s/%s.%04d_%02d_%02d_%02d.log",
				m_path, m_head, ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour);
	}
	else if(m_reopen == REOPEN_DD)
	{
		sprintf(fname, "%s/%s.%04d_%02d_%02d.log",
				m_path, m_head, ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
	}

	if((m_fp = fopen(fname, "a")) == NULL)
	{
		fprintf(stderr, "logging fail: cannot open file: %s, %d\n", fname, errno);
		return FALSE;
	}
	if(m_reopen == REOPEN_MM)
		m_last = now / 60;
	if(m_reopen == REOPEN_HH)
		m_last = now / 3600;
	if(m_reopen == REOPEN_DD)
		m_last = ptm->tm_mday;
	return TRUE;
}

bool Log::checkTime(time_t now)
{
	if(m_reopen == REOPEN_MM && ((now / 60) != m_last))
		return true;
	if(m_reopen == REOPEN_HH && ((now / 3600) != m_last))
		return true;
	if(m_reopen == REOPEN_DD)
	{
		struct tm * ptm = localtime(&now);
		if(m_last != ptm->tm_mday)
			return true;
	}
	return false;
}

void Log::log( const char * format, ...)
{
	va_list args;
	struct tm * ptm;
	time_t now = time(NULL);
	ptm = localtime(&now);
	char LEVEL[10];

	if(m_fp == NULL || checkTime(now))
		reopen(now);
	if(m_fp == NULL)
		return;

	if( mLoglevel > LEVEL_INFO )
		return;

	strcpy(LEVEL, "INFO");
	
	va_start(args, format);

	fprintf(m_fp, "%02d%02d%02d%02d%02d%02d %8s ", 
			ptm->tm_year - 100, ptm->tm_mon + 1, ptm->tm_mday,
			ptm->tm_hour, ptm->tm_min, ptm->tm_sec, LEVEL );

	vfprintf(m_fp, format, args);
	fprintf(m_fp, "\n");
	fflush(m_fp);

	va_end(args);
}

void Log::log( loglevel_t level, const char * format, ...)
{
	va_list args;
	struct tm * ptm;
	time_t now = time(NULL);
	ptm = localtime(&now);
	char LEVEL[10];

	if(m_fp == NULL || checkTime(now))
		reopen(now);
	if(m_fp == NULL)
		return;

	if( mLoglevel > level )
		return;

	if( level == LEVEL_TRACE )
		strcpy(LEVEL, "TRACE");
	else if( level == LEVEL_DEBUG )
		strcpy(LEVEL, "DEBUG");
	else if( level == LEVEL_INFO )
		strcpy(LEVEL, "INFO");
	else if( level == LEVEL_ERROR )
		strcpy(LEVEL, "ERROR");
	else if( level == LEVEL_CRITICAL )
		strcpy(LEVEL, "CRITICAL");
	
	va_start(args, format);

	fprintf(m_fp, "%02d%02d%02d%02d%02d%02d %8s ", 
			ptm->tm_year - 100, ptm->tm_mon + 1, ptm->tm_mday,
			ptm->tm_hour, ptm->tm_min, ptm->tm_sec, LEVEL );

	vfprintf(m_fp, format, args);
	fprintf(m_fp, "\n");
	fflush(m_fp);

	va_end(args);
}

void Log::close()
{
	if(m_fp != NULL)
	{
		fclose(m_fp);
		m_fp = NULL;
		m_path[0] = '\0';
		m_head[0] = '\0';
		m_last = 0;
		m_reopen = REOPEN_DD;
	}
}

#ifdef _LOGTEST_
void sig_usr1(int signo)
{
}

int main()
{
	Log log;
	signal(SIGUSR1, sig_usr1);
	log.init(".", "log", Log::REOPEN_DD);
	while(1)
	{
		log.log("merong");
		pause();
	}
	return 0;
}
#endif
