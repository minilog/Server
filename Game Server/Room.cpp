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

void Room::Update(float _dt, double _time)
{
	//int tickThisFrame = (int)GetTickCount();

	if (!isPlaying || !(GetTickCount() - startingTime >= time_StartGame))
		return;

	HandleShoots();

	HandleInputs(_time);

	for (auto brick : map->GetBrickList())
	{
		if (!brick->IsDelete)
		{
			// players va chạm bricks
			for (auto player : playerList)
			{
				if (GameCollision::IsCollideInNextFrame(player, brick, _dt))
				{
					player->MakeCollision(brick);
				}
			}

			// bullets va chạm bricks
			{
				for (auto bullet : bulletList)
				{
					if (GameCollision::IsCollideInNextFrame(bullet, brick, _dt))
					{
						bullet->MakeCollision(brick);
						brick->MakeCollision(bullet);
					}
				}
			}

			// npc va chạm bricks
			if (npc->IsDelete == false)
			{
				if (GameCollision::IsCollideInNextFrame(npc, brick, _dt))
				{
					npc->MakeCollision(brick);
				}
			}
		}
	}

	// update
	for (auto bullet : bulletList)
	{
		bullet->Update(_dt);
	}
	for (auto player : playerList)
	{
		player->Update(_dt);
	}
	npc->Update(_dt);

	// send world
	count++;
	if (count >= 4)
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
			npc->Write(os);

			// gửi brickNormalList
			for (auto brick : map->GetBrickNorList())
			{
				os.Write(brick->IsDelete);
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
	}
}

void Room::HandleInputs(double _time)
{
	// xử lý rollback di chuyển của players
	while (!pInputList.empty())
	{
		PlayerInput input = pInputList.at(pInputList.size() - 1);
		pInputList.pop_back();

		// xử lý chính
		{
			int nFramePrevious = (int)((_time + 0.003f - (double)input.time) / (double)(1000 / 60.f)); // số frame đã trôi qua

			Player* player = nullptr; // xác định player gửi input
			for (auto p : playerList)
			{
				if (p->ID == input.playerID)
				{
					player = p;
					break;
				}
			}

			// nhận ngay tức thì => ko roll back
			if (nFramePrevious <= 0)
			{
				player->SetDirection(input.direction);
				printf("Receive input from Player %i, Room %i, Dir = %i\n", input.playerID, ID, input.direction);
				return;
			}

			// không nhận các packet trễ
			if (nFramePrevious >= 30 || player->LastReceiveTime >= input.time)
				return;

			player->LastReceiveTime = input.time;

			printf("Receive input from Player %i, Room %i, Dir = %i\n", input.playerID, ID, input.direction);
			printf("(%i, %i)\n", (int)player->GetPosition().x, (int)player->GetPosition().y);
			
			player->SetPositionInPreviousFrame(nFramePrevious);
			player->SetDirection(input.direction);

			// chạy frame liên tục cho đến hiện tại
			{
				for (int i = 0; i < nFramePrevious; i++)
				{
					// va chạm các viên gạch với player này
					for (auto brick : map->GetBrickList())
					{
						if (!brick->IsDelete)
						{
							if (GameCollision::IsCollideInNextFrame(player, brick, 1 / 60.f))
							{
								player->MakeCollision(brick);
							}
						}
					}

					// update
					player->Update_Rollback(1 / 60.f);

					printf("(%i, %i)\n", (int)player->GetPosition().x, (int)player->GetPosition().y);
				}
			}
		}
	}
}

void Room::HandleShoots()
{
	// xử lý rollback player bắn đạn
	while (!pShootList.empty())
	{
		PlayerShoot pShoot = pShootList.at(pShootList.size() - 1);
		pShootList.pop_back();

		// xử lý chính
		{
			int nFramePrevious = (int)(((int)GetTickCount() - pShoot.time) / (1000.f / 60.f)); // số frame đã trôi qua

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

			// nhận ngay tức thì => ko roll back
			if (nFramePrevious <= 0)
			{
				player->SpawnBulletInPreviousFrame(0);
				printf("Receive Shoot from Player %i, Room %i", pShoot.playerID, ID);
				return;
			}

			// không nhận các packet trễ
			if (nFramePrevious >= 30 || player->LastShootTime >= pShoot.time)
				return;

			player->LastShootTime = pShoot.time;

			printf("Receive Shoot from Player %i, Room %i", pShoot.playerID, ID);
			Bullet* bullet = player->SpawnBulletInPreviousFrame(nFramePrevious);

			// chạy frame liên tục cho đến hiện tại
			for (int i = 0; i < nFramePrevious; i++)
			{
				for (auto brick : map->GetBrickList())
				{
					if (!brick->IsDelete)
					{
						if (bullet->IsDelete == false)
						{
							// bullet va chạm bricks
							if (GameCollision::IsCollideInNextFrame(bullet, brick, 1 / 60.f))
							{
								bullet->MakeCollision(brick);
								brick->MakeCollision(bullet);
							}
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

		// khởi tạo player
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

		// khởi tạo NPC
		npc = new NPC(0);

		printf("Game in room = %i start\n", ID);
	}
}

