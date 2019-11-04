#pragma once
#include <vector>
#include "NPC.h"

#include "TCPSocket.h"
#include "GameMap.h"
#include "Packet.h"
#include <map>
#include "Player.h"

using namespace std;
class Room
{
	int ID = -1;
	bool isPlaying = false;
	int startingTime = -1;
	std::vector<TCPSocketPtr> clientList;
	const float time_StartGame = 3000;

	// IN BATTLE
	GameMap* map;
	vector<Player*> playerList;

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
	Room(int _networkID);
	~Room() {}
	void Update(float _dt);
	void WriteUpdateRooms(OutputMemoryBitStream& _os);
	void HandlePlayerInput(TCPSocketPtr _playerSocket, InputMemoryBitStream& _is);
	void HandlePlayerOutRoom(TCPSocketPtr _playerSocket);
	void HandlePlayerJoinRoom(TCPSocketPtr _playerSocket);
	void HandlePlayerReadyOrCancel(TCPSocketPtr _playerSocket);
	int GetID() { return ID; }
	int GetNPlayer() { return (int)clientList.size(); }

private:
	int count = 0;
};

