#pragma once
#include "Entity.h"

class Bullet :public Entity
{
public:
	Bullet();
	Bullet(int id, int id_pl);
	Bullet(int id);
	~Bullet();
	bool isActive = false;
	bool isChange = false;
	RECT GetBound();
	int ID_of_player = 0;

	/*void Emplace(Bullet *newBullet);
	bool Compare(Bullet* newBullet);*/
	void CollisionWith(Entity* en) override;
	void Update(float dt) override;
	void SetActive(bool flag);
	void setMoveDirection(Direction direction);
	void Read(InputMemoryBitStream& is) override;
	void Write(OutputMemoryBitStream& os) override;
};


