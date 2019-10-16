#pragma once
#include "Item.h"
class ProtectPlayer : public Item
{

public:
	ProtectPlayer(D3DXVECTOR3 Position);
	~ProtectPlayer();

	RECT rect();

	RECT GetBound();
private:
	RECT bound;
};

