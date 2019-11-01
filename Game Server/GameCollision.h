#pragma once
#include "Entity.h"

class GameCollision
{
public:
	// tính toán frame tiếp theo có va chạm hay không
	static bool	IsCollideInNextFrame(Entity* _en1, Entity* _en2, float _dt);
};

