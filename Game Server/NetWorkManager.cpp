#include "NetWorkManager.h"

#include "SocketUtil.h"
#include "GameLog.h"

void NetWorkManager::CreatePlayerAndSend()
{

}

void NetWorkManager::Handle_Packet()
{
	while (!queue_packet.empty())
	{
		Packet p = queue_packet.at(queue_packet.size() - 1);
		mListWorld_room[p.room_id - 1]->HandleObject(p);
		queue_packet.pop_back();
	}
}

void NetWorkManager::Handle_Exit(TCPSocketPtr sock)
{
	int id = sock->ID;
	int id_room = sock->ID_Room;
	for (int i = 0; i < readBlockSockets.size(); i++)
	{
		if (readBlockSockets[i] == sock)
		{
			readBlockSockets.erase(readBlockSockets.begin() + i);
			break;
		}
	}
	if (id_room > 0)
		for (auto ele : mListWorld_room)
		{
			if (ele->ID == id_room)
				ele->Handle_Exit(id);
		}

	printf("\nSOMEONE LOGOUT");

}

void NetWorkManager::DeleteWorld(int i)
{
	if (mListWorld_room[i]->name._Equal("Room 1") || mListWorld_room[i]->name._Equal("Room 2"))
	{

	}
	else
	{
		mListWorld_room.erase(mListWorld_room.begin() + i);
		i--;
	}

	for (int j = 0; j < readBlockSockets.size(); j++)
	{
		if (readBlockSockets[j]->ID_Room == mListWorld_room[i]->ID)
		{
			readBlockSockets.erase(readBlockSockets.begin() + j);
			j--;
		}

	}
	mListWorld_room[i]->isDelete = false;


}

void NetWorkManager::CreateRoomAndAdd(TCPSocketPtr soc)
{
	World* world = new World();
	mListWorld_room.push_back(world);
	int i = 2;
	while (1)
	{
		i++;
		bool flag = false;
		for (auto ele : mListWorld_room)
			if (ele->ID == i) flag = true;
		if (!flag) break;

	}
	world->ID = i;
	world->name = "Room " + std::to_string(i);
	soc->ID_Room = i;
	world->AddMember(soc);

}

NetWorkManager::NetWorkManager()
{
	WSADATA		wsaData;
	WORD		wVersion = MAKEWORD(2, 2);
	int			ret = WSAStartup(wVersion, &wsaData);
	socket_sever = SocketUtil::CreateTCPSocket();;
	SocketAddress receivingAddres(INADDR_ANY, 8888);
	if (socket_sever->Bind(receivingAddres) == SOCKET_ERROR)
	{
		return;
	};
	if (socket_sever->Listen(16) == SOCKET_ERROR)
	{
		return;
	};
	printf("Dang doi ket noi o cong 8888...");

	readBlockSockets.push_back(socket_sever);

	World* world = new World(); world->ID = 1; world->name = "Room 1"; mListWorld_room.push_back(world);
	World* world1 = new World(); world1->ID = 2;  world1->name = "Room 2"; mListWorld_room.push_back(world1);



}


NetWorkManager::~NetWorkManager()
{
}
int count_to_send = 0;
void NetWorkManager::Update(float dt)
{
	count_to_send++;
	for (int i = 0; i < mListWorld_room.size(); i++)
	{
		if (mListWorld_room[i]->isDelete)
		{
			DeleteWorld(i);
		}
		else
		{
			mListWorld_room[i]->CheckCollision(dt);
			mListWorld_room[i]->Update(dt);
			if (count_to_send == 3)
			{
				mListWorld_room[i]->SendWorld();
				count_to_send = 0;
			}
		}

	}





}

void NetWorkManager::ProcessNewClient()
{
	SocketAddress newClientAddress;
	auto newSocket = socket_sever->Accept(newClientAddress);
	newSocket->ChangetoDontWait(1);
	readBlockSockets.push_back(newSocket);// Add socket to list 

}

void NetWorkManager::UpdatePlayerCount()
{

}

void NetWorkManager::ReceivePacket()
{



	if (SocketUtil::Select(&readBlockSockets, &readableSockets, nullptr, nullptr, nullptr, nullptr))
	{
		for (const TCPSocketPtr& socket : readableSockets) {

			if (socket == socket_sever)
			{

				ProcessNewClient();
				printf("\nCo ket noi moi");

			}
			else
			{

				char* buff = static_cast<char*>(std::malloc(1024));
				size_t receivedByteCount = socket->Receive(buff, 1024);
				if (receivedByteCount > 0)
				{
					InputMemoryBitStream is(buff,
						static_cast<uint32_t> (receivedByteCount));
					int type_of_packet = 0;
					is.Read(type_of_packet, Define::bitofTypePacket);
					if (type_of_packet == Define::ExitGame)
					{
						Handle_Exit(socket);
						continue;
					}
					if (socket->ID_Room == 0)
					{

						if (type_of_packet == Define::UpdateCountPlayer)
						{
							OutputMemoryBitStream os;
							os.Write(Define::UpdateCountPlayer, Define::bitofTypePacket);
							os.Write(mListWorld_room.size(), Define::bitofTypePacket);
							for (auto ele : mListWorld_room)
							{
								os.Write(ele->listClient.size(), Define::bitofTypePacket);
								os.Write(ele->name);
								os.Write(ele->isStart);
							}
							socket->Send(os.GetBufferPtr(), os.GetByteLength());
						}
						else if (type_of_packet == Define::ChooseRoom)
						{
							int room_selected = 0;
							is.Read(room_selected, Define::bitofTypePacket);
							socket->ID_Room = room_selected;
							mListWorld_room[room_selected - 1]->AddMember(socket);
						}
						else if (type_of_packet == Define::CreateRoom)
						{
							CreateRoomAndAdd(socket);
						}
					}
					else
					{
						for (auto room : mListWorld_room)
						{
							if (room->ID == socket->ID_Room)
							{
								if (room->isStart)
								{


									if (type_of_packet == Define::UpdateCountPlayer)
									{
										room->CreatePlayerAndSend();
									}
									else if (type_of_packet == Define::OutRoom)
									{
										room->Count_player_exit++;
										continue;;
									}

									Packet p(is, socket->ID_Room, type_of_packet);
									queue_packet.push_back(p);

								}
								else
								{

									if (type_of_packet == Define::UpdateCountPlayer)
									{
										room->UpdatePlayerCount();
									}
									else if (type_of_packet == Define::RequestName)
									{
										string name = "";
										is.Read(name);
										socket->name = name;
									}
								}
								//break;
							}

						}


					}

				}

			}
		}
		readableSockets.clear();
	}

}



