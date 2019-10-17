#pragma once
#include "World.h"

class NetWorkManager
{
public:
	vector<TCPSocketPtr> readBlockSockets;

	TCPSocketPtr socket_sever;
	bool isStart = false;
	vector<Packet> queue_packet;
	void Handle_Packet();
	void Handle_Exit(TCPSocketPtr sock);
	vector<World*> mListWorld_room;
	void DeleteWorld(int id);
	void CreateRoomAndAdd(TCPSocketPtr soc);
	NetWorkManager();
	~NetWorkManager();
	void Update(float dt);
	void ProcessNewClient();
	void ReceivePacket();

private:
	void CreateSocketServer();

	//void SendTimeServerToClientSocket(TCPSocketPtr _socket);

	void Send_SyncTimePacket(const TCPSocketPtr& _socket, int _NReceived);
};

