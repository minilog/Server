#include "Boss.h"
#include "Define.h"


Boss::Boss(int ID, D3DXVECTOR2 pos)
{
	this->Entity::SetPosition(pos);
	this->ID = ID;
	width = 50;
	height = 50;
	Tag = boss;
}


void Boss::Write(OutputMemoryBitStream& os)
{
	os.Write((int)Tag, Define::bitofID);
	os.Write(ID, Define::bitofID);
	os.Write(mHP, Define::bitofTypePacket);
}

void Boss::Update(float dt)
{

}

Boss::~Boss()
{
}
