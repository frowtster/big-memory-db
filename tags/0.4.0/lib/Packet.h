#pragma once

#include "Defines.h"

class Packet
{
public:
	char *mBuffer;
	Packet();
	virtual int Parse() = 0;
	virtual int Serialize(){return TRUE;};
	virtual void Clear(){};
	void Release();
	char *GetBuffer();
};

