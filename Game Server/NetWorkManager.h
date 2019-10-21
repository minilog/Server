#pragma once
#include "World.h"

class NetWorkManager
{
public:
	NetWorkManager();
	~NetWorkManager() {}

	vector<TCPSocketPtr> readBlockSockets;

	TCPSocketPtr socket_sever;
	bool isStart = false;
	vector<Packet> queue_packet;
	void Handle_Packet();
	void Handle_Exit(TCPSocketPtr sock);
	vector<World*> mListWorld_room;
	void DeleteWorld(int id);
	void CreateRoomAndAdd(TCPSocketPtr soc);

	void Update(float dt);
	void ProcessNewClient();
	void ReceivePacket();

private:
	void CreateSocketServer();

	//void SendTimeServerToClientSocket(TCPSocketPtr _socket);

	void Send_SyncTime(const TCPSocketPtr& _socket, int _NReceived);

	void Send_UpdateCountPlayer_OnLobby(const TCPSocketPtr& _socket);;

	void Receive_ChooseRoom(InputMemoryBitStream& _is, const TCPSocketPtr& _socket);

	void Handle_UpdateCountPlayer_OnRoom(InputMemoryBitStream& _is, const TCPSocketPtr& _socket);

	void Handle_PlayerOutRoon(const TCPSocketPtr& _socket);

	void Handle_RequestName(InputMemoryBitStream& _is, const TCPSocketPtr& _socket);
};

