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

	void HandlePacket();
	void ReceivePacket();

// các hàm hỗ trợ
private:
	void CreateSocketServer();
	void Send_SyncTime(const TCPSocketPtr& _socket, int _NReceived);
	void Send_UpdatePlayers(const TCPSocketPtr& _socket);;
	void Receive_JoinRoom(InputMemoryBitStream& _is, const TCPSocketPtr& _socket);
	void Handle_UpdateCountPlayer_OnRoom(InputMemoryBitStream& _is, const TCPSocketPtr& _socket);
	void Handle_PlayerOutRoon(const TCPSocketPtr& _socket);
	void Handle_RequestName(InputMemoryBitStream& _is, const TCPSocketPtr& _socket);
	void HandlePlayerExit(TCPSocketPtr _playerSocket);
	void ProcessNewClient();
};

