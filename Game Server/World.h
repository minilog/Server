#pragma once
#include <vector>
#include "NPC.h"

#include "TCPSocket.h"
#include "GameMap.h"
#include "Packet.h"
#include <map>
#include "UpgradeItem.h"
#include "ProtectPlayer.h"
#include "WorldState.h"
#include "Boss.h"
using namespace std;
class World
{
public:
	World();
	~World();
	void CreatePlayerAndSend();
	std::vector<Bullet*>  mListBullets;
	std::vector<Item*>  mListItems;
	std::string name = "";
	int last_time_add_item = 0;
	GameMap *mMap;
	std::vector<Boss*> mBoss;
	std::vector<TCPSocketPtr> listClient;
	std::vector<NPC*>  mListNPC;
	std::vector<PlayerServer*>  mListPlayer;
	std::vector<Entity* > mListObject;
	std::vector<WorldState> mListWorldState;
	vector<D3DXVECTOR2> mListPosition_Random;

	void Handle_Late_Packet(int id_of_worldState, int action, int id_player_late);
	D3DXVECTOR2 GetRandomPosition();
	int RandomNumber(int x, int y);
	int time_to_start = -1;
	int ID = 0;
	int Count_player_exit = 0;

	float time_remaining = 120;
	bool isDelete = false;
	bool isStart = false;
	int mCurrent_ID = 0;
	int count_brick_send = 0;
	void HandleObject(Packet p);
	Item* check_time_and_add_item();

	void CheckCollision(float dt);
	void Update(float dt);
	void SendWorld();
	void AddMember(TCPSocketPtr socket);
	void UpdatePlayerCount();
	void Handle_Exit(int id);
};

