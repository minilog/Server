#pragma once
class InputMemoryBitStream;
class Packet
{
public:
	Packet(InputMemoryBitStream &is, int room, int type);
	int type_of_packet = 0;
	int id = 0;
	int action = 0;
	int last_time_id = 0;
	int room_id = 0;
	~Packet();
};

