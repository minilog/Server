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




void Receive_thread(NetWorkManager* net)
{
	while (1)
	{
		net->ReceivePacket();
	}

}

int main()
{
	srand(time(NULL));
	NetWorkManager* net_work = new NetWorkManager();
	GameTime *_gameTime = new GameTime();
	_gameTime->init();
	float _detalTime = 0;
	float _lastTime = 0;

	const float tickPerFrame = 1.0f / 60;

	int temp = 0, temp1 = 0;

	MSG msg;
	std::thread task_receive_packet(Receive_thread, net_work);
	task_receive_packet.detach();

	while (1)
	{
		_gameTime->update();
		_detalTime = _gameTime->getTotalTime() - _lastTime;

		if (_detalTime >= tickPerFrame)
		{
			_lastTime += tickPerFrame;
			net_work->Update(1.0f / 60);
		}
		else
		{
			Sleep((DWORD)((tickPerFrame - _detalTime) * 1000.0f));
		}
	}


	return 0;
}


