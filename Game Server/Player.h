#pragma once

#include <vector>
#include "Bullet.h"

class Player : public Entity
{
	const float speed = 150.f;
	const float shieldTime = 5.0f;
	Direction direction = D_Stand;
	Direction shootDirection = D_Up;
	int level = 1;
	int HP = 2;
	std::vector<D3DXVECTOR2> positionList; // save 30 frames
	std::vector<Direction> shootDirList; // dùng để rollback bắn đạn
	std::vector<Bullet*> bulletList; // trỏ đến
	float count_Spawn = 3.0f;
	float count_Shoot = 0.5f;
	float count_Shield = 5.0f;

public:
	int LastReceiveTime = -1;
	int LastShootTime = -1;

public:
	Player(int _ID);
	~Player(){}
	void Update(float _dt) override;
	void Update_Rollback(float _dt);
	void CheckCollision(Entity* e);
	void Write(OutputMemoryBitStream& _os) override;
	void SetPositionInPreviousFrame(int _preFrame);
	void SetDirection(Direction _dir);
	void AddBullet(Bullet* _b) { bulletList.push_back(_b); }
	Bullet* SpawnBulletInPreviousFrame(int _preFrame);
	void ApplyVelocity();
	void ChangeHP(int amount);
	void ZeroVelocity() { velocity = D3DXVECTOR2(0, 0); }
	void ApplyShield() { count_Shield = shieldTime; }
	void LevelUp() { level++; if (level > 2) level = 2; }
	int GetDamage() { return level; }
};

