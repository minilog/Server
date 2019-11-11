#pragma once
#include <vector>
#include "NPC.h"

#include "TCPSocket.h"
#include "GameMap.h"
#include "Packet.h"
#include <map>
#include "Player.h"

struct PlayerInput
{
	int playerID = 0;
	Direction direction = D_Stand;
	int time = 0;
};

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
	vector<PlayerInput*> pInputList;

public:
	vector<bool> playerInRoomList;
	vector<bool> playerReadyList;

public:
	Room(int _networkID);
	~Room() {}
	void Update(float _dt);
	void WriteUpdateRooms(OutputMemoryBitStream& _os);
	void HandlePlayerInput(TCPSocketPtr _playerSocket, InputMemoryBitStream& _is);
	void HandlePlayerOutRoom(TCPSocketPtr _playerSocket);
	void HandlePlayerJoinRoom(TCPSocketPtr _playerSocket);
	void HandlePlayerReadyOrCancel(TCPSocketPtr _playerSocket);
	void HandleInputs();
	int GetID() { return ID; }
	int GetNPlayer() { return (int)clientList.size(); }

private:
	int count = 0;
};
