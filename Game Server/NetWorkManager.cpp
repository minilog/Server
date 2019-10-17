#include "NetWorkManager.h"

#include "SocketUtil.h"
#include "GameLog.h"

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
	int id_room = sock->ID_Room;

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
	// xóa World đó ra khỏi danh sách World, ngoại trừ Wolrd 1 và World 2
	if (mListWorld_room[i]->name._Equal("Room 1") || mListWorld_room[i]->name._Equal("Room 2"))
	{

	}
	else
	{
		mListWorld_room.erase(mListWorld_room.begin() + i);
		i--;
	}

	// xóa luôn các Socket của các Client đang ở trong Wolrd đó
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
	// tạo 1 Wolrd mới, đưa vào danh sách World
	World* world = new World();
	mListWorld_room.push_back(world);

	// gán ID cho World mới được tạo, World 1 (ID = 1), World 2(ID = 2), World 3 (ID = 3),...
	int i = 2;
	while (1)
	{
		i++;
		bool flag = false;
		for (auto ele : mListWorld_room)
			if (ele->ID == i) 
				flag = true;
		if (!flag) 
			break;
	}
	world->ID = i;
	world->name = "Room " + std::to_string(i);

	// gán ID World đó cho Socket tạo ra nó
	soc->ID_Room = i;

	// thêm Socket đó vào World
	world->AddMember(soc);
}

NetWorkManager::NetWorkManager()
{
	CreateSocketServer();

	//tạo wolrd 1
	World* world = new World(); 
	world->ID = 1; 
	world->name = "Room 1"; 
	mListWorld_room.push_back(world);

	// tạo world 2
	World* world1 = new World(); world1->ID = 2;  
	world1->name = "Room 2"; 
	mListWorld_room.push_back(world1);
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

			// gửi dữ liệu của World cho các Client mỗi 3 lần cập nhật
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
			// nếu là Server Socket thì thêm Client Socket vừa Connect vào
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

					// packet là ExitGame của Người Chơi này
					if (packetType == Define::ExitGame)
					{
						Handle_Exit(socket);
						continue;
					}

					// packetType là yêu cầu SyncTime
					if (packetType == Define::SyncTime)
					{
						// NReceived để chắc chắn Client nhận đúng Packet cần nhận
						int NReceived = 0;
						is.Read(NReceived, 32);
						Send_SyncTimePacket(socket, NReceived);
					}

				//	// nếu Client Socket đang chưa vào Phòng Chơi thì ... 
				//	if (socket->ID_Room == 0)
				//	{
				//		// packetType là UpdateCountPlayer thì gửi lại Client Socket đó
				//		// packetType = UpdateCountPlayer
				//		// số World hiện có
				//		// và số Người chơi hiện có, tên, Game đã bắt đầu hay chưa của World đó
				//		if (packetType == Define::UpdateCountPlayer)
				//		{
				//			OutputMemoryBitStream os;
				//			os.Write(Define::UpdateCountPlayer, Define::bitofTypePacket);
				//			os.Write(mListWorld_room.size(), Define::bitofTypePacket);
				//			for (auto ele : mListWorld_room)
				//			{
				//				os.Write(ele->listClient.size(), Define::bitofTypePacket);
				//				os.Write(ele->name);
				//				os.Write(ele->isStart);
				//			}
				//			socket->Send(os.GetBufferPtr(), os.GetByteLength());
				//			printf("Gui cho 1 Client - Cap nhat so luong nguoi choi");
				//		}
				//		// packetType là ChooseRoom, thì đọc Dữ Liệu:
				//		// room nào được chọn => gán Client Socket này có ID Room của Room được chọn
				//		else if (packetType == Define::ChooseRoom)
				//		{
				//			int room_selected = 0;
				//			is.Read(room_selected, Define::bitofTypePacket);
				//			socket->ID_Room = room_selected;
				//			
				//			// Room được chọn thêm Client Socket này vào
				//			mListWorld_room[room_selected - 1]->AddMember(socket);
				//		}
				//		// packetType là CreateRoom, thì chạy hàm Thêm Phòng với tham số là Client Socket này
				//		else if (packetType == Define::CreateRoom)
				//		{
				//			CreateRoomAndAdd(socket);
				//		}
				//	}
				//	// nếu Người chơi đang ở trong 1 Room
				//	else
				//	{
				//		// xác đinh Room của Người Chơi đang ở trong...
				//		for (auto room : mListWorld_room)
				//		{
				//			if (room->ID == socket->ID_Room)
				//			{
				//				// nếu Room đã Bắt Đầu...
				//				if (room->isStart)
				//				{
				//					// packetType là UpdateCountPlayer thì Room gọi hàm Tạo Nhân Vật và Gửi đi
				//					if (packetType == Define::UpdateCountPlayer)
				//					{
				//						room->CreatePlayerAndSend();
				//					}

				//					// packetType là OutRoom => Room tăng CountPlayerExit thêm 1
				//					// thoát, duyệt Client Socket tiếp theo
				//					else if (packetType == Define::OutRoom)
				//					{
				//						room->Count_player_exit++;
				//						continue;
				//					}

				//					// tạo Packet có: 
				//					// InputStream is, 
				//					// ID Room của Người Chơi này 
				//					// packetType
				//					Packet p(is, socket->ID_Room, packetType);

				//					// đưa vào danh sách Packet
				//					queue_packet.push_back(p);
				//				}
				//				// nếu Room chưa Bắt Đầu...
				//				else
				//				{
				//					// packetType là UpdateCountPlayer => Room chạy hàm Cập Nhật Số Lượng Người Chơi
				//					if (packetType == Define::UpdateCountPlayer)
				//					{
				//						room->UpdatePlayerCount();
				//					}
				//					// packetType là RequestName => đọc Tên và đặt Tên đó cho ClientSocket.name 
				//					else if (packetType == Define::RequestName)
				//					{
				//						string name = "";
				//						is.Read(name);
				//						socket->name = name;
				//					}
				//				}
				//				//break;
				//			}

				//		}


				//	}

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

void NetWorkManager::Send_SyncTimePacket(const TCPSocketPtr & socket, int _NReceived)
{
	OutputMemoryBitStream os;
	os.Write(Define::SyncTime, Define::bitofTypePacket);
	os.Write(_NReceived, 32);
	os.Write((int)GetTickCount(), 32);
	socket->Send(os.GetBufferPtr(), os.GetByteLength());
	printf("Gui time Server cho 1 Client: %i\n", (int)GetTickCount());
}

