#pragma once
#include "Brick.h"
class Boundary : public Brick
{
public:
	Boundary(D3DXVECTOR3 position);
	~Boundary();
	RECT rect();

	RECT GetBound();
	void BeCollideWith_Bullet() override;
};

