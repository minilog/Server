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

	// tạo 3 NPCs
	for (auto i = 0; i < 3; i++)
	{
		NPC* npc = new NPC(i);
		npcList.push_back(npc);
	}

	// tạo 2 items
	protectItem = new ProtectItem(D3DXVECTOR2(0, 0));
	protectItem->IsDelete = true;
	upgradeItem = new UpgradeItem(D3DXVECTOR2(0, 0));
	upgradeItem->IsDelete = true;
}

void Room::Update(float dt)
{
	if (!isPlaying || !(GetTickCount() - startingTime >= time_StartGame))
		return;

	// xử lý data từ các client trước :P
	HandleInputList();
	HandleShootList();

	for (auto player : playerList)
	{
		player->ApplyVelocity();
	}

	for (auto player : playerList)
	{
		if (!player->IsDelete)
		{	
			for (auto npc : npcList) // players va chạm npcs
			{
				if (!npc->IsDelete)
				{
					if (GameCollision::IsCollideInNextFrame(player, npc, dt, 1))
					{
						player->ZeroVelocity();
					}
					if (GameCollision::IsCollideInNextFrame(npc, player, dt, 1))
					{
						npc->ZeroVelocity();
						npc->CheckCollision(player);
					}
				}
			}
			for (auto player2 : playerList) // players va chạm với nhau
			{
				if (!player2->IsDelete && player->ID != player2->ID && GameCollision::IsCollideInNextFrame(player, player2, dt, 1))
				{
					player->ZeroVelocity();
				}
			}

			// players va chạm items
			if (!protectItem->IsDelete &&
				GameCollision::IsCollideInNextFrame(player, protectItem, dt))
			{
					protectItem->IsDelete = true;
					player->ApplyShield();
					player->Score += 100;
					player->ScorePosition = protectItem->GetPosition();
			}
			if (!upgradeItem->IsDelete &&
				GameCollision::IsCollideInNextFrame(player, upgradeItem, dt))
			{
					upgradeItem->IsDelete = true;
					player->LevelUp();
					player->Score += 100;
					player->ScorePosition = upgradeItem->GetPosition();
			}
		}
	}

	// sau khi check vận tốc có bằng không hay không thì cập nhật
	for (auto player : playerList)
	{
		player->Update(dt);
	}

	for (auto brick : map->GetBrickList())
	{
		if (!brick->IsDelete)
		{
			for (auto player : playerList) 	// players va chạm bricks
			{
				player->CheckCollision(brick);
			}
			for (auto bullet : bulletList)  // bullets va chạm bricks
			{
				if (!bullet->IsDelete)
				{
					if (GameCollision::IsCollideInNextFrame(bullet, brick, dt))
					{
						if (brick->Type != ET_Water)
						{
							bullet->IsDelete = true;
							bullet->ApplyDestroyPosition();
						}
						if (brick->Type == ET_NormalBrick)
							brick->IsDelete = true;
					}
				}
			}
			for (auto npc : npcList) // npcs va chạm bricks
			{
				if (!npc->IsDelete)
				{
					if (GameCollision::IsCollideInNextFrame(npc, brick, dt, 1))
					{
						npc->ZeroVelocity();
						npc->CheckCollision(brick);
					}
				}
			}
		}
	}

	for (auto bullet : bulletList)
	{
		if (!bullet->IsDelete)
		{
			for (auto npc : npcList) // npcs va chạm bullets
			{
				if (!npc->IsDelete)
				{
					if (GameCollision::IsCollideInNextFrame(npc, bullet, dt))
					{
						npc->ChangeHP(-bullet->Damage);
						bullet->IsDelete = true;
						bullet->ApplyDestroyPosition();

						// nếu npc bị tiêu diệt thì thêm điểm cho players và set vị trí spawn điểm
						if (npc->IsDelete)
						{
							for (auto player : playerList)
							{
								if (player->ID == bullet->PlayerID)
								{
									player->Score += 100;
									player->ScorePosition = npc->GetPosition();
								}
							}
						}
					}
				}
			}

			for (auto player : playerList) 	// players va chạm bullets
			{
				if (!player->IsDelete && player->ID != bullet->PlayerID)
				{
					if (GameCollision::IsCollideInNextFrame(player, bullet, dt))
					{
						player->ChangeHP(-bullet->Damage);
						bullet->IsDelete = true;
						bullet->ApplyDestroyPosition();

						// nếu player bị tiêu diệt thì thêm điểm cho player và set vị trí spawn điểm
						if (player->IsDelete)
						{
							for (auto player2 : playerList)
							{
								if (player2->ID == bullet->PlayerID)
								{
									player2->Score += 100;
									player2->ScorePosition = player->GetPosition();
								}
							}
						}
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
			// npcs va chạm npcs
			for (auto npc2 : npcList)
			{
				if (!npc2->IsDelete &&
					npc->ID != npc2->ID &&
					GameCollision::IsCollideInNextFrame(npc, npc2, dt, 1))
				{
					npc->CheckCollision(npc2);	
					npc->ZeroVelocity();
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
	protectItem->Update(dt);
	upgradeItem->Update(dt);

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
			// gửi item
			protectItem->Write(os);
			upgradeItem->Write(os);
			// gửi pointed
			for (auto player : playerList)
			{
				if (player->ID == client->PlayerID)
				{
					player->WriteScorePosition(os);
				}
			}

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
		_os.Write((int)timeUp, NBit_Time);
	}
}

void Room::HandleInputList() // xử lý rollback di chuyển của players
{
	while (!pInputList.empty())
	{
		PlayerInput input = pInputList.at(pInputList.size() - 1);
		pInputList.pop_back();

		// xử lý chính
		{
			int tick = (int)GetTickCount();
			int NFramePre = (int)(((int)GetTickCount() - input.time + 8) / 15.f); // số frame đã trôi qua

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

			if (NFramePre <= 0) // nhận ngay tức thì => ko roll back
			{
				printf("Receive input from Player %i, Room %i, Dir = %i\n", input.playerID, ID, input.direction);
				player->SetDirection(input.direction);	
				player->LastReceiveTime = input.time;
				return;
			}

			// không nhận các packet trễ
			if (NFramePre >= 16 || player->LastReceiveTime >= input.time)
				return;

			printf("Receive input from Player %i, Room %i, Dir = %i\n", input.playerID, ID, input.direction);
			player->SetPositionInPreviousFrame(NFramePre);
			player->SetDirection(input.direction);
			player->LastReceiveTime = input.time;

			// va chạm với gạch 1 lần trước (đảm bảo position đúng)
			for (auto brick : map->GetBrickList())
			{
				if (!brick->IsDelete)
				{
					player->CheckCollision(brick);
				}
			}
			printf("(%i, %i)\n", (int)player->GetPosition().x, (int)player->GetPosition().y);

			// chạy frame liên tục cho đến hiện tại
			for (int i = 0; i < NFramePre; i++)
			{
				player->ApplyVelocity();

				for (auto npc : npcList) // player va chạm npcs
				{
					if (!npc->IsDelete &&
						GameCollision::IsCollideInNextFrame(player, npc, 1 / 60.0f, 1))
					{
						player->ZeroVelocity();
					}
				}

				for (auto p2 : playerList) // player va chạm players khác
				{
					if (!p2->IsDelete && player->ID != p2->ID && GameCollision::IsCollideInNextFrame(player, p2, 1 / 60.0f, 1))
					{
						player->ZeroVelocity();
					}
				}

				player->Update_Rollback(1 / 60.f);

				for (auto brick : map->GetBrickList()) // player va chạm bricks
				{
					if (!brick->IsDelete)
					{
						player->CheckCollision(brick);
					}
				}

				printf("(%i, %i)\n", (int)player->GetPosition().x, (int)player->GetPosition().y);
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
				printf("Receive Shoot from Player %i, Room %i\n", pShoot.playerID, ID);
				return;
			}

			// không nhận các packet trễ
			if (nFramePrevious >= 16 || player->LastShootTime >= pShoot.time)
				return;

			player->LastShootTime = pShoot.time;

			printf("Receive Shoot from Player %i, Room %i\n", pShoot.playerID, ID);
			Bullet* bullet = player->SpawnBulletInPreviousFrame(nFramePrevious);
			if (bullet == nullptr)
				return;	


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
							player->ChangeHP(-bullet->Damage);
							bullet->IsDelete = true;
							bullet->ApplyDestroyPosition();

							// nếu player bị tiêu diệt thì thêm điểm cho player và set vị trí spawn điểm
							if (player->IsDelete)
							{
								for (auto player2 : playerList)
								{
									if (player2->ID == bullet->PlayerID)
									{
										player2->Score += 100;
										player2->ScorePosition = player->GetPosition();
									}
								}
							}
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
							npc->ChangeHP(-bullet->Damage);
							bullet->IsDelete = true;
							bullet->ApplyDestroyPosition();

							// nếu npc bị tiêu diệt thì thêm điểm cho players và set vị trí spawn điểm
							if (npc->IsDelete)
							{
								for (auto player : playerList)
								{
									if (player->ID == bullet->PlayerID)
									{
										player->Score += 100;
										player->ScorePosition = npc->GetPosition();
									}
								}
							}
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
							if (brick->Type != ET_Water)
							{
								bullet->IsDelete = true;
								bullet->ApplyDestroyPosition();
							}
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

		printf("Game in room = %i start\n", ID);
	}
}
