#pragma once

#include "Entity.h"

class Brick : public Entity
{
public:
	Brick() {}
	~Brick() {}

// các hàm hỗ trợ
protected:
	void BaseInit(D3DXVECTOR2 _pos)
	{
		position = _pos;
		width = 16;
		height = 16;
	}
};