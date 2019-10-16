#pragma once
#include <vector>
#include "PlayerServer.h"

class WorldState
{
public:
	std::vector<PlayerServer> mListPlayer;
	WorldState(std::vector<PlayerServer*> mListPlayer);
	~WorldState();
	void Update();
	bool Check_If_Late(int ID, int id_time_pack);
	void ReplaceNewVerSion(WorldState world);
};

