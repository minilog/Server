#include "PlayerServer.h"
#include "Bullet.h"

PlayerServer::PlayerServer()
{
	this->Tag = Entity::player;
	this->width = 32;
	this->height = 32;
	this->vx = 0;
	this->vy = 0;
	ID_currentBullet = ID * 10 + 1;

}

PlayerServer::PlayerServer(int ID)
{
	this->ID = ID;

	this->width = 32;
	this->height = 32;
	this->vx = 0;
	this->vy = 0;
	this->Tag = Entity::player;
	ID_currentBullet = ID * 10 + 1;
}

PlayerServer::~PlayerServer()
{

}

void PlayerServer::CollideWith_World()
{
	this->vx = 0;
	this->vy = 0;
}

void PlayerServer::ActiveShield()
{
	is_protect = true;
	time_start_protect = 0;
}

RECT PlayerServer::GetBound()
{
	RECT rect;
	float width = 32 / 2;
	rect.left = this->posX - width;
	rect.right = rect.left + 32;
	rect.top = this->posY - width;
	rect.bottom = rect.top + 32;

	return rect;
}

void PlayerServer::Up_ID_OfBullet()
{
	ID_currentBullet++;
	if (ID_currentBullet == ID * 10 + 5)
		ID_currentBullet = ID * 10 + 1;
}

void PlayerServer::CollisionWith(Entity* en)
{
	if (en->Tag == EntityTypes::UpgradeItem)
	{
		mScore += 200;
		mScore_send = 200;
		position_add.x = GetPosition().x; 	position_add.y = GetPosition().y;
		if (mLevel < 3) mLevel++;
	}
	else if (en->Tag == EntityTypes::ProtectPlayerItem)
	{
		mScore += 100;
		mScore_send = 100;
		position_add.x = GetPosition().x; 	position_add.y = GetPosition().y;
		ActiveShield();
	}
	else if (en->Tag == EntityTypes::bullet)
	{
		if (!is_protect)
			if (mHeal > 0)
				mHeal--;

	}
	else if (en->Tag == EntityTypes::player || en->Tag == EntityTypes::npc)
	{
		vx = 0;
		vy = 0;
	}
}



void PlayerServer::Write(OutputMemoryBitStream& os)
{
	Entity::Write(os);
	os.Write((int)mAction, Define::bitofID);
	os.Write(mLevel, Define::bitofTypePacket);
	os.Write(is_protect);
	os.Write(mHeal, Define::bitofTypePacket);
	os.Write(LastMoveTime);


	os.Write(mScore_send, Define::bitofID);
	os.Write((int)position_add.x, Define::bitofLocation);
	os.Write((int)position_add.y, Define::bitofLocation);
	mScore_send = 0;

}



void PlayerServer::Read(InputMemoryBitStream& is)
{
	Entity::Read(is);
	int action = 0;
	is.Read(action, Define::bitofID);

}

void PlayerServer::onHandleKeyboard(int action)
{
}

void PlayerServer::Update(float dt)
{
	if (isLose) return;
	if (mHeal == 0)
	{
		if (!isWaiting)
		{
			last_time_wait = GetTickCount();
			isWaiting = true;

			SetPosition(0, 0);
		}
	}
	if (isWaiting && GetTickCount() - last_time_wait > 4000)
	{
		int x = 0;
		int y = 0;
		switch (ID)
		{
		case 1: x = 177; y = 713; break;
		case 2:x = 637; y = 593; break;
		case 3:x = 597; y = 73; break;
		case 4:x = 204; y = 113; break;
		}
		SetPosition(x, y);
		ActiveShield();
		mHeal = 2;
		mLevel = 1;
		last_time_wait = 0;
		isWaiting = false;
	}
	Entity::Update(dt);
	if (is_protect)
		time_start_protect += dt;
	if (time_start_protect > 3)
		is_protect = false;

}



void PlayerServer::MoveLeft()
{
	dir = left;
	if (mLevel > 1)
		this->SetVx(-250.0f);
	else
		this->SetVx(-200.0f);

	this->SetVy(0);
}

void PlayerServer::MoveRight()
{
	dir = right;
	if (mLevel > 1)
		this->SetVx(250.0f);
	else
		this->SetVx(200.0f);
	this->SetVy(0);
}

void PlayerServer::MoveUp()
{
	dir = up;
	this->SetVx(0);
	if (mLevel > 1)
		this->SetVy(250.0f);
	else
		this->SetVy(200.0f);

}

void PlayerServer::MoveDown()
{
	dir = down;
	this->SetVx(0);
	if (mLevel > 1)
		this->SetVy(-250.0f);
	else
		this->SetVy(-200.0f);
}

void PlayerServer::IDLE()
{
	this->SetVx(0);
	this->SetVy(0);
}