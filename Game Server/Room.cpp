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

void Room::HandlePlayerInput(TCPSocketPtr _playerSocket, InputMemoryBitStream& _is)
{
	Direction dir = D_Stand;

	_is.Read(dir, NBit_Direction);

	for (auto player : playerList)
	{
		if (player->ID == _playerSocket->PlayerID)
		{
			player->SetDirection(dir);
		}
	}

	{
		//int sentTime = 0;
		//Direction dir = D_Stand;

		//_is.Read(sentTime, NBit_Time);
		//_is.Read(dir, NBit_Direction);

		//int myTime = (int)GetTickCount();
		//int nFramePrevious = (int)((myTime - sentTime) / (1000.f / 60.f)); // đã bao nhiêu frame trôi qua từ lúc client gửi

		//Player* p = playerList[_playerSocket->PlayerID];
		//
		//if (nFramePrevious >= 30 || nFramePrevious < 0 || p->LastReceiveTime >= sentTime)
		//	return;


		printf("Receive input from Player %i, Room %i, Dir = %i\n", _playerSocket->PlayerID, _playerSocket->PlayerRoomID, dir);
		//p->LastReceiveTime = sentTime;
		//p->SetPositionInPreviousFrame(nFramePrevious);
		//p->SetDirection(dir);

		//for (int i = 0; i < nFramePrevious; i++)
		//{
		//	// check collision
		//	for (auto brick : map->GetBrickList())
		//	{
		//		if (!brick->IsDelete)
		//		{
		//			if (GameCollision::IsCollideInNextFrame(p, brick, 1 / 60.f))
		//			{
		//				p->MakeCollision(brick);
		//			}
		//		}
		//	}

		//	// update
		//	p->Update(1 / 60.f);
		//}
	}
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
