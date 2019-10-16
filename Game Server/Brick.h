#pragma once
#include "Entity.h"

class Brick : public Entity
{
public:
	~Brick();

	virtual void Update();

	void Draw(D3DXVECTOR3 position = D3DXVECTOR3(), RECT sourceRect = RECT(), D3DXVECTOR2 scale = D3DXVECTOR2(), D3DXVECTOR2 transform = D3DXVECTOR2(), float angle = 0, D3DXVECTOR2 rotationCenter = D3DXVECTOR2(), D3DXCOLOR colorKey = D3DCOLOR_XRGB(255, 255, 255));

	void OnSetPosition(D3DXVECTOR3 position);

	virtual void BeCollideWith_Bullet();
	void Write(OutputMemoryBitStream& os) override;
	void Read(InputMemoryBitStream& is) override;
	void setDelete(bool deleted);
	bool getDelete();
protected:
	Brick();
	bool Init(D3DXVECTOR3 position);

	virtual RECT rect() = 0;
	RECT bound;
	RECT reg;

};

