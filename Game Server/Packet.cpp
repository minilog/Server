#include "Packet.h"
#include "MemoryBitStream.h"
#include "GameDefine.h"


Packet::Packet(InputMemoryBitStream &is, int room, int type)
{
	RoomID = room;
	PacketType = type;
}

Packet::~Packet()
{
}
