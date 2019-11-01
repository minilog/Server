#pragma once

#include "Entity.h"

class NPC :public Entity
{
	const float speed = 200.f;
	Direction direction; // hướng di chuyển
	bool isExplosion = false;

public:
	NPC(int _ID);
	~NPC() {}

	void Update(float _dt) override;
	void MakeCollision(Entity* _en) override { velocity = D3DXVECTOR2(0.f, 0.f); }
	void Write(OutputMemoryBitStream& _os) override {}

	void SetDirection(Direction _dir);
};

