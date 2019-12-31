// TCPServer.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <conio.h>
#include <string>
#include <ctime>
#include "NetWorkManager.h"
#include "GameTime.h"
#include <thread>
#pragma comment(lib,"WS2_32")

void ReceivePacket(NetWorkManager* network)
{
	while (1)
	{
		network->ReceivePacket();
	}
}

int main()
{
	srand(time(NULL));
	NetWorkManager* network = new NetWorkManager();
	GameTime *gameTime = new GameTime();
	gameTime->init();

	// tạo thêm thread
	MSG msg;
	std::thread Thread(ReceivePacket, network);
	Thread.detach();

	float detalTime = 0;
	float lastTime = 0;

	while (1)
	{
		gameTime->update();
		detalTime = gameTime->getTotalTime() - lastTime;

		if (detalTime >= 1.0f / 60)
		{
			lastTime += 1.0f / 60;

			// thiết lập thời gian bắt đầu cho frame 
			// ...
			network->Update(1.0f / 60);//_detalTime
		}
		else
		{
			Sleep((1.0f / 60 - detalTime) * 1000.0f);
		}
	}

	return 0;
}