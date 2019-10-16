#include "World.h"
//#include "Bullet.h"


World::World()
{
	{
		mListBullets.push_back(new Bullet(11, 1));
		mListBullets.push_back(new Bullet(12, 1));
		mListBullets.push_back(new Bullet(13, 1));
		mListBullets.push_back(new Bullet(14, 1));
		mListBullets.push_back(new Bullet(21, 2));
		mListBullets.push_back(new Bullet(22, 2));
		mListBullets.push_back(new Bullet(23, 2));
		mListBullets.push_back(new Bullet(24, 2));
		mListBullets.push_back(new Bullet(31, 3));
		mListBullets.push_back(new Bullet(32, 3));
		mListBullets.push_back(new Bullet(33, 3));
		mListBullets.push_back(new Bullet(34, 3));
		mListBullets.push_back(new Bullet(41, 4));
		mListBullets.push_back(new Bullet(42, 4));
		mListBullets.push_back(new Bullet(43, 4));
		mListBullets.push_back(new Bullet(44, 4));
	}
	mMap = new GameMap("Resource files/map.tmx");
	int temp = 0;
	for (int i = 0; i < 50; i++)
	{
		int x = 0;
		int y = 0;
		if (temp == 0)
		{
			x = RandomNumber(533, 624);
			y = RandomNumber(48, 77);
		}
		else if (temp == 1)
		{
			x = RandomNumber(508, 562);
			y = RandomNumber(678, 724);
		}
		else if (temp == 2)
		{
			x = RandomNumber(458, 512);
			y = RandomNumber(200, 285);
		}
		else if (temp == 3)
		{
			x = RandomNumber(287, 484);
			y = RandomNumber(375, 405);
		}
		else if (temp == 4)
		{
			x = RandomNumber(222, 272);
			y = RandomNumber(614, 727);
			temp = 0;
		}
		temp++;
		mListPosition_Random.push_back(D3DXVECTOR2(x, y));
	}

	for (int i = 1; i < 5; i++)
	{
		NPC *npc = new NPC();
		npc->ID = i;
		npc->SetPosition(GetRandomPosition());
		mListNPC.push_back(npc);
	}
	mBoss.push_back(new Boss(1, D3DXVECTOR2(68, 462)));
	mBoss.push_back(new Boss(2, D3DXVECTOR2(392, 727)));
	mBoss.push_back(new Boss(3, D3DXVECTOR2(412, 47)));
	mBoss.push_back(new Boss(4, D3DXVECTOR2(740, 462)));
}


World::~World()
{
}

void World::CreatePlayerAndSend()
{
	mListPlayer.clear();
	OutputMemoryBitStream os1;
	os1.Write(Define::LetStart, Define::bitofTypePacket);
	os1.Write(listClient.size(), Define::bitofTypePacket);
	for (auto ele : listClient)
	{

		PlayerServer* pl = new PlayerServer(ele->ID);
		pl->SetPosition(GetRandomPosition());
		pl->mAction = Action::GoRight;
		pl->mName = ele->name;
		pl->ActiveShield();
		mListPlayer.push_back(pl);
		pl->Write(os1);
		os1.Write(pl->mName);

	}
	for (auto ele : listClient)
		ele->Send(os1.GetBufferPtr(), os1.GetByteLength());

}

void World::HandleObject(Packet p)
{
	int ID = p.id;
	int action = p.action;
	int last_time_id = p.last_time_id;
	/*for(int i=0;i<mListWorldState.size();i++)
	{
		if(mListWorldState[i].Check_If_Late(ID,last_time_id))
		{
			Handle_Late_Packet(i - 1,action,ID);
			break;
		}
	}*/
	for (auto ele : mListPlayer)
	{
		if (ele->ID == ID)
		{
			ele->OnChangeAction((Action)action);
			ele->last_id_packet = last_time_id;
			if (ele->mAction == Fight)
			{
				ele->Up_ID_OfBullet();
				for (auto bullet : mListBullets)
				{
					if (bullet->ID == ele->ID_currentBullet)
					{

						bullet->SetPosition(ele->GetPosition());
						bullet->SetActive(true);
						bullet->setMoveDirection(ele->dir);
						break;
					}
				}
			}
			break;
		}
	}
}

Item* World::check_time_and_add_item()
{
	if (GetTickCount() - last_time_add_item > 10000)
	{
		Item* item;
		int type = rand() + 100;
		D3DXVECTOR2 ran = GetRandomPosition();
		if (type % 2 == 0) item = new ProtectPlayer(D3DXVECTOR3(ran));
		else item = new UpgradeItem(D3DXVECTOR3(ran));
		mListItems.push_back(item);

		last_time_add_item = GetTickCount();
		return item;
	}
	return nullptr;
}



