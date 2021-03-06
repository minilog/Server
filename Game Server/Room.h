﻿#pragma once
#include <vector>
#include "NPC.h"

#include "TCPSocket.h"
#include "GameMap.h"
#include "Packet.h"
#include <map>
#include "Player.h"
#include "Bullet.h"
#include "NPC.h"
#include "ProtectItem.h"
#include "UpgradeItem.h"

struct PlayerInput
{
	int playerID = -1;
	Direction direction = Direction(-1);
	int time = -1;
};

struct PlayerShoot
{
	int playerID = -1;
	int time = -1;
};

using namespace std;
class Room
{
	int ID = -1;
	bool isPlaying = false;
	int startingTime = -1;
	std::vector<TCPSocketPtr> clientList;
	const float time_StartGame = 3000;
	const float timeUp = 10000;
	float count_TimeUp = 10000;

	// IN BATTLE
	GameMap* map;
	vector<Player*> playerList;
	vector<Bullet*> bulletList;
	vector<PlayerInput> pInputList;
	vector<PlayerShoot> pShootList;
	vector<NPC*> npcList;
	ProtectItem* protectItem;
	UpgradeItem* upgradeItem;

public:
	vector<bool> playerInRoomList;
	vector<bool> playerReadyList;

public:
	Room(int networkID);
	~Room() {}
	void Update(float dt);
	void WriteUpdateRooms(OutputMemoryBitStream& _os);
	void ReceivePlayerInput(TCPSocketPtr _playerSocket, InputMemoryBitStream& _is);
	void ReceivePlayerShoot(TCPSocketPtr _playerSocket, InputMemoryBitStream& _is);
	void HandlePlayerOutRoom(TCPSocketPtr _playerSocket);
	void HandlePlayerJoinRoom(TCPSocketPtr _playerSocket);
	void HandlePlayerReadyOrCancel(TCPSocketPtr _playerSocket);
	int GetID() { return ID; }
	int GetNPlayer() { return (int)clientList.size(); }

private:
	int count = 0;
	void HandleInputList();
	void HandleShootList();
};
