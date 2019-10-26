#pragma once
#include <vector>
#include "NPC.h"

#include "TCPSocket.h"
#include "GameMap.h"
#include "Packet.h"
#include <map>

using namespace std;
class Room
{
	int networkID = -1; // ID phòng dao động từ 0 - 3

	std::vector<TCPSocketPtr> clientList; // danh sách người chơi
public:
	Room(int _networkID) { networkID = _networkID; }
	~Room() {}

	void HandleDataFromClients(Packet _packet) {}
	void HandlePlayerExit(int _playerNetworkID) {}
	void AddPlayer(TCPSocketPtr _playerSocket)
	{
		// gán người chơi có ID phòng là ID phòng đó 
		_playerSocket->NetworkRoomID = networkID;

		_playerSocket->NetworkID = clientList.size();
		clientList.push_back(_playerSocket);
	}

	int GetNetworkID() { return networkID; }
	int GetNPlayer() { return (int)clientList.size(); }
};

