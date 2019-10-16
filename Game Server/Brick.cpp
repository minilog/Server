#include "Brick.h"

void Brick::setDelete(bool deleted)
{
	isDelete = deleted;
}
bool Brick::getDelete()
{
	return isDelete;
}
Brick::Brick()
{
	isDelete = false;
}

bool Brick::Init(D3DXVECTOR3 position) {
	Entity::SetWidth(16);
	Entity::SetHeight(16);

	SetPosition(position);

	return true;
}
void Brick::OnSetPosition(D3DXVECTOR3 position) {

}

void Brick::BeCollideWith_Bullet()
{

}

void Brick::Write(OutputMemoryBitStream& os)
{
	os.Write((int)Tag, Define::bitofID);
	os.Write(ID, Define::bitofID);
	os.Write(isDelete);
}


void Brick::Read(InputMemoryBitStream& is)
{


}



void Brick::Update()
{
}

void Brick::Draw(D3DXVECTOR3 position, RECT sourceRect, D3DXVECTOR2 scale, D3DXVECTOR2 transform, float angle, D3DXVECTOR2 rotationCenter, D3DXCOLOR colorKey)
{

}

Brick::~Brick()
{
}
