#pragma once

#include "Packet.h"
#include "Defines.h"

class OmniPacket : public Packet
{
public:
	char mCmd[CMDLEN+1];
	char *mBody;
	unsigned long mTrid;
	char mHeader[BUFLEN];
	unsigned long mBodySize;

	int mbNewLineOK;
	unsigned int mHeaderLen;

protected:
	unsigned int mTridLen;
	unsigned int mSizeLen;

public:
	OmniPacket();
	~OmniPacket();
	virtual int Parse();
	virtual int Serialize();

	virtual void Clear();

};

