#pragma once
#include "World.h"

class NetWorkManager
{
public:
	vector<TCPSocketPtr> readBlockSockets;
	vector<TCPSocketPtr> readableSockets;
	//vector<vector<WorldState>> mListWorldState;



	TCPSocketPtr socket_sever;
	bool isStart = false;
	void CreatePlayerAndSend();
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
	void UpdatePlayerCount();
	void ReceivePacket();

};

