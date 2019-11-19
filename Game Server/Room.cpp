#include "Room.h"

Room::Room(int _networkID)
{
	ID = _networkID;

	map = new GameMap("Resource files/map.tmx");

	for (int i = 0; i < 4; i++)
	{
		playerInRoomList.push_back(false);
		playerReadyList.push_back(false);
	}
}

void Room::Update(float dt)
{
	if (!isPlaying || !(GetTickCount() - startingTime >= time_StartGame))
		return;

	HandleShootList();
	HandleInputList();

	for (auto player : playerList)
	{
		player->ApplyVelocity();
	}

	for (auto player : playerList)
	{
		if (!player->IsDelete)
		{	// players va chạm npcs
			for (auto npc : npcList)
			{
				if (!npc->IsDelete)
				{
					if (GameCollision::IsCollideInNextFrame(player, npc, dt))
					{
						npc->CheckCollistion(player);
						player->ZeroVelocity();
						npc->ZeroVelocity();
					}
				}
			}

			for (auto player2 : playerList)
			{
				if (!player2->IsDelete && player->ID != player2->ID)
				{
					if (GameCollision::IsCollideInNextFrame(player, player2, dt))
					{
						player->CheckCollision(player2);
						player->ZeroVelocity();
						player2->ZeroVelocity();
					}
				}
			}
		}
	}

	for (auto player : playerList)
	{
		player->Update(dt);
	}

	for (auto brick : map->GetBrickList())
	{
		if (!brick->IsDelete)
		{
			// players va chạm bricks
			for (auto player : playerList)
			{
				player->CheckCollision(brick);
			}

			// bullets va chạm bricks
			for (auto bullet : bulletList)
			{
				if (!bullet->IsDelete)
				{
					if (GameCollision::IsCollideInNextFrame(bullet, brick, dt))
					{
						bullet->IsDelete = true;
						if (brick->Type == ET_NormalBrick)
							brick->IsDelete = true;
					}
				}
			}

			// npcs va chạm bricks
			for (auto npc : npcList)
			{
				if (!npc->IsDelete)
				{
					if (GameCollision::IsCollideInNextFrame(npc, brick, dt))
					{
						npc->ZeroVelocity();
					}
				}
			}
		}
	}

	for (auto bullet : bulletList)
	{
		if (!bullet->IsDelete)
		{
			// npcs va chạm bullets
			for (auto npc : npcList)
			{
				if (!npc->IsDelete)
				{
					if (GameCollision::IsCollideInNextFrame(npc, bullet, dt))
					{
						npc->ChangeHP(-1);
						bullet->IsDelete = true;
					}
				}
			}

			// players va chạm bullets
			for (auto player : playerList)
			{
				if (!player->IsDelete && player->ID != bullet->PlayerID)
				{
					if (GameCollision::IsCollideInNextFrame(player, bullet, dt))
					{
						player->ChangeHP(-1);
						bullet->IsDelete = true;
					}
				}
			}
		}
	}

	// npcs va chạm npcs
	for (auto npc : npcList)
	{
		if (!npc->IsDelete)
		{
			// npcs va chạm bullets
			for (auto npc2 : npcList)
			{
				if (!npc2->IsDelete && npc->ID != npc2->ID)
				{
					if (GameCollision::IsCollideInNextFrame(npc, npc2, dt))
					{
						npc->CheckCollistion(npc2);
						npc->ZeroVelocity();
						npc2->ZeroVelocity();
					}
				}
			}
		}
	}

	for (auto bullet : bulletList)
	{
		bullet->Update(dt);
	}
	for (auto npc : npcList)
	{
		npc->Update(dt);
	}

	// send world
	count++;
	if (count >= 3)
	{
		count = 0;

		for (auto client : clientList)
		{
			OutputMemoryBitStream os;
			
			os.Write(PT_World, NBit_PacketType);
			os.Write((int)GetTickCount(), NBit_Time); // write server time

			// gửi playerList
			for (auto player : playerList)
			{
				player->Write(os);
			}
			// gửi bulletList
			for (auto bullet : bulletList)
			{
				bullet->Write(os);
			}
			// gửi npc
			for (auto npc : npcList)
			{
				npc->Write(os);
			}
			// gửi brickNormalList
			for (auto brick : map->GetBrickNorList())
			{
				os.Write(brick->IsDelete);
			}
			os.Write(PT_World, NBit_PacketType);

			client->Send(os);
		}
	}
}

