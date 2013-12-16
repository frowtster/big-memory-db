#ifdef _WIN32
#include <windows.h>

#endif

#pragma once

#define MAX_LINE_LENGTH 512

class FileIni
{
public:
	static size_t GetPrivateProfileStr(
		const char * lpAppName,
		const char * lpKeyName,
		const char * lpDefault,
		char * lpReturnedString,
		size_t nSize,
		const char * lpFileName
	);

};
