#include "TimeSceneY.h"

#include "SocketUtil.h"
#include "MemoryBitStream.h"
#include "Yato.h"

TimeSceneY::TimeSceneY()
{
	InitSpritesAndLabels();

	CreateSocketAndTryConnectToServer();
}

void TimeSceneY::Draw()
{
	ping_Label.Draw();
	myTime_Label.Draw("Host tick count: " + to_string((unsigned long)GetTickCount()));
	sendTime_Label.Draw();
	receiveTime_Label.Draw();
	timeDistanceWithServer_Label.Draw();
	serverTime_Label.Draw();
}

void TimeSceneY::SendPacket()
{
	OutputMemoryBitStream os;
	os.Write(PT_TickCount, 2); // gửi PacketType với 2 bit
	os.Write(GetTickCount());
	// gửi os đi
}

void TimeSceneY::ReceivePacket()
{
	char* buff = static_cast<char*>(std::malloc(1024));
	size_t receivedByteCount = TCPSocket->Receive(buff, 1024);

	if (receivedByteCount > 0)
	{
		InputMemoryBitStream is(buff,
			static_cast<uint32_t> (receivedByteCount));

		int typeofPacket = 0;
		is.Read(typeofPacket, Define::bitofTypePacket); // read loại Packet
	}
	free(buff);
}

void TimeSceneY::InitSpritesAndLabels()
{
	ping_Label = Label("Ping: ms", 20, 10, D3DXVECTOR2(1170, 15));
	myTime_Label = Label("My time: ", 20, 10, D3DXVECTOR2(15, 15));
	sendTime_Label = Label("Send time: ", 20, 10, D3DXVECTOR2(15, 45));
	receiveTime_Label = Label("Receive time: ", 20, 10, D3DXVECTOR2(15, 75));
	timeDistanceWithServer_Label = Label("Time distance with Server: ", 20, 10, D3DXVECTOR2(15, 105));
	serverTime_Label = Label("Server time: ", 20, 10, D3DXVECTOR2(15, 135));
}

void TimeSceneY::CreateSocketAndTryConnectToServer()
{
	string ip = "127.0.0.1";
	if (__argv[1] != NULL)
	{
		ip = string(__argv[1]);
	}

	SocketAddress socketAddress(inet_addr(ip.c_str()), 8888);

	TCPSocket = SocketUtil::CreateTCPSocket();
	if (TCPSocket->Connect(socketAddress) == SOCKET_ERROR)
	{
		OutputDebugStringA("Connect to Server failed!");
	}
	else
	{
		OutputDebugStringA("Connect to Server successfull!");
	}

	TCPSocket->ChangetoDontWait(1);

	// GameGlobal save the socket successfully connect to Server
	GameGlobal::socket = TCPSocket;
}
