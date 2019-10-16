#pragma once
#include "Item.h"
class UpgradeItem : public Item
{
public:
	UpgradeItem(D3DXVECTOR3 Position);
	~UpgradeItem();


	RECT rect();

	RECT GetBound();
private:
	RECT bound;
};

