#pragma once

#include "Entity.h"

class Bullet : public Entity
{
	const float speed = 400.f;
	Direction direction; // hướng bay
	D3DXVECTOR2 spawnPosition; // dùng để diễn trên client
	D3DXVECTOR2 destroyPosition = D3DXVECTOR2(0, 0); // dùng để client vẽ

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
		_os.Write((int)(position.x * 10), NBit_Position);
		_os.Write((int)(position.y * 10), NBit_Position);
		_os.Write(direction, NBit_Direction);
		_os.Write((int)(spawnPosition.x * 10), NBit_Position);
		_os.Write((int)(spawnPosition.y * 10), NBit_Position);
		_os.Write((int)(destroyPosition.x * 10), NBit_Position);
		_os.Write((int)(destroyPosition.y * 10), NBit_Position);
	}

	void Spawn(D3DXVECTOR2 _pos, Direction _dir)
	{
		IsDelete = false;

		position = _pos;
		spawnPosition = _pos;

		// set spawn position
		if (_dir == D_Left)
		{
			spawnPosition.x -= 14;
		}
		else if (_dir == D_Right)
		{
			spawnPosition.x += 14;
		}
		else if (_dir == D_Up)
		{
			spawnPosition.y -= 14;
		}
		else if (_dir == D_Down)
		{
			spawnPosition.y += 14;
		}

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

	void ApplyDestroyPosition()
	{
		destroyPosition = position;

		int random1 = rand() % 11 - 5;
		int random2 = rand() % 11 - 5;
		destroyPosition.x += random1 / 10.0f;
		destroyPosition.y += random2 / 10.0f;
	}
};


