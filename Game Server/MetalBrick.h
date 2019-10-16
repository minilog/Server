#pragma once
#include "Brick.h"
class MetalBrick : public Brick
{
public:
	MetalBrick(D3DXVECTOR3 position);
	~MetalBrick();
	RECT GetBound();
	void BeCollideWith_Bullet() override;
	RECT rect();
};

