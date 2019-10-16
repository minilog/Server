#pragma once
#include "Entity.h"

class Item : public Entity
{
public:
	~Item();

	virtual void Update(float dt);


	void BeCollideWith_Player();

	bool getDelete();
protected:
	Item();
	bool Init(D3DXVECTOR3 position);

	float exist_time = 0;
	float cout_time = 0;
	virtual RECT rect() = 0;
	RECT reg;
	bool isDelete;
};

