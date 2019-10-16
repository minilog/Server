#include "GameCollision.h"

GameCollision::GameCollision() {}

bool GameCollision::isCollide(Entity *ent1, Entity *ent2, float dt) {
	//if (abs(ent1->GetPosition().x - ent2->GetPosition().x)>50) return false;
	//if (abs(ent1->GetPosition().y - ent2->GetPosition().y)>50) return false;

	float cur1X = ent1->GetPosition().x;
	float cur1Y = ent1->GetPosition().y;
	float cur2X = ent2->GetPosition().x;
	float cur2Y = ent2->GetPosition().y;
	Entity *NextPos_ent1 = ent1;
	Entity *NextPos_ent2 = ent2;

	NextPos_ent1->SetPosition(ent1->GetPosition().x + ent1->GetVx()*dt, ent1->GetPosition().y + ent1->GetVy()*dt);
	NextPos_ent2->SetPosition(ent2->GetPosition().x + ent2->GetVx()*dt, ent2->GetPosition().y + ent2->GetVy()*dt);

	RECT rect1 = NextPos_ent1->GetBound();
	RECT rect2 = NextPos_ent2->GetBound();

	if (rect1.left > rect2.right || rect1.right < rect2.left || rect1.top > rect2.bottom || rect1.bottom < rect2.top) {
		ent1->SetPosition(cur1X, cur1Y);
		ent2->SetPosition(cur2X, cur2Y);
		return false;
	}
	ent1->SetPosition(cur1X, cur1Y);
	ent2->SetPosition(cur2X, cur2Y);
	return true;
}

GameCollision::~GameCollision()
{
}
