#pragma once

#include "Brick.h"

class Boundary : public Brick
{
public:
	Boundary(D3DXVECTOR2 _pos)
	{
		BaseInit(_pos);
		Type = ET_Boundary;
	}
	~Boundary() {}
};
