#pragma once

#include "Entity.h"

class Eagle :public Entity
{
	int HP = 2;
public:
	Eagle(int _ID, D3DXVECTOR2 _pos)
	{
		ID = _ID;

		Type = ET_Eagle;
		position = _pos;
		width = 50;
		height = 50;
	}
	~Eagle() {}

	void Write(OutputMemoryBitStream& _os) override
	{
		_os.Write(IsDelete);
	}

	void MakeCollision(Entity* _en) override
	{
		HP -= 1;
		if (HP <= 0)
		{
			IsDelete = true;
		}
	}
};

