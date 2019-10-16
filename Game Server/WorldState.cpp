#include "WorldState.h"




WorldState::WorldState(std::vector<PlayerServer*> m_ListPlayer)
{
	for (auto pl : m_ListPlayer)
	{
		PlayerServer sv = PlayerServer();
		sv.SetPosition(pl->GetPosition());
		sv.mAction = pl->mAction;
		sv.last_id_packet = pl->last_id_packet;
		mListPlayer.push_back(sv);
	}

}

WorldState::~WorldState()
{
}

void WorldState::Update()
{
	for (auto pl : mListPlayer)
		pl.Update(3.0f / 60);
}

bool WorldState::Check_If_Late(int ID, int id)
{
	for (auto pl : mListPlayer)
		if (ID == pl.ID)
		{
			if (pl.last_id_packet > id)
				return true;
		}
	return false;
}

void WorldState::ReplaceNewVerSion(WorldState world)
{

	for (int i = 0; i < world.mListPlayer.size(); i++)
	{
		mListPlayer[i].SetPosition(world.mListPlayer[i].GetPosition());
	}
}
