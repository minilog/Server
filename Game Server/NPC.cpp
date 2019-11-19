﻿#include "NPC.h"
#include <time.h>

NPC::NPC(int _ID)
{
	ID = _ID;

	Type = ET_NPC;
	IsDelete = true;
	width = 28;
	height = 28;
}

void NPC::Update(float _dt)
{
	if (IsDelete)
	{
		count_Spawn -= _dt;
		if (count_Spawn <= 0)
		{
			count_Spawn = 2.0f;

			// spawn NPC tại đây
			IsDelete = false;

			// random position
			int random = rand() % 4; // random = 0 - 3
			float a = 250.0f, b = 500.0f;
			switch (random)
			{
			case 0:
				position = D3DXVECTOR2(a, a);
				break;
			case 1:
				position = D3DXVECTOR2(b, a);
				break;
			case 2:
				position = D3DXVECTOR2(a, b);
				break;
			case 3:
				position = D3DXVECTOR2(b, b);
				break;
			}

			// random direction
			int random2 = rand() % 4;
			switch (random2)
			{
			case 0:
				SetDirection(D_Left);
				break;
			case 1:
				SetDirection(D_Right);
				break;
			case 2:
				SetDirection(D_Up);
				break;
			case 3:
				SetDirection(D_Down);
				break;
			}
		}
		return;
	}

	count_ChangeDirection -= _dt;
	if (count_ChangeDirection <= 0 || direction == D_Stand)
	{
		count_ChangeDirection = 2.0f;

		// random direction
		int random = rand() % 4;
		switch (random)
		{
		case 0:
			SetDirection(D_Left);
			break;
		case 1:
			SetDirection(D_Right);
			break;
		case 2:
			SetDirection(D_Up);
			break;
		case 3:
			SetDirection(D_Down);
			break;
		}
		return;
	}

	position += velocity * _dt;
}

void NPC::ChangeHP(int amount)
{
	HP += amount;
	if (HP <= 0)
	{
		HP = 2;
		IsDelete = true;
	}
}

void NPC::Write(OutputMemoryBitStream & _os)
{
	int x = (int)position.x;
	int y = (int)position.y;

	_os.Write(x * 10, NBit_Position);
	_os.Write(y * 10, NBit_Position);
	_os.Write(direction, NBit_Direction);
	_os.Write(IsDelete);
}

void NPC::SetDirection(Direction _dir)
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
