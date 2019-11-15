#pragma once
#include "Room.h"

class NetWorkManager
{
	TCPSocketPtr socketServer;
	vector<TCPSocketPtr> clientSockets;
	vector<Room*> roomList;
	vector<Packet> packetQueues;

public:
	NetWorkManager();
	~NetWorkManager() {}
	void Update(float dt, double _time = 0.0f);
	void ReceivePacket();
};

