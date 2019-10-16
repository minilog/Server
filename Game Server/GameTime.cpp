#include "GameTime.h"

GameTime::GameTime()
{
}

GameTime::~GameTime()
{
}

void GameTime::init()
{
	_totalTime = (float)clock() / CLOCKS_PER_SEC;
	_elapsedTime = 0;
}

void GameTime::update()
{
	// thời gian hiện tại
	float curTime = (float)clock() / CLOCKS_PER_SEC;

	// thời gian trôi qua từ lần update trước
	_elapsedTime = curTime - _totalTime;

	// tổng thời gian = thời gian hiện tại
	_totalTime = curTime;
}

float GameTime::getElapsedTime() const
{
	return _elapsedTime;
}

float GameTime::getTotalTime() const
{
	return _totalTime;
}

clock_t GameTime::getTicks() const
{
	return clock();
}
