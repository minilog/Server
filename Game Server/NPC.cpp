#include "NPC.h"



NPC::NPC()
{
	this->Tag = Entity::npc;
	this->width = 32;
	this->height = 32;
	this->vx = 0;
	this->vy = 0;

}


NPC::~NPC()
{
}

void NPC::CollideWith_World()
{
	vx = 0;
	vy = 0;
}

void NPC::Respawn()
{
	last_time_die = GetTickCount();
	is_spawn = true;
}

void NPC::RandomAction()
{
	int x = rand() % (4 - 1 + 1) + 1;
	switch (x)
	{
	case 1:MoveLeft(); break;
	case 2:MoveRight(); break;
	case 3:MoveUp(); break;
	case 4:MoveDown(); break;
	}
}

void NPC::CollisionWith(Entity* en)
{
	if (en->Tag == EntityTypes::bullet)
	{
		if (mHeal > 0)
			mHeal--;
		if (mHeal == 0)
		{
			Respawn();
		}
	}
	else if (en->Tag == EntityTypes::player || en->Tag == EntityTypes::npc)
	{
		vx = 0;
		vy = 0;
	}
}

void NPC::Update(float dt)
{
	if (is_spawn)
	{
		SetPosition(-30, -30);
		if (GetTickCount() - last_time_die > 3000)
		{
			int ran = rand() % (4 - 1 + 1) + 1;
			int x = 0; int y = 0;
			switch (ran)
			{
			case 1: x = 177; y = 713; break;
			case 2:x = 637; y = 593; break;
			case 3:x = 597; y = 73; break;
			case 4:x = 204; y = 113; break;
			}
			SetPosition(x, y);
			is_spawn = false;
			mHeal = 2;
		}
	}
	Entity::Update(dt);
	last_random += dt;
	if (last_random > 1)
	{
		RandomAction();
		last_random = 0;
	}
}

RECT NPC::GetBound()
{
	RECT rect;
	float width = 32 / 2;
	rect.left = this->posX - width;
	rect.right = rect.left + 32;
	rect.top = this->posY - width;
	rect.bottom = rect.top + 32;

	return rect;
}

void NPC::Write(OutputMemoryBitStream& os)
{
	Entity::Write(os);
	int a = (int)dir;
	os.Write(a, Define::bitofID);
	os.Write(mHeal, Define::bitofTypePacket);

}

void NPC::MoveLeft()
{
	dir = left;
	this->SetVx(-200.0f);
	this->SetVy(0);
}

void NPC::MoveRight()
{
	dir = right;
	this->SetVx(200.0f);
	this->SetVy(0);
}

void NPC::MoveUp()
{
	dir = up;
	this->SetVx(0);
	this->SetVy(200.0f);

}

void NPC::MoveDown()
{
	dir = down;
	this->SetVx(0);
	this->SetVy(-200.0f);
}

void NPC::IDLE()
{
	this->SetVx(0);
	this->SetVy(0);
}




