#include "BrickNormal.h"



BrickNormal::BrickNormal(D3DXVECTOR3 position)
{
	Init(position);
	Tag = EntityTypes::Brick;
	bound = Entity::GetBound();
}

BrickNormal::~BrickNormal() {}


RECT BrickNormal::rect() {
	reg.left = 0;
	reg.right = this->GetWidth();
	reg.top = 128;
	reg.bottom = 128 + this->GetHeight();

	return reg;
}

void BrickNormal::Update()
{
}

void BrickNormal::CollisionWith(Entity* en)
{
	if (en->Tag == EntityTypes::bullet)
		isDelete = true;
}



RECT BrickNormal::GetBound()
{
	return bound;
}
