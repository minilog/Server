#include "Entity.h"

Entity::Entity()
{
	Tag = none;
}

void Entity::Write(OutputMemoryBitStream& os)
{

	os.Write((int)Tag, Define::bitofID);
	os.Write(ID, Define::bitofID);
	os.Write((int)dir, Define::bitofID);
	os.Write((int)posX, Define::bitofLocation);
	os.Write((int)posY, Define::bitofLocation);

}

void Entity::Read(InputMemoryBitStream& is)
{
	int dir_temp = 0;
	//is.Read(tag, Define::bitofID);
	//is.Read(ID, Define::bitofID);
	is.Read(dir_temp, Define::bitofID);
	int x = 0; int y = 0;
	is.Read(x, Define::bitofLocation);
	is.Read(y, Define::bitofLocation);
	SetPosition(x, y);
	//Tag = (EntityTypes)tag;
	dir = (Direction)dir_temp;
}
D3DXVECTOR2 Entity::GetPosition()
{
	return D3DXVECTOR2(posX, posY);
}

RECT Entity::GetBound()
{
	RECT bound;

	bound.left = posX - width / 2;
	bound.right = posX + width / 2;
	bound.top = posY - height / 2;
	bound.bottom = posY + height / 2;

	return bound;
}

void Entity::OnSetPosition(D3DXVECTOR2 pos)
{

}

void Entity::Update(float dt)
{
	//velocity = pixel / s
	posX += vx * dt;
	posY += vy * dt;
}

void Entity::CollisionWith(Entity* en)
{


}

void Entity::SetPosition(float x, float y)
{
	posX = x;
	posY = y;
}

void Entity::Emplace(Entity* en)
{
	SetPosition(en->GetPosition());
	vx = en->vx;
	vy = en->vy;
	Tag = en->Tag;
	dir = en->dir;
	width = en->width;
	height = en->height;
}


void Entity::SetPosition(D3DXVECTOR2 pos)
{
	SetPosition(pos.x, pos.y);
}

void Entity::SetPosition(D3DXVECTOR3 pos)
{
	SetPosition(pos.x, pos.y);
}


void Entity::AddPosition(D3DXVECTOR2 pos)
{
	this->SetPosition(this->GetPosition() + pos);
}


void Entity::AddPosition(float x, float y)
{
	AddPosition(D3DXVECTOR2(x, y));
}

void Entity::SetWidth(int width)
{
	this->width = width;
}

int Entity::GetWidth()
{
	return width;
}

void Entity::SetHeight(int height)
{
	this->height = height;
}

int Entity::GetHeight()
{
	return height;
}

float Entity::GetVx()
{
	return vx;
}

void Entity::SetVx(float vx)
{
	this->vx = vx;
}

void Entity::AddVx(float vx)
{
	this->vx += vx;
}

float Entity::GetVy()
{
	return vy;
}

void Entity::SetVy(float vy)
{
	this->vy = vy;
}

void Entity::AddVy(float vy)
{
	this->vy += vy;
}
