#pragma once

#include "MemoryBitStream.h"

class Packet
{
public:
	Packet(InputMemoryBitStream &is, int room, int type);
	int PacketType = 0;
	int ID = 0;
	int RoomID = 0;
	int LastReceivedID = 0;
	~Packet();
};

