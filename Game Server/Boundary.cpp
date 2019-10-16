#include "Boundary.h"



Boundary::Boundary(D3DXVECTOR3 position)
{
	Init(position);
	Tag = EntityTypes::boundary;
	bound = Entity::GetBound();
}


RECT Boundary::rect() {
	reg.left = 32;
	reg.right = 32 + this->GetWidth();
	reg.top = 144;
	reg.bottom = 144 + this->GetHeight();

	return reg;
}

RECT Boundary::GetBound()
{
	return bound;
}

void Boundary::BeCollideWith_Bullet()
{

}

Boundary::~Boundary()
{
}


