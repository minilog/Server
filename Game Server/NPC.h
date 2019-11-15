#pragma once

#include "Entity.h"

class NPC :public Entity
{
	const float speed = 150.f;
	Direction direction; // hướng di chuyển
	float count_Spawn = 2.0f;
	float count_ChangeDirection = 2.0f;

public:
	NPC(int _ID);
	~NPC() {}
	void Update(float _dt) override;
	void MakeCollision(Entity* _en) override { velocity = D3DXVECTOR2(0.f, 0.f); direction = D_Stand; }
	void Write(OutputMemoryBitStream& _os) override;
	void SetDirection(Direction _dir); // set direction & vận tốc
};

