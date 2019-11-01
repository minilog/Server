#pragma once

#include "Item.h"

class ProtectItem : public Item
{
public:
	ProtectItem(D3DXVECTOR2 _pos)
	{
		BaseInit(_pos);
		Type = ET_ProtectItem;
	}
	~ProtectItem() {}

};

