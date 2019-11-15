#include "NetWorkManager.h"

#include "SocketUtil.h"
#include "GameLog.h"
using namespace Define;

NetWorkManager::NetWorkManager()
{
	// init server socket
	{
		WSADATA		wsaData;
		WORD		wVersion = MAKEWORD(2, 2);
		int			ret = WSAStartup(wVersion, &wsaData);
		socketServer = SocketUtil::CreateTCPSocket();;
		SocketAddress receivingAddres(INADDR_ANY, 8888);
		if (socketServer->Bind(receivingAddres) == SOCKET_ERROR)
		{
			return;
		};
		if (socketServer->Listen(16) == SOCKET_ERROR)
		{
			return;
		};
		printf("Waiting connection on port 8888...\n");

		clientSockets.push_back(socketServer);
	}

	// tạo 4 phòng
	for (int i = 0; i < 4; i++)
	{
		Room* room = new Room(i);
		roomList.push_back(room);
	}
}

void NetWorkManager::Update(float _dt, double _time)
{
	for (auto room : roomList)
	{
		room->Update(_dt, _time);
	}
}

void NetWorkManager::ReceivePacket()
{
	vector<TCPSocketPtr> readableSockets;
	if (SocketUtil::Select(&clientSockets, &readableSockets, nullptr, nullptr, nullptr, nullptr))
	{
		for (const TCPSocketPtr& socket : readableSockets) {
			// someone connect to this server
			if (socket == socketServer)
			{
				// add new socket
				SocketAddress newClientAddress;
				auto newSocket = socketServer->Accept(newClientAddress);
				newSocket->ChangetoDontWait(1);

				clientSockets.push_back(newSocket);

				printf("Accept new client connection\n");
			}
			// someone send data
			else
			{
				char* buff = static_cast<char*>(std::malloc(1024));
				int receivedByteCount = socket->Receive(buff, 1024);

				// nếu có Dữ Liệu
				if (receivedByteCount > 0)
				{
					InputMemoryBitStream is(buff,
						static_cast<uint32_t> (receivedByteCount));

					int packetType;
					while ((int)is.GetRemainingBitCount() > NBit_PacketType)
					{
						// define packet type first, then you can read data
						packetType = 0;
						is.Read(packetType, NBit_PacketType);

						if (packetType == PT_SyncTime)
						{
							int _sentID = 0;

							is.Read(_sentID, NBit_PacketID);

							// reply sync time
							{
								OutputMemoryBitStream os;

								os.Write(PT_SyncTime, NBit_PacketType);
								os.Write(_sentID, NBit_PacketID);
								os.Write((int)GetTickCount(), NBit_Time);

								socket->Send(os);

			/*					printf("Reply sync time PlayerID = %i, RoomID = %i, Time = %i\n", 
									socket->PlayerID, socket->PlayerRoomID, (int)GetTickCount());*/
							}
						}

						if (packetType == PT_UpdateRooms)
						{
							// reply update rooms
							{
								OutputMemoryBitStream os;

								os.Write(PT_UpdateRooms, NBit_PacketType);
								for (auto room : roomList)
								{
									room->WriteUpdateRooms(os);
								}
								// client in lobby
								if (socket->PlayerID == -1)
								{
									os.Write(false);
								}
								// client in room
								else
								{
									os.Write(true);
								}
								os.Write(socket->PlayerID, NBit_PlayerID); // gửi ID người chơi
								os.Write(socket->PlayerRoomID, NBit_RoomID); // gửi ID phòng

								socket->Send(os);
							}
						}

						if (packetType == PT_JoinRoom)
						{
							// accept join room
							{
								int _roomID = 0;

								is.Read(_roomID, NBit_RoomID);

								if (socket->PlayerRoomID == -1)
								{
									roomList[_roomID]->HandlePlayerJoinRoom(socket);
									printf("Accept player join room %i! \n", _roomID);
								}
							}
						}

						if (packetType == PT_ExitRoom)
						{
							// accept exit room
							{
								int _playerID = socket->PlayerID;
								int _playerRoomID = socket->PlayerRoomID;

								if (_playerRoomID != -1)
								{
									Room* r = roomList[_playerRoomID];
									r->HandlePlayerOutRoom(socket);
									printf("Accept Player ID = %i, RoomID = %i exit room\n", _playerID, _playerRoomID);
								}
							}
						}

						if (packetType == PT_ReadyCancel)
						{
							// accept ready or cancel
							{
								int _playerID = socket->PlayerID;
								int _playerRoomID = socket->PlayerRoomID;

								if (_playerRoomID != -1)
								{
									printf("Accept Player ID = %i, RoomID = %i ready / cancel\n", _playerID, _playerRoomID);

									Room* r = roomList[_playerRoomID];
									r->HandlePlayerReadyOrCancel(socket);
								}
							}
						}

						if (packetType == PT_ExitGame)
						{
							// accept player exit game
							{
								int _playerID = socket->PlayerID;
								int _playerRoomID = socket->PlayerRoomID;

								// handle player out room
								if (_playerID != -1)
								{
									Room* r = roomList[_playerRoomID];
									r->HandlePlayerOutRoom(socket);
								}

								// erase client socket
								for (int i = 0; i < (int)clientSockets.size(); i++)
								{
									if (clientSockets[i] == socket)
									{
										clientSockets.erase(clientSockets.begin() + i);
										break;
									}
								}

								printf("Player ID = %i, RoomID = %i exit game\n", _playerID, _playerRoomID);
								break;
							}
						}

						if (packetType == PT_PlayerInput)
						{
							roomList[socket->PlayerRoomID]->ReceivePlayerInput(socket, is);
						}

						if (packetType == PT_PlayerShoot)
						{
							roomList[socket->PlayerRoomID]->ReceivePlayerShoot(socket, is);
						}

						// clear the byte is reading
						{
							int nClearBit = is.GetRemainingBitCount() % 8; // số bit cần clear
							int k;
							is.Read(k, nClearBit);
						}
					}

					free(buff);
				}
			}
		}
		readableSockets.clear();
	}
}