#pragma once

#include <vector>
#include "Bullet.h"

class Player : public Entity
{
	const float speed1 = 200.f;
	const float speed2 = 250.f;
	Direction direction = D_Stand;
	int level = 1;
	std::vector<D3DXVECTOR2> positionList; // save 30 frames
public:
	int LastReceiveTime = -1;

public:
	Player(int _ID);
	~Player(){}
	void Update(float _dt) override;
	void MakeCollision(Entity* _en) override { velocity = D3DXVECTOR2(0.f, 0.f); }
	void Write(OutputMemoryBitStream& _os) override;
	void SetPositionInPreviousFrame(int _preFrame);
	void SetDirection(Direction _dir);
};

