#include "Water.h"



Water::Water(D3DXVECTOR3 position)
{
	Init(position);
	Tag = EntityTypes::Water;
	bound = Entity::GetBound();
}



RECT Water::rect() {
	reg.left = 0;
	reg.right = this->GetWidth();
	reg.top = 160;
	reg.bottom = 160 + this->GetHeight();

	return reg;
}
Water::~Water()
{
}

RECT Water::GetBound()
{
	return bound;
}

void Water::BeCollideWith_Bullet()
{
}

