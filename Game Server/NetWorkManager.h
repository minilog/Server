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
	void Update(float dt);
	void ReceivePacket();

private:
	void CreateServerSocket();
};

