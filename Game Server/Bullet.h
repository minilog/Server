#pragma once

#include "Entity.h"

class Bullet : public Entity
{
	const float speed = 400.f;
	Direction direction; // hướng bay
public:
	int PlayerID = -1; // là của người chơi nào

public:
	Bullet(int _ID, int _playerID)
	{
		ID = _ID;
		PlayerID = _playerID;

		Type = ET_Bullet;
		IsDelete = true;
		width = 6;
		height = 6;
;	}
	~Bullet() {}

	void Update(float _dt) override
	{
		if (IsDelete)
			return;

		position += velocity * _dt;
	}

	void Write(OutputMemoryBitStream& _os)
	{
		_os.Write(IsDelete);
		_os.Write((int)position.x, NBit_Position);
		_os.Write((int)position.y, NBit_Position);
		_os.Write(direction, NBit_Direction);
	}

	void Spawn(D3DXVECTOR2 _pos, Direction _dir)
	{
		IsDelete = false;

		position = _pos;

		// thay đổi position bắt đầu dựa theo hướng của tank
		//if (_dir == D_Left)
		//{
		//	position.x -= 14;
		//}
		//else if (_dir == D_Right)
		//{
		//	position.x += 14;
		//}
		//else if (_dir == D_Up)
		//{
		//	position.y -= 14;
		//}
		//else if (_dir == D_Down)
		//{
		//	position.y += 14;
		//}

		SetDirection(_dir);
	}

	// thay đổi vận tốc đựa theo hướng bay
	void SetDirection(Direction _dir)
	{
		direction = _dir;
		switch (direction)
		{
		case D_Left:
			velocity = D3DXVECTOR2(-speed, 0.f);
			break;
		case D_Right:
			velocity = D3DXVECTOR2(speed, 0.f);
			break;
		case D_Up:
			velocity = D3DXVECTOR2(0.f, -speed);
			break;
		case D_Down:
			velocity = D3DXVECTOR2(0.f, speed);
			break;
		default:
			break;
		}
	}
};


