#pragma once

#include <d3dx9.h>
#include "MemoryBitStream.h"
#include "Define.h"

class PlayerServer;
enum Direction
{
	left,
	right,
	up,
	down,
	none
};
class Entity
{
public:
	virtual ~Entity() = default;
	Entity();	
	//enum SideCollisions
	//{
	//	Left, //0
	//	Right, //1
	//	Top, //2
	//	Bottom, //3
	//	TopLeft, //4
	//	TopRight, //5
	//	BottomLeft, //6
	//	BottomRight, //7
	//	NotKnow
	//};


	enum EntityTypes
	{
		player, bullet, item, block, none, Brick,
		MetalBrick,
		Water,
		ProtectPlayerItem,
		UpgradeItem,
		boundary,
		npc,
		boss
	};
	bool isDelete = false;
	int ID = 0;
	Direction dir = up;
	EntityTypes Tag = none; //Tag de nhan vien loai Entity
	virtual void Write(OutputMemoryBitStream &os);
	virtual void Read(InputMemoryBitStream &is);

	virtual RECT GetBound();

	virtual void SetPosition(float x, float y);
	virtual void Emplace(Entity* en);
	virtual void SetPosition(D3DXVECTOR2 pos);
	virtual void SetPosition(D3DXVECTOR3 pos);
	virtual void AddPosition(D3DXVECTOR2 pos);

	virtual void AddPosition(float x, float y);

	virtual void SetWidth(int width);

	virtual int GetWidth();

	virtual void SetHeight(int height);

	virtual int GetHeight();

	virtual float GetVx();

	virtual void SetVx(float vx);

	virtual void AddVx(float vx);

	virtual float GetVy();

	virtual void SetVy(float vy);

	virtual void AddVy(float vy);

	virtual D3DXVECTOR2 GetPosition();

	virtual void Update(float dt);
	virtual void CollisionWith(Entity* en);

protected:

	//duoc goi khi set position cua Entity, dung cho ke thua
	virtual void OnSetPosition(D3DXVECTOR2 pos);

	//vi tri tam position x va y
	float posX, posY;

	//phan toc vx, vy
	float vx = 0, vy = 0;

	//size cua entity
	float width, height;
};

