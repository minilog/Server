#pragma once

#include "Brick.h"

class Water : public Brick
{
public:
	Water(D3DXVECTOR2 _pos)
	{
		BaseInit(_pos);
		Type = ET_Water;
	}
	~Water() {}
};

