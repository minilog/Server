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

void Room::Update(float _dt)
{
	if (!isPlaying || !(GetTickCount() - startingTime >= time_StartGame))
		return;

	HandleInputs();

	for (auto brick : map->GetBrickList())
	{
		if (!brick->IsDelete)
		{
			for (auto player : playerList)
			{
				if (GameCollision::IsCollideInNextFrame(player, brick, _dt))
				{
					player->MakeCollision(brick);
				}
			}
		}
	}

	// update
	for (auto player : playerList)
	{
		player->Update(_dt);
	}

	// send world
	count++;
	if (count >= 3)
	{
		for (auto client : clientList)
		{
			OutputMemoryBitStream os;
			
			os.Write(PT_PlayerInput, NBit_PacketType);
			os.Write((int)GetTickCount(), NBit_Time); // write server time

			for (auto player : playerList)
			{
				player->Write(os);
			}

			client->Send(os);
		}

		count = 0;
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


void Room::HandleInputs()
{
	while (!pInputList.empty())
	{
		PlayerInput* input = pInputList.at(pInputList.size() - 1);

		{
			int nFramePrevious = (int)(((int)GetTickCount() - input->time) / (1000.f / 60.f)); // số frame đã trôi qua

			Player* player = nullptr; // xác định player gửi input
			for (auto p : playerList)
			{
				if (p->ID == input->playerID)
				{
					player = p;
					break;
				}
			}

			if (nFramePrevious <= 0)
			{
				player->SetDirection(input->direction);
				printf("Receive input from Player %i, Room %i, Dir = %i\n", input->playerID, ID, input->direction);
				return;
			}

			// không nhận các packet trễ
			if (nFramePrevious >= 30 || player->LastReceiveTime >= input->time)
				return;

			{
				printf("Receive input from Player %i, Room %i, Dir = %i\n", input->playerID, ID, input->direction);

				player->LastReceiveTime = input->time;

				player->SetPositionInPreviousFrame(nFramePrevious);
				player->SetDirection(input->direction);

				printf("(%i, %i)\n", (int)player->GetPosition().x, (int)player->GetPosition().y);
			}

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
					player->Update(1 / 60.f);

					printf("(%i, %i)\n", (int)player->GetPosition().x, (int)player->GetPosition().y);
				}
			}
		}

		pInputList.pop_back();
		delete input;
	}
}

void Room::HandlePlayerInput(TCPSocketPtr _playerSocket, InputMemoryBitStream& _is)
{
	int receiveTime = 0;
	_is.Read(receiveTime, NBit_Time);

	Direction dir = D_Stand;
	_is.Read(dir, NBit_Direction);

	PlayerInput* pInput = new PlayerInput();
	pInput->playerID = _playerSocket->PlayerID;
	pInput->direction = dir;
	pInput->time = receiveTime;

	pInputList.push_back(pInput);
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

		// init players
		for (int i = 0; i < 4; i++)
		{
			if (playerInRoomList[i] == true)
			{
				Player* player = new Player(i);
				playerList.push_back(player);
			}
		}

		printf("Game in room = %i start\n", ID);
	}
}

