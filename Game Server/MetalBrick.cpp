#include "MetalBrick.h"

MetalBrick::MetalBrick(D3DXVECTOR3 position)
{
	Init(position);
	Tag = EntityTypes::MetalBrick;
	bound = Entity::GetBound();
}

RECT MetalBrick::rect() {
	reg.left = 0;
	reg.right = this->GetWidth();
	reg.top = 144;
	reg.bottom = reg.top + this->GetHeight();

	return reg;
}
void MetalBrick::BeCollideWith_Bullet()
{
}
MetalBrick::~MetalBrick()
{
}

RECT MetalBrick::GetBound()
{
	return bound;
}
