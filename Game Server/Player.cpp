#include "Player.h"

Player::Player(int _ID)
{
	ID = _ID;

	Type = ET_Player;
	width = 28;
	height = 28;
}

void Player::Update(float _dt)
{
	position += velocity * _dt;
}