void Room::WriteUpdateRooms(OutputMemoryBitStream & _os)
{
	for (int i = 0; i < 4; i++)
	{
		_os.Write((bool)playerInRoomList[i]);
		_os.Write((bool)playerReadyList[i]);
	}
			
	_os.Write(isPlaying);
	if (isPlaying)
	{
		_os.Write(startingTime, NBit_Time);
	}
}

void Room::HandleInputList()
{
	// xử lý rollback di chuyển của players
	while (!pInputList.empty())
	{
		PlayerInput input = pInputList.at(pInputList.size() - 1);
		pInputList.pop_back();

		// xử lý chính
		{
			int nFramePrevious = (int)(((int)GetTickCount() - input.time) / 16.67f); // số frame đã trôi qua

			Player* player = nullptr; // xác định player gửi input
			for (auto p : playerList)
			{
				if (p->ID == input.playerID)
				{
					player = p;
					break;
				}
			}

			if (player->IsDelete)
				return;

			// nhận ngay tức thì => ko roll back
			if (nFramePrevious <= 0)
			{
				player->SetDirection(input.direction);
				player->ApplyVelocity();
				printf("Receive input from Player %i, Room %i, Dir = %i\n", input.playerID, ID, input.direction);
				return;
			}

			// không nhận các packet trễ
			if (nFramePrevious >= 16 || player->LastReceiveTime >= input.time)
				return;

			player->LastReceiveTime = input.time;

			printf("Receive input from Player %i, Room %i, Dir = %i\n", input.playerID, ID, input.direction);
			printf("(%i, %i)\n", (int)player->GetPosition().x, (int)player->GetPosition().y);
			
			player->SetPositionInPreviousFrame(nFramePrevious);
			player->SetDirection(input.direction);
			player->ApplyVelocity();

			// va chạm 1 lần trước
			for (auto brick : map->GetBrickList())
			{
				if (!brick->IsDelete)
				{
					player->CheckCollision(brick);
				}
			}

			// chạy frame liên tục cho đến hiện tại
			{
				for (int i = 0; i < nFramePrevious; i++)
				{
					player->ApplyVelocity();

					for (auto npc : npcList)
					{
						if (!npc->IsDelete)
						{
							if (GameCollision::IsCollideInNextFrame(player, npc, 1 / 60.0f))
							{
								player->ZeroVelocity();
							}
						}
					}

					for (auto player2 : playerList)
					{
						if (!player2->IsDelete && player->ID != player2->ID)
						{
							if (GameCollision::IsCollideInNextFrame(player, player2, 1 / 60.0f))
							{
								player->ZeroVelocity();
							}
						}
					}

					player->Update_Rollback(1 / 60.f);

					for (auto brick : map->GetBrickList())
					{
						if (!brick->IsDelete)
						{
							// player va chạm bricks
							player->CheckCollision(brick);
						}
					}

					printf("(%i, %i)\n", (int)player->GetPosition().x, (int)player->GetPosition().y);
				}
			}
		}
	}
}

void Room::HandleShootList()
{
	// xử lý rollback player bắn đạn
	while (!pShootList.empty())
	{
		PlayerShoot pShoot = pShootList.at(pShootList.size() - 1);
		pShootList.pop_back();

		// xử lý chính
		{
			int nFramePrevious = (int)(((int)GetTickCount() - pShoot.time) / 16.67f); // số frame đã trôi qua

			 // xác định player gửi input
			Player* player = nullptr;
			for (auto p : playerList)
			{
				if (p->ID == pShoot.playerID)
				{
					player = p;
					break;
				}
			}

			if (player->IsDelete)
				return;

			// nhận ngay tức thì => ko roll back
			if (nFramePrevious <= 0)
			{
				player->SpawnBulletInPreviousFrame(0);
				printf("Receive Shoot from Player %i, Room %i", pShoot.playerID, ID);
				return;
			}

			// không nhận các packet trễ
			if (nFramePrevious >= 16 || player->LastShootTime >= pShoot.time)
				return;

			player->LastShootTime = pShoot.time;

			printf("Receive Shoot from Player %i, Room %i", pShoot.playerID, ID);
			Bullet* bullet = player->SpawnBulletInPreviousFrame(nFramePrevious);

			// chạy frame liên tục cho đến hiện tại
			for (int i = 0; i < nFramePrevious; i++)
			{
				// bullet va chạm players
				for (auto player : playerList)
				{
					if (!player->IsDelete && bullet->PlayerID != player->ID)
					{
						if (GameCollision::IsCollideInNextFrame(bullet, player, 1 / 60.f))
						{
							player->ChangeHP(-1);
							bullet->IsDelete = true;
						}
					}
				}

				// bullet va chạm npcs
				for (auto npc : npcList)
				{
					if (!npc->IsDelete)
					{
						if (GameCollision::IsCollideInNextFrame(bullet, npc, 1 / 60.f))
						{
							npc->ChangeHP(-1);
							bullet->IsDelete = true;
						}
					}
				}

				// bullet va chạm bricks
				for (auto brick : map->GetBrickList())
				{
					if (!brick->IsDelete)
					{
						if (GameCollision::IsCollideInNextFrame(bullet, brick, 1 / 60.f))
						{
							bullet->IsDelete = true;
							if (brick->Type == ET_NormalBrick)
								brick->IsDelete = true;
						}
					}
				}

				// update
				bullet->Update(1 / 60.0f);
			}
		}
	}
}

