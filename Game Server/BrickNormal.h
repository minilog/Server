#pragma once

#include "Brick.h"

class BrickNormal : public Brick
{
public:
	BrickNormal(D3DXVECTOR2 _pos)
	{
		BaseInit(_pos);
		Type = ET_NormalBrick;
	}
	~BrickNormal() {}

	void MakeCollision(Entity* _en) override
	{
		IsDelete = true;
	}

	void Write(OutputMemoryBitStream& _os) override
	{
		_os.Write(IsDelete);
	}
};