void World::CheckCollision(float dt)
{



	for (auto pl : mListPlayer)
	{
		vector<Entity*> listCollision;
		mMap->GetQuadTree()->getEntitiesCollideAble(listCollision, pl);
		for (auto brick : listCollision)
		{
			if (brick->isDelete) continue;
			if (GameCollision::isCollide(pl, brick, dt))
				pl->CollideWith_World();
		}

		for (auto item : mListItems)
		{
			if (GameCollision::isCollide(pl, item, dt))
			{
				pl->CollisionWith(item);
				item->BeCollideWith_Player();
			}
		}
		for (auto player_another : mListPlayer)
		{
			if (player_another->ID == pl->ID) continue;
			if (GameCollision::isCollide(pl, player_another, dt))
			{
				pl->CollisionWith(player_another);
				player_another->CollisionWith(pl);
			}
		}
		for (auto npc : mListNPC)
		{
			if (GameCollision::isCollide(pl, npc, dt))
			{
				pl->CollisionWith(npc);
				npc->CollisionWith(pl);
			}
		}

	}

	//Check bullet colli voi map object
	for (auto bl : mListBullets)
	{
		if (!bl->isActive) continue;
		vector<Entity*> listCollision;
		mMap->GetQuadTree()->getEntitiesCollideAble(listCollision, bl);
		for (auto brick : listCollision)
		{
			if (brick->isDelete) continue;
			if (GameCollision::isCollide(bl, brick, dt))
			{
				bl->CollisionWith(brick);
				bl->isChange = true;


				for (auto brick2 : mMap->GetListBrick())
				{
					if (brick2->ID == brick->ID)
					{
						brick2->CollisionWith(bl);
						count_brick_send++;
						break;
					}
				}
				//break;


			}
		}
		for (auto pl : mListPlayer)
		{
			if (bl->ID_of_player == pl->ID) continue;
			if (GameCollision::isCollide(bl, pl, dt))
			{

				pl->CollisionWith(bl);
				bl->CollisionWith(pl);

				if (pl->mHeal == 0)
				{//Check bullet cua Player nao
					mListPlayer.at(bl->ID_of_player - 1)->mScore += 500;
					mListPlayer.at(bl->ID_of_player - 1)->mScore_send = 500;
				}

				bl->isChange = true;
			}
		}
		for (auto npc : mListNPC)
		{

			if (GameCollision::isCollide(bl, npc, dt))
			{

				npc->CollisionWith(bl);
				bl->CollisionWith(npc);

				if (npc->mHeal == 0)
				{//Check bullet cua Player nao
					mListPlayer.at(bl->ID_of_player - 1)->mScore += 300;
					mListPlayer.at(bl->ID_of_player - 1)->mScore_send = 300;
					mListPlayer.at(bl->ID_of_player - 1)->position_add = npc->GetPosition();
				}

				bl->isChange = true;
			}
		}
		for (auto boss : mBoss)
		{

			if (GameCollision::isCollide(bl, boss, dt))
			{
				boss->mHP--;
				if (boss->mHP <= 0) {
					boss->SetPosition(-100, -100);
					for (auto pl : mListPlayer)
						if (pl->ID == boss->ID)
						{
							pl->SetPosition(-100, -100);
							pl->isLose = true;
						}
				}
				bl->CollisionWith(boss);
				bl->isChange = true;
			}
		}
	}

	for (auto npc : mListNPC)
	{
		vector<Entity*> listCollision;
		mMap->GetQuadTree()->getEntitiesCollideAble(listCollision, npc);
		for (auto brick : listCollision)
		{
			if (brick->isDelete) continue;
			if (GameCollision::isCollide(npc, brick, dt))
				npc->CollideWith_World();
		}
		for (auto npc2 : mListNPC)
		{
			if (npc2->ID == npc->ID) continue;
			if (GameCollision::isCollide(npc2, npc, dt))
			{
				npc2->CollisionWith(npc);
				npc->CollisionWith(npc2);
			}
		}
	}






}
void World::Update(float dt)
{
	if (isStart)
	{
		time_remaining -= dt;
		if (time_remaining < 0)
		{

			for (auto cl : listClient)
			{
				OutputMemoryBitStream os;
				os.Write(Define::GameOver, Define::bitofTypePacket);
				cl->Send(os.GetBufferPtr(), os.GetByteLength());
			}
			if (Count_player_exit > listClient.size())
			{
				Count_player_exit = 0;
				isStart = false;
				listClient.clear();
				for (auto boss : mBoss)
					boss->mHP = 10;
				time_remaining = 120;
				isStart = false;
				isDelete = true;
				mCurrent_ID = 0;
			}

			return;
		}
	}
	if (GetTickCount() - time_to_start > 1000 && time_to_start != -1)
	{
		isStart = true;
		CreatePlayerAndSend();
		time_to_start = -1;
	}


	for (auto ele : mListPlayer)
	{
		ele->Update(dt);
	}
	for (auto ele : mListNPC)
	{
		ele->Update(dt);
	}
	for (auto ele : mListBullets)
	{
		ele->Update(dt);
	}
	for (int i = 0; i < mListItems.size(); i++)
	{
		mListItems[i]->Update(dt);
		if (mListItems[i]->getDelete())
		{
			mListItems.erase(mListItems.begin() + i);
			i--;
		}
	}


}