void Room::ReceivePlayerInput(TCPSocketPtr _playerSocket, InputMemoryBitStream& _is)
{
	int receiveTime = 0;
	_is.Read(receiveTime, NBit_Time);

	Direction dir = D_Stand;
	_is.Read(dir, NBit_Direction);

	PlayerInput pInput;
	pInput.playerID = _playerSocket->PlayerID;
	pInput.direction = dir;
	pInput.time = receiveTime;

	pInputList.push_back(pInput);
}

void Room::ReceivePlayerShoot(TCPSocketPtr _playerSocket, InputMemoryBitStream & _is)
{
	int receiveTime = 0;
	_is.Read(receiveTime, NBit_Time);

	PlayerShoot pShoot;
	pShoot.playerID = _playerSocket->PlayerID;
	pShoot.time = receiveTime;

	pShootList.push_back(pShoot);
}

void Room::HandlePlayerOutRoom(TCPSocketPtr _playerSocket)
{
	int pID = _playerSocket->PlayerID;

	playerInRoomList[pID] = false;
	playerReadyList[pID] = false;

	_playerSocket->PlayerID = -1;
	_playerSocket->PlayerRoomID = -1;

	// pop back that client
	for (int i = 0; i < (int)clientList.size(); i++)
	{
		if (clientList[i] == _playerSocket)
		{
			clientList.erase(clientList.begin() + i);
		}
	}
}

void Room::HandlePlayerJoinRoom(TCPSocketPtr _playerSocket)
{
	if (isPlaying)
		return;

	for (int i = 0; i < 4; i++)
	{
		if (playerInRoomList[i] == false)
		{
			_playerSocket->PlayerID = i;
			playerInRoomList[i] = true;
			playerReadyList[i] = false;

			break;
		}
	}

	_playerSocket->PlayerRoomID = ID;

	clientList.push_back(_playerSocket);
}

void Room::HandlePlayerReadyOrCancel(TCPSocketPtr _playerSocket)
{
	if (isPlaying)
		return;

	int pID = _playerSocket->PlayerID;

	playerReadyList[pID] = !playerReadyList[pID];

	int nPlayers = 0;
	int nReady = 0;
	for (int i = 0; i < 4; i++)
	{
		nPlayers += playerInRoomList[i];
		nReady += playerReadyList[i];
	}

	if (nPlayers >= 1 && nPlayers == nReady)
	{
		isPlaying = true;
		startingTime = (int)GetTickCount();

		// tạo players
		for (int i = 0; i < 4; i++)
		{
			if (playerInRoomList[i] == true)
			{
				Player* player = new Player(i);
				playerList.push_back(player);

				// tạo 4 bullet cho mỗi player
				for (int j = 0; j < 4; j++)
				{
					Bullet* bullet = new Bullet(j, i);
					bulletList.push_back(bullet);
					player->AddBullet(bullet);
				}
			}
		}

		// tạo 3 NPCs
		for (auto i = 0; i < 3; i++)
		{
			NPC* npc = new NPC(i);
			npcList.push_back(npc);
		}

		printf("Game in room = %i start\n", ID);
	}
}

