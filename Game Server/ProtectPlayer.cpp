#include "ProtectPlayer.h"



ProtectPlayer::ProtectPlayer(D3DXVECTOR3 Position)
{
	Init(Position);
	Tag = EntityTypes::ProtectPlayerItem;
	bound = Entity::GetBound();
}


ProtectPlayer::~ProtectPlayer()
{
}



RECT ProtectPlayer::rect()
{
	reg.left = 0;
	reg.right = this->GetWidth();
	reg.top = 32;
	reg.bottom = 32 + this->GetHeight();

	return reg;
}

RECT ProtectPlayer::GetBound()
{
	return bound;
}
