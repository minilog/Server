#include "Entity.h"

RECT Entity::GetBound()
{
	RECT bound;

	bound.left = (long)(position.x - width / 2.f);
	bound.right = (long)(position.x + width / 2.f);
	bound.top = (long)(position.y - height / 2.f);
	bound.bottom = (long)(position.y + height / 2.f);

	return bound;
}