#pragma once
#include "Brick.h"
class BrickNormal : public Brick
{
public:
	BrickNormal(D3DXVECTOR3 position);
	~BrickNormal();

	RECT rect();
	void Update();
	void CollisionWith(Entity* en) override;
	RECT GetBound();

};

