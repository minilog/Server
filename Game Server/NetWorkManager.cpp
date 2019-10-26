#include "NetWorkManager.h"

#include "SocketUtil.h"
#include "GameLog.h"

NetWorkManager::NetWorkManager()
{
	CreateSocketServer();

	Room* room0 = new Room(0);
	roomList.push_back(room0);

	Room* room1 = new Room(1);
	roomList.push_back(room1);

	Room* room2 = new Room(2);
	roomList.push_back(room2);

	Room* room3 = new Room(3);
	roomList.push_back(room3);
}

void NetWorkManager::HandlePacket()
{
	// nếu có Packet trong Queue thì xử lý dữ liệu của nó
	while (!packetQueues.empty())
	{
		Packet p = packetQueues.at(packetQueues.size() - 1);

		// xử lý packet
		roomList[p.RoomID]->HandleDataFromClients(p);

		// đưa packet này ra khởi danh sách
		packetQueues.pop_back();
	}


}

void NetWorkManager::Update(float dt)
{
}


void NetWorkManager::ReceivePacket()
{
	vector<TCPSocketPtr> readableSockets;

	// kiểm tra Socket nào sẵn sàng đọc Dữ Liệu
	if (SocketUtil::Select(&clientSockets, &readableSockets, nullptr, nullptr, nullptr, nullptr))
	{
		// duyệt từng Socket để nhận Dữ Liệu 
		for (const TCPSocketPtr& socket : readableSockets) {
			// nếu là Server Socket => thêm Client Socket vừa Connect vào
			if (socket == socketServer)
			{
				ProcessNewClient();
				printf("\nCo ket noi moi");
			}

			// nếu là 1 trong những socket của người chơi
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

					// khi mà stream vẫn còn dữ liệu để đọc... (> 4 bit)
					while (is.GetRemainingBitCount() > Define::bitofTypePacket)
					{
						// đọc packetType, xác định loại Dữ Liệu sẽ được đọc tiếp theo
						int packetType = 0;
						is.Read(packetType, Define::bitofTypePacket);

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
							// gửi Dữ Liệu về các phòng và số lượng người chơi
							Send_UpdatePlayers(socket);
						}

						// packetType là yêu cầu muốn vào phòng chơi...
						if (packetType == Define::JoinRoon)
						{
							Receive_JoinRoom(is, socket);
						}

						// packet là Thoát Game...
						if (packetType == Define::ExitGame)
						{
							// xử lý Thoát Game của người chơi này
							HandlePlayerExit(socket);
							continue;
						}

						// packetType là yêu cầu rời khỏi phòng
						if (packetType == Define::OutRoom)
						{
							// nếu phòng của người chơi đã bắt đầu, thì tăng số lượng người chơi đã thoát của phòng đó
							Handle_PlayerOutRoon(socket);
						}
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
	printf("Dang doi ket noi o cong 8888...\n");

	clientSockets.push_back(socketServer);
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

void NetWorkManager::Send_UpdatePlayers(const TCPSocketPtr& _socket)
{
	OutputMemoryBitStream os;
	// loại packet
	os.Write(Define::UpdateCountPlayer, Define::bitofTypePacket);

	// cho người chơi biết ID của mình và ID phòng hiện tại
	if (_socket->NetworkID == -1)
	{
		os.Write(false); // nghĩa là số âm, người chơi không ở trong phòng
	}
	else
	{
		os.Write(true);
		os.Write(_socket->NetworkID, 2); // Id dao động từ 0 - 3 => 2 bit
		os.Write(_socket->NetworkRoomID, 2); // ID room dao động từ 0 - 3 => 2 bit
	}

	// gửi số lượng người chơi, tên của mỗi phòng chơi, phòng chơi đã bắt đầu hay chưa
	for (auto room : roomList)
	{
		os.Write(room->GetNPlayer(), 2); // số người chơi: 0 - 3 => 2 bit
	}
	_socket->Send(os);
}

void NetWorkManager::Receive_JoinRoom(InputMemoryBitStream & _is, const TCPSocketPtr & _socket)
{
	// người chơi đang ở sảnh
	if (_socket->NetworkRoomID == -1)
	{
		// đọc ID phòng được chọn
		int _networkRoomID = 0;
		_is.Read(_networkRoomID, Define::NBit_RoomID);

		// phòng thêm người chơi này vào
		roomList[_networkRoomID]->AddPlayer(_socket);
	}
}

void NetWorkManager::Handle_UpdateCountPlayer_OnRoom(InputMemoryBitStream & _is, const TCPSocketPtr & _socket)
{

}

void NetWorkManager::Handle_PlayerOutRoon(const TCPSocketPtr & _socket)
{

}

void NetWorkManager::Handle_RequestName(InputMemoryBitStream& _is, const TCPSocketPtr & _socket)
{						

}

void NetWorkManager::HandlePlayerExit(TCPSocketPtr _playerSocket)
{
	// xác định socket của Client có ID gì và đang ở Room nào
	int _playerNetworkID = _playerSocket->NetworkID;
	int _playerRoomID = _playerSocket->NetworkRoomID;

	// xóa socket của Client đó ra khỏi danh sách readSocket
	for (int i = 0; i < clientSockets.size(); i++)
	{
		if (clientSockets[i] == _playerSocket)
		{
			clientSockets.erase(clientSockets.begin() + i);
			break;
		}
	}

	// nếu Client đó đang ở trong 1 Phòng chơi, Phòng chơi đó xử lý Thoát Game của người chơi đó
	if (_playerRoomID >= 0)
	{
		for (auto room : roomList)
		{
			if (room->GetNetworkID() == _playerRoomID)
				room->HandlePlayerExit(_playerNetworkID);
		}
	}

	printf("\nSOMEONE LOGOUT");
}


void NetWorkManager::ProcessNewClient()
{
	// khởi tạo Client Socket mới, đưa vào danh sách
	SocketAddress newClientAddress;
	auto newSocket = socketServer->Accept(newClientAddress);
	newSocket->ChangetoDontWait(1);
	clientSockets.push_back(newSocket);
}