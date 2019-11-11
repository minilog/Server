#include "Player.h"

Player::Player(int _ID)
{
	ID = _ID;
	Type = ET_Player;
	width = 28;
	height = 28;

	switch (_ID)
	{
	case 0:
		position = D3DXVECTOR2(55.f, 55.f);
		break;
	case 1:
		position = D3DXVECTOR2(745.f, 55.f);
		break;
	case 2:
		position = D3DXVECTOR2(55.f, 745.f);
		break;
	case 3:
		position = D3DXVECTOR2(745.f, 745.f);
		break;
	default:
		break;
	}

	for (int i = 0; i < 30; i++)
	{
		positionList.push_back(position);
	}
}

void Player::Update(float _dt)
{
	position += velocity * _dt;

	// delete in the begin, add in the end
	positionList.erase(positionList.begin());
	positionList.push_back(position);
}

void Player::Write(OutputMemoryBitStream & _os)
{
	int x = int(position.x * 10);
	int y = int(position.y * 10);

	_os.Write(x, NBit_Position);
	_os.Write(y, NBit_Position);
	_os.Write(direction, NBit_Direction);
}	

void Player::SetPositionInPreviousFrame(int _preFrame)
{
	position = positionList[30 - 1 - _preFrame];
}

void Player::SetDirection(Direction _dir)
{
	// set vận tốc
	direction = _dir;
	switch (direction)
	{
	case D_Stand:
		velocity = D3DXVECTOR2(0.f, 0.f);
		break;
	case D_Left:
		if (level == 1)
		{
			velocity = D3DXVECTOR2(-speed1, 0.f);
		}
		else
		{
			velocity = D3DXVECTOR2(-speed2, 0.f);
		}
		break;
	case D_Right:
		if (level == 1)
		{
			velocity = D3DXVECTOR2(speed1, 0.f);
		}
		else
		{
			velocity = D3DXVECTOR2(speed2, 0.f);
		}
		break;
	case D_Up:
		if (level == 1)
		{
			velocity = D3DXVECTOR2(0.f, -speed1);
		}
		else
		{
			velocity = D3DXVECTOR2(0.f, -speed2);
		}
		break;
	case D_Down:
		if (level == 1)
		{
			velocity = D3DXVECTOR2(0.f, speed1);
		}
		else
		{
			velocity = D3DXVECTOR2(0.f, speed1);
		}
		break;
	}
}
