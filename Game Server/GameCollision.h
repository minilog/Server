#pragma once
#include "Entity.h"
class GameCollision
{
public:
	GameCollision();
	~GameCollision();

	static bool	isCollide(Entity *obj1, Entity *obj2, float dt);
};

