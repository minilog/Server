#include "NPC.h"

NPC::NPC(int _ID)
{
	ID = _ID;

	Type = ET_NPC;
	IsDelete = true;
	width = 32;
	height = 32;
}

void NPC::Update(float _dt)
{
	if (IsDelete)
		return;

	position += velocity * _dt;
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
