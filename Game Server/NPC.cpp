#include "NPC.h"
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
	if (IsDelete == true)
	{
		count_Spawn -= _dt;
		if (count_Spawn <= 0)
		{
			// spawn NPC tại đây
			int random = rand() % 4; // random = 0 - 3
			IsDelete = false;
			position = D3DXVECTOR2(250.0f, 250.0f);
			SetDirection(D_Right);

			count_Spawn = 2.0f;
		}
	}
	else
	{
		count_ChangeDirection -= _dt;
		if (count_ChangeDirection <= 0 || direction == D_Stand)
		{
			count_ChangeDirection = 2.0f;
			int random = rand() % 4;
			if (random == 0)
			{
				SetDirection(D_Left);
			}
			else if (random == 1)
			{
				SetDirection(D_Right);
			}
			else if (random == 2)
			{
				SetDirection(D_Up);
			}
			else if (random == 3)
			{
				SetDirection(D_Down);
			}
			return;
		}

		position += velocity * _dt;
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
