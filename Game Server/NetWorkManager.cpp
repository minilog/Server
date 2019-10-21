#include "NetWorkManager.h"

#include "SocketUtil.h"
#include "GameLog.h"

NetWorkManager::NetWorkManager()
{
	CreateSocketServer();
}

void NetWorkManager::Handle_Packet()
{
	// nếu có Packet trong Queue thì xử lý dữ liệu của nó
	while (!queue_packet.empty())
	{
		Packet p = queue_packet.at(queue_packet.size() - 1);
		// xử lý packet
		//mListWorld_room[p.room_id - 1]->HandleObject(p);
		queue_packet.pop_back();
	}
}

void NetWorkManager::Handle_Exit(TCPSocketPtr sock)
{
	// xác định socket của Client có ID gì và đang ở Room nào
	int id = sock->ID;
	int id_room = sock->RoomID;

	// xóa socket của Client đó ra khỏi danh sách readSocket
	for (int i = 0; i < readBlockSockets.size(); i++)
	{
		if (readBlockSockets[i] == sock)
		{
			readBlockSockets.erase(readBlockSockets.begin() + i);
			break;
		}
	}
	
	// nếu Client đó đang ở trong 1 Phòng chơi, Phòng chơi đó xử lý Thoát Game của người chơi đó
	//if (id_room >= 0)
	//{
	//	for (auto room : mListWorld_room)
	//	{
	//		if (room->ID == id_room)
	//			room->Handle_Exit(id);
	//	}
	//}

	printf("\nSOMEONE LOGOUT");

}

void NetWorkManager::DeleteWorld(int i)
{
	mListWorld_room.erase(mListWorld_room.begin() + i);

	// xóa các Socket Người Chơi đang ở trong Phòng Chơi đó
	for (int j = 0; j < readBlockSockets.size(); j++)
	{
		if (readBlockSockets[j]->RoomID == mListWorld_room[i]->ID)
		{
			readBlockSockets.erase(readBlockSockets.begin() + j);
			j--;
		}

	}
}

void NetWorkManager::CreateRoomAndAdd(TCPSocketPtr soc)
{
	// tạo 1 Phòng Chơi mới, đưa vào danh sách Phòng Chơi
	World* room = new World();
	mListWorld_room.push_back(room);

	// gán ID cho Phòng Chơi mới được tạo
	int i = 0;
	while (1)
	{
		bool flag = false;

		for (auto room : mListWorld_room)
			if (room->ID == i) 
				flag = true;

		if (!flag) 
			break;

		i++;
	}
	room->ID = i;
	room->name = "Room: " + std::to_string(i);

	// người chơi có ID phòng
	soc->RoomID = i;

	// thêm người chơi vào phòng chơi
	room->AddMember(soc);
}

int count_to_send = 0;
void NetWorkManager::Update(float dt)
{
	count_to_send++;
	for (int i = 0; i < mListWorld_room.size(); i++)
	{
		// World nào trong danh sách có biến isDelete = true, xóa World đó
		if (mListWorld_room[i]->isDelete)
		{
			DeleteWorld(i);
		}
		// Va Chạm game & Cập Nhật game
		else
		{
			//mListWorld_room[i]->CheckCollision(dt);
			//mListWorld_room[i]->Update(dt);

			//// gửi dữ liệu của World cho các Client mỗi 3 lần cập nhật
			//if (count_to_send == 3)
			//{
			//	mListWorld_room[i]->SendWorld();
			//	count_to_send = 0;
			//}
		}

	}
}

void NetWorkManager::ProcessNewClient()
{
	// khởi tạo Client Socket mới, đưa vào danh sách
	SocketAddress newClientAddress;
	auto newSocket = socket_sever->Accept(newClientAddress);
	newSocket->ChangetoDontWait(1);
	readBlockSockets.push_back(newSocket);
}

