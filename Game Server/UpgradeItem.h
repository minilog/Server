#pragma once

#include "Item.h"

class UpgradeItem : public Item
{
public:
	UpgradeItem(D3DXVECTOR2 _pos)
	{
		BaseInit(_pos);
		Type = ET_UpgradeItem;
	}
	~UpgradeItem() {}
};

