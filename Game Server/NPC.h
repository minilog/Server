#pragma once
#include "Entity.h"
#include "PlayerServer.h"

class NPC :public Entity
{
public:
	NPC();
	~NPC();
	void CollideWith_World();
	Action mAction = Idle;
	int mLevel = 1;
	int mHeal = 2;
	float last_random = 0;

	int last_time_die = 0;
	bool is_spawn = false;

	void Respawn();
	void RandomAction();
	void CollisionWith(Entity* en) override;
	void Update(float dt) override;
	RECT GetBound() override;
	void Write(OutputMemoryBitStream& os) override;
	void MoveLeft();
	void MoveRight();
	void MoveUp();
	void MoveDown();
	void IDLE();
};

