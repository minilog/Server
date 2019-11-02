#include "Room.h"

void Room::Write(OutputMemoryBitStream & _os)
{
	_os.Write(Player0);
	_os.Write(Player0_Ready);

	_os.Write(Player1);
	_os.Write(Player1_Ready);

	_os.Write(Player2);
	_os.Write(Player2_Ready);

	_os.Write(Player3);
	_os.Write(Player3_Ready);

	_os.Write(isPlaying);
	_os.Write(startingTime, NBit_Time);
}

void Room::HandlePlayerOutRoom(TCPSocketPtr _playerSocket)
{
	int pID = _playerSocket->PlayerID;
	if (pID == 0)
	{
		Player0 = false;
		Player0_Ready = false;
	}
	else if (pID == 1)
	{
		Player1 = false;
		Player1_Ready = false;
	}
	else if (pID == 2)
	{
		Player2 = false;
		Player2_Ready = false;
	}
	else if (pID == 3)
	{
		Player3 = false;
		Player3_Ready = false;
	}

	_playerSocket->PlayerID = -1;
	_playerSocket->PlayerRoomID = -1;

	// pop back that client
	for (int i = 0; i < clientList.size(); i++)
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

	if (!Player0)
	{
		_playerSocket->PlayerID = 0;
		Player0 = true;
		Player0_Ready = false;
	}
	else if (!Player1)
	{
		_playerSocket->PlayerID = 1;
		Player1 = true;
		Player1_Ready = false;
	}
	else if (!Player2)
	{
		_playerSocket->PlayerID = 2;
		Player2 = true;
		Player2_Ready = false;
	}
	else if (!Player3)
	{
		_playerSocket->PlayerID = 3;
		Player3 = true;
		Player3_Ready = false;
	}

	_playerSocket->PlayerRoomID = ID;

	clientList.push_back(_playerSocket);
}

void Room::HandlePlayerReadyOrCancel(TCPSocketPtr _playerSocket)
{
	if (isPlaying)
		return;

	int pID = _playerSocket->PlayerID;
	if (pID == 0)
	{
		Player0_Ready = !Player0_Ready;
	}
	else if (pID == 1)
	{
		Player1_Ready = !Player1_Ready;
	}
	else if (pID == 2)
	{
		Player2_Ready = !Player2_Ready;
	}
	else if (pID == 3)
	{
		Player3_Ready = !Player3_Ready;
	}

	int nPlayers = Player0 + Player1 + Player2 + Player3;
	int nReady = Player0_Ready + Player1_Ready + Player2_Ready + Player3_Ready;

	if (nPlayers >= 2 && nPlayers == nReady)
	{
		isPlaying = true;
		startingTime = (int)GetTickCount();

		printf("Game in room = %i start\n", ID);
	}
}
