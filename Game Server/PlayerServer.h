#pragma once

#include <vector>
#include "Bullet.h"

enum Action
{
	Idle,
	GoLeft,
	GoRight,
	GoUp,
	GoDown,
	Fight
};
class PlayerServer : public Entity
{
public:
	PlayerServer();
	PlayerServer(int ID);
	~PlayerServer();
	void CollideWith_World();
	int ID_currentBullet = 0;
	Action mAction = Idle;
	int mScore = 0;
	int mLevel = 1;
	int mHeal = 2;
	std::string mName = "";
	bool is_protect = false;
	float time_start_protect = 0;

	bool isLose = false;
	int last_id_packet = 0;

	int mScore_send = 0;
	D3DXVECTOR2 position_add = D3DXVECTOR2(0, 0);

	int id_of_bullet = 10;
	float last_move_time = 0;

	void ActiveShield();
	void OnChangeAction(Action action);
	RECT GetBound() override;
	void Up_ID_OfBullet();
	void CollisionWith(Entity* en) override;
	void Write(OutputMemoryBitStream& os) override;
	void Read(InputMemoryBitStream& is) override;
	void onHandleKeyboard(int Action);
	void Update(float dt) override;
private:
	bool isWaiting = false;
	int last_time_wait = 0;
	void MoveLeft();
	void MoveRight();
	void MoveUp();
	void MoveDown();
	void IDLE();
};

