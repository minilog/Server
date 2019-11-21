#pragma once

#include "Entity.h"

class Item : public Entity
{
	float count_existTime = 7.0f; // đếm
	float count_SpawnTime = 5.0f;
public:
	Item() {}
	~Item() {}

	void Update(float _dt) override
	{
		if (IsDelete)
		{
			count_SpawnTime -= _dt;
			if (count_SpawnTime < 0)
			{
				count_SpawnTime = 5.0f;
				count_existTime = 7.0f;
				IsDelete = false;
			}
			return;
		}

		count_existTime -= _dt;
		if (count_existTime < 0)
		{
			count_existTime = 7.0f;
			IsDelete = true;
		}
	}
	
	void MakeCollision(Entity* _en) override
	{
		IsDelete = true;
	}

// các biến và hàm hỗ trợ
protected:
	void BaseInit(D3DXVECTOR2 _pos)
	{
		position = _pos;
		width = 32;
		height = 32;
	}
};

