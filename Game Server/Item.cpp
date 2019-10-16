#include "Item.h"

Item::Item()
{
	ID = 0;
	isDelete = false;
	exist_time = 7;
}

bool Item::Init(D3DXVECTOR3 position)
{
	Entity::SetWidth(32);
	Entity::SetHeight(32);
	SetPosition(position);

	return true;
}


Item::~Item()
{
}

void Item::Update(float dt)
{
	cout_time += dt;
	if (cout_time >= exist_time) isDelete = true;
}

void Item::BeCollideWith_Player()
{
	isDelete = true;
}

bool Item::getDelete()
{
	return isDelete;
}
