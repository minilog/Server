#pragma once

#include <d3dx9.h>
#include "MemoryBitStream.h"
#include "GameDefine.h"
using namespace Define;

class Entity
{
protected:
	D3DXVECTOR2 position = D3DXVECTOR2(0.f, 0.f);
	D3DXVECTOR2 velocity = D3DXVECTOR2(0.f, 0.f);
	int width = 0, height = 0; // nên là số chẵn
public:
	int ID = -1; // -1: chưa xác định
	EntityType Type = ET_Unknown; // nhận diện Entity
	bool IsDelete = false;

public:
	Entity() {}
	~Entity() {}

	virtual void Update(float dt) {}
	virtual void Draw() {}
	virtual void MakeCollision(Entity* _en) {}	// thực hiện va chạm
	virtual void Write(OutputMemoryBitStream &_os) {}
	virtual void Read(InputMemoryBitStream& _is) {}

	RECT GetBound();
	D3DXVECTOR2 GetPosition() { return position; }
	void SetPosition(D3DXVECTOR2 _pos) { position = _pos; }
	D3DXVECTOR2 GetVelocity() { return velocity; }
	void SetVelocity(D3DXVECTOR2 _vel) { velocity = _vel; }
};

