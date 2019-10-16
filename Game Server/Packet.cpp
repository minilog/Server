#include "Packet.h"
#include "MemoryBitStream.h"


Packet::Packet(InputMemoryBitStream &is, int room, int type)
{
	room_id = room;
	type_of_packet = type;
	switch (type_of_packet)
	{
	case Define::TankPacket:
		break;
	case Define::InputPacket:

		is.Read(id, Define::bitofID);
		is.Read(action, Define::bitofID);
		is.Read(last_time_id, Define::bitofID);
		break;
	}
}

Packet::~Packet()
{
}
