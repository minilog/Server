#include "Packet.h"
#include "MemoryBitStream.h"
#include "Define.h"


Packet::Packet(InputMemoryBitStream &is, int room, int type)
{
	RoomID = room;
	PacketType = type;
	switch (PacketType)
	{
	case Define::TankPacket:
		break;
	case Define::InputPacket:

		is.Read(ID, 11);
		is.Read(LastReceivedID, 11);
		break;
	}
}

Packet::~Packet()
{
}
