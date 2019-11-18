#pragma once

#include <vector>
#include "Bullet.h"

class Player : public Entity
{
	const float speed1 = 150.f;
	const float speed2 = 250.f;
	Direction direction = D_Stand;
	Direction shootDirection = D_Up;
	int level = 1;
	std::vector<D3DXVECTOR2> positionList; // save 30 frames
	std::vector<Direction> shootDirList; // dùng để rollback bắn đạn
	std::vector<Bullet*> bulletList; // trỏ đến

public:
	int LastReceiveTime = -1;
	int LastShootTime = -1;

public:
	Player(int _ID);
	~Player(){}
	void Update(float _dt) override;
	void Update_Rollback(float _dt);
	void MakeCollision(Entity* _en) override { velocity = D3DXVECTOR2(0.f, 0.f); }
	void Write(OutputMemoryBitStream& _os) override;
	void SetPositionInPreviousFrame(int _preFrame);
	void SetDirectionAndVelocity(Direction _dir);
	void AddBullet(Bullet* _b) { bulletList.push_back(_b); }
	Bullet* SpawnBulletInPreviousFrame(int _preFrame);
	void ApplyVelocity(); // sử dụng trong update - trước khi thực hiện va chạm
};