void NetWorkManager::ReceivePacket()
{
	vector<TCPSocketPtr> readableSockets;

	// kiểm tra Socket nào sẵn sàng đọc Dữ Liệu
	if (SocketUtil::Select(&readBlockSockets, &readableSockets, nullptr, nullptr, nullptr, nullptr))
	{
		// duyệt từng Socket để nhận Dữ Liệu 
		for (const TCPSocketPtr& socket : readableSockets) {
			// nếu là Server Socket => thêm Client Socket vừa Connect vào
			if (socket == socket_sever)
			{
				ProcessNewClient();
				printf("\nCo ket noi moi");
			}
			else
			{
				// đưa Dữ Liệu đọc được vào Buffer
				char* buff = static_cast<char*>(std::malloc(1024));
				int receivedByteCount = socket->Receive(buff, 1024);

				// nếu có Dữ Liệu > 0
				if (receivedByteCount > 0)
				{
					InputMemoryBitStream is(buff,
						static_cast<uint32_t> (receivedByteCount));

					// đọc packetType, xác định loại Dữ Liệu sẽ được đọc tiếp theo
					int packetType = 0;
					is.Read(packetType, Define::bitofTypePacket);

					// packet là Thoát Game...
					if (packetType == Define::ExitGame)
					{
						// xử lý Thoát Game của người chơi này
						Handle_Exit(socket);
						continue;
					}

					// packetType là yêu cầu Đồng Bộ Thời Gian...
					if (packetType == Define::SyncTime)
					{
						// NReceived để chắc chắn Client nhận đúng Packet cần nhận
						int NReceived = 0;
						is.Read(NReceived, 32);
						// gửi lại Người Chơi yêu cầu Đồng Bộ Thời Gian thời gian của Server này
						Send_SyncTime(socket, NReceived);
					}

					// packetType là cập nhật số lượng người chơi...
					if (packetType == Define::UpdateCountPlayer)
					{
						// người chơi đang ở sảnh chọn phòng chơi...
						if (socket->RoomID == -1)
						{
							// gửi Dữ Liệu về các phòng và số lượng người chơi
							Send_UpdateCountPlayer_OnLobby(socket);
							printf("Cap nhat so luong nguoi choi o Sanh\n");
						}
						// người chơi đang ở trong phòng...
						else
						{
							// cập nhật người chơi trong phòng chơi đó
							// bằng cách thêm vào danh sách packet => phòng chơi sẽ xử lý
							Handle_UpdateCountPlayer_OnRoom(is, socket);
						}
					}

					// packetType là yêu cầu muốn vào phòng chơi...
					if (packetType == Define::ChooseRoom)
					{
						// nhận thông tin phòng được chọn, thêm người chơi vào phòng
						Receive_ChooseRoom(is, socket);
					}

					// packetType là yêu cầu tạo phòng...
					if (packetType == Define::CreateRoom)
					{
						// tạo phòng và thêm người chơi vào phòng đó
						CreateRoomAndAdd(socket);
					}

					// packetType là yêu cầu rời khỏi phòng
					if (packetType == Define::OutRoom)
					{
						// nếu phòng của người chơi đã bắt đầu, thì tăng số lượng người chơi đã thoát của phòng đó
						Handle_PlayerOutRoon(socket);
					}

					if (packetType == Define::RequestName)
					{
						// nếu phòng của người chơi chưa bắt đầu, đọc tên của người chơi đó
						Handle_RequestName(is, socket);
					}
				}
			}
		}
		readableSockets.clear();
	}

}

void NetWorkManager::CreateSocketServer()
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
}

void NetWorkManager::Send_SyncTime(const TCPSocketPtr & socket, int _NReceived)
{
	OutputMemoryBitStream os;
	os.Write(Define::SyncTime, Define::bitofTypePacket);
	os.Write(_NReceived, 32);
	os.Write((int)GetTickCount(), 32);
	socket->Send(os);
	//socket->Send(os.GetBufferPtr(), os.GetByteLength());
	printf("Gui time Server cho 1 Client: %i\n", (int)GetTickCount());
}

void NetWorkManager::Send_UpdateCountPlayer_OnLobby(const TCPSocketPtr& _socket)
{
	OutputMemoryBitStream os;
	os.Write(Define::UpdateCountPlayer, Define::bitofTypePacket);

	// gửi số lượng phòng chơi hiện có
	os.Write((int)mListWorld_room.size(), 4);

	// gửi số lượng người chơi, tên của mỗi phòng chơi, phòng chơi đã bắt đầu hay chưa
	for (auto room : mListWorld_room)
	{
		os.Write((int)room->listClient.size(), 4);
		os.Write(room->name);
		os.Write(room->isStart);
	}
	_socket->Send(os);
}

void NetWorkManager::Receive_ChooseRoom(InputMemoryBitStream & _is, const TCPSocketPtr & _socket)
{
	// người chơi đang ở sảnh chọn phòng
	if (_socket->RoomID == -1)
	{
		// đọc ID phòng được chọn
		int roomSelected = 0;
		_is.Read(roomSelected, 4);

		// gán người chơi có ID phòng là ID phòng đó 
		_socket->RoomID = roomSelected;

		// phòng chơi được chọn thêm người chơi này vào
		mListWorld_room[roomSelected]->AddMember(_socket);
	}
}

void NetWorkManager::Handle_UpdateCountPlayer_OnRoom(InputMemoryBitStream & _is, const TCPSocketPtr & _socket)
{
	// tìm phòng mà người chơi đang ở trong đó
	for (auto room : mListWorld_room)
	{
		if (room->ID == _socket->RoomID)
		{
			// phòng chưa bắt đầu chơi...
			if (!room->isStart)
			{
				// phòng cập nhật số lượng người chơi
				room->UpdatePlayerCount();
			}
			// phòng đang chơi...
			else
			{
				// tạo nhân vật và gửi thông tin đến người chơi này
				room->CreatePlayerAndSend();

				// tạo Packet có: Input Stream, ID phòng của người chơi này, packetType là cập nhật số lượng người chơi
				// và đưa vào danh sách packet
				Packet p(_is, _socket->RoomID, Define::UpdateCountPlayer);
				queue_packet.push_back(p);
			}

			break;
		}
	}
}

void NetWorkManager::Handle_PlayerOutRoon(const TCPSocketPtr & _socket)
{
	// tìm phòng mà người chơi đang ở trong đó
	for (auto room : mListWorld_room)
	{
		if (room->ID == _socket->RoomID)
		{
			if (room->isStart)
			{
				room->Count_player_exit++;
			}

			break;
		}
	}
}

void NetWorkManager::Handle_RequestName(InputMemoryBitStream& _is, const TCPSocketPtr & _socket)
{						
	// tìm phòng mà người chơi đang ở trong đó
	for (auto room : mListWorld_room)
	{
		if (room->ID == _socket->RoomID)
		{
			if (!room->isStart)
			{
				string name = "";
				_is.Read(name);
				_socket->name = name;
			}

			break;
		}
	}
}

