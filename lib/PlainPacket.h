
#include "Packet.h"

class PlainPacket : public Packet
{
public:
	PlainPacket();
	virtual int Parse();

};

