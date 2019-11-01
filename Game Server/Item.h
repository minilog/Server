#pragma once

#include "Entity.h"

class Item : public Entity
{
	const float existTime = 7.f;

public:
	Item() {}
	~Item() {}

	void Update(float _dt) override
	{
		if (IsDelete)
			return;

		count_existTime += _dt;
		if (count_existTime >= existTime)
		{
			IsDelete = true;
			count_existTime = 0.f;
		}
	}
	
	void MakeCollision(Entity* _en) override
	{
		IsDelete = true;
	}

// các biến và hàm hỗ trợ
private:
	float count_existTime = 0.f; // đếm
protected:
	void BaseInit(D3DXVECTOR2 _pos)
	{
		position = _pos;
		width = 32;
		height = 32;
	}
};

