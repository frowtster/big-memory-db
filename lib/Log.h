#ifndef _LOG_H_
#define _LOG_H_
#include <stdio.h>
#include <time.h>

class Log
{
public:
	enum loglevel_t
	{
		LEVEL_TRACE = 0,
		LEVEL_DEBUG,
		LEVEL_INFO,
		LEVEL_ERROR,
		LEVEL_CRITICAL
	};
	enum reopen_t
	{
		REOPEN_DD = 0,
		REOPEN_HH = 1,
		REOPEN_MM = 2
	};

private:
	loglevel_t mLoglevel;
public:
	char m_path[256]; 		// full path
	char m_head[32];		// prefix of file name
	FILE * m_fp;			// log file pointer
	time_t m_last;			// save last access time for reopen log file
	reopen_t m_reopen;
public:
	Log():
	m_fp(NULL),
	m_last(0),
	m_reopen(REOPEN_DD)
	{
		m_path[0] = '\0';
		m_head[0] = '\0';
	}
	~Log()
	{
		if(m_fp != NULL)
			fclose(m_fp);
	}
	int init(const char * path, const char * head, reopen_t reopen, loglevel_t level);
	void changeLogLevel( loglevel_t level );
	void log(const char * format, ...);
	void log( loglevel_t, const char * format, ...);
	int reopen(time_t now);
	bool checkTime(time_t now);
	void close();
};

#endif
