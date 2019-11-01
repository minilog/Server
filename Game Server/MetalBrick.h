#pragma once

#include "Brick.h"

class MetalBrick : public Brick
{
public:
	MetalBrick(D3DXVECTOR2 _pos)
	{
		BaseInit(_pos);
		Type = ET_MetalBrick;
	}
	~MetalBrick() {}
};