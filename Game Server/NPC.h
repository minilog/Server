#pragma once

#include "Entity.h"
#include "Bullet.h"

class NPC :public Entity
{
	const float speed = 130.f;
	Direction direction; // hướng di chuyển
	float count_Spawn = 2.0f;
	float count_ChangeDirection = 2.0f;
	int HP = 2;

	Bullet *bullet; //trỏ đến
	float count_ShootBullet;

public:
	NPC(int _ID);
	~NPC() {}
	void Update(float _dt) override;
	void ZeroVelocity() { velocity = D3DXVECTOR2(0.f, 0.f); direction = D_Stand; }
	void ChangeHP(int amount);
	void Write(OutputMemoryBitStream& _os) override;
	void SetDirection(Direction _dir); // set direction & vận tốc
	void CheckCollision(Entity* e);
	void AddBulletPtr(Bullet *bullet) { this->bullet = bullet; }
};

