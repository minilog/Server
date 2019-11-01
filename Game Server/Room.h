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
	int ID = -1;
	bool isPlaying = false;
	int startingTime = -1;
	std::vector<TCPSocketPtr> clientList; // danh sách người chơi
public:
	bool Player0 = false;
	bool Player1 = false;
	bool Player2 = false;
	bool Player3 = false;
	bool Player0_Ready = false;
	bool Player1_Ready = false;
	bool Player2_Ready = false;
	bool Player3_Ready = false;

public:
	Room(int _networkID) { ID = _networkID; }
	~Room() {}

	void Write(OutputMemoryBitStream& _os);

	void HandlePlayerInput(Packet _packet) {}
	void HandlePlayerOutRoom(TCPSocketPtr _playerSocket);
	void HandlePlayerJoinRoom(TCPSocketPtr _playerSocket);
	void HandlePlayerReadyOrCancel(TCPSocketPtr _playerSocket);

	int GetID() { return ID; }
	int GetNPlayer() { return (int)clientList.size(); }
};