void World::SendWorld()
{
	if (!isStart) return;
	OutputMemoryBitStream os;
	os.Write(Define::WorldStatePacket, Define::bitofTypePacket);

	Item* add = check_time_and_add_item();

	int count_bullet_send = 0;
	for (auto ele : mListBullets)
	{
		if (ele->isChange)
		{
			count_bullet_send++;
		}
	}

	int size = mListPlayer.size() + count_brick_send + count_bullet_send + mListNPC.size() + mBoss.size();
	if (add != nullptr) size += 1;

	os.Write(size, Define::bitofID);
	if (add != nullptr)
		add->Write(os);
	for (auto ele : mBoss)
		ele->Write(os);
	for (auto ele : mListPlayer)
	{
		ele->Write(os);
	}
	for (auto ele : mListNPC)
	{
		ele->Write(os);
	}

	for (auto ele : mListBullets)
	{
		if (ele->isChange)
		{
			ele->Write(os);
			if (!ele->isActive)
				int l = 0;

		}
	}
	for (auto ele : mMap->GetListBrick())
	{
		if (ele->isDelete)
		{
			ele->Write(os);
		}

	}

	for (auto client : listClient)
	{
		if (client->ID == 0) continue;
		int byte_send = client->Send(os.GetBufferPtr(), os.GetByteLength());
		if (byte_send < 0)
			printf("Send error");

	}
	//Add a State World to the list
	//WorldState world(mListPlayer);
	//mListWorldState.push_back(world);
	//if (mListWorldState.size() >= 10) 
	//	mListWorldState.erase(mListWorldState.begin());

	//Delete Map if Object's delete = true
	mMap->Update(0);
	count_brick_send = 0;

	//reset state bullet
	for (auto ele : mListBullets)
	{
		ele->isChange = false;
	}
}

void World::AddMember(TCPSocketPtr socket)
{
	mCurrent_ID++;
	socket->ID = mCurrent_ID;
	listClient.push_back(socket);

	OutputMemoryBitStream os;
	os.Write(Define::WelcomePacket, Define::bitofTypePacket);
	os.Write(socket->ID, Define::bitofID);
	socket->Send(os.GetBufferPtr(), os.GetByteLength());


	if (mCurrent_ID == 2) //if enought player, Provide them first position by ID
		time_to_start = GetTickCount(); //Wait last player


}

void World::UpdatePlayerCount()
{

	for (auto ele : listClient)
	{
		OutputMemoryBitStream os1;
		os1.Write(Define::UpdateCountPlayer, Define::bitofTypePacket);
		os1.Write(listClient.size(), Define::bitofID);
		os1.Write(name);
		ele->Send(os1.GetBufferPtr(), os1.GetByteLength());
	}

}

void World::Handle_Exit(int id)
{
	for (int i = 0; i < listClient.size(); i++)
	{
		if (listClient[i]->ID == id)
		{
			listClient.erase(listClient.begin() + i);
			break;
		}
	}
	if (listClient.size() == 0)
	{
		isDelete = true;
		isStart = false;
		mCurrent_ID = 0;
	}
}


void World::Handle_Late_Packet(int id_of_worldState, int action, int id_player_late)
{
	mListWorldState[id_of_worldState].mListPlayer[id_player_late - 1].mAction = (Action)action;
	mListWorldState[id_of_worldState].Update();
	for (int i = id_of_worldState + 1; i < mListWorldState.size(); i++)
	{
		mListWorldState[i].ReplaceNewVerSion(mListWorldState[i - 1]);
		mListWorldState[i].Update();
	}
	for (int i = 0; i < mListPlayer.size(); i++)
		mListPlayer[i]->SetPosition(mListWorldState[mListWorldState.size()].mListPlayer[i].GetPosition());
}

D3DXVECTOR2 World::GetRandomPosition()
{
	int i = RandomNumber(1, 49);
	return mListPosition_Random[i];
}

int World::RandomNumber(int x, int y)
{
	return rand() % (y - x + 1) + x;
}

