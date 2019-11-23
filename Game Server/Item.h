#pragma once

#include "Entity.h"

class Item : public Entity
{
	float count_existTime = 7.0f; // đếm
	float count_SpawnTime = 10.0f;
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
				int random = rand() % 20;
				count_SpawnTime = 10.0f + (float)random;

				count_existTime = 7.0f;
				int random1 = rand() % 720;
				int random2 = rand() % 720;
				position = D3DXVECTOR2((float)random1 + 32, (float)random2 + 32);
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

	void Write(OutputMemoryBitStream& os)
	{
		os.Write((int)(position.x * 10), NBit_Position);
		os.Write((int)(position.y * 10), NBit_Position);
		os.Write(IsDelete);
	}

// các biến và hàm hỗ trợ
protected:
	void BaseInit(D3DXVECTOR2 _pos)
	{
		position = _pos;
		width = 24;
		height = 24;

		int random = rand() % 20;
		count_SpawnTime = 10.0f + (float)random;
	}
};

