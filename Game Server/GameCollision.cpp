#include "GameCollision.h"

bool GameCollision::IsCollideInNextFrame(Entity *_en1, Entity *_en2, float _dt) 
{
	// ngăn lỗi tự phát sinh
	if (_en1 == nullptr || _en2 == nullptr)
	{
		return false;
	}

	D3DXVECTOR2 currentPosition1 = _en1->GetPosition();
	D3DXVECTOR2 currentPosition2 = _en2->GetPosition();

	// cho 2 đối tượng cập nhật vị trí 1 frame
	_en1->SetPosition(_en1->GetPosition() + _en1->GetVelocity() * _dt);
	_en2->SetPosition(_en2->GetPosition() + _en2->GetVelocity() * _dt);

	// lấy 2 hcn của frame đó
	RECT rect1 = _en1->GetBound();
	RECT rect2 = _en2->GetBound();

	// đưa 2 đối tượng về lại vị trí ban đầu
	_en1->SetPosition(currentPosition1);
	_en2->SetPosition(currentPosition2);

	// trả về - frame tiếp theo có va chạm hay không
	if (rect1.left > rect2.right || rect1.right < rect2.left || rect1.top > rect2.bottom || rect1.bottom < rect2.top) {

		return false;
	}
	else
	{
		return true;
	}
}

CollisionResult GameCollision::Get_CollisionResult(Entity * e1, Entity * e2)
{
	CollisionResult result;

	RECT rect1 = e1->GetBound();
	RECT rect2 = e2->GetBound();

	if (rect1.left <= rect2.right &&
		rect1.right >= rect2.left &&
		rect1.top <= rect2.bottom &&
		rect1.bottom >= rect2.top)
	{
		result.IsCollided = true;
	}
	else
	{
		result.IsCollided = false;
		// không va chạm thì không xét nữa
		return result;
	}

	// xác định RECT
	result.Rect.left = rect1.left > rect2.left ? rect1.left : rect2.left;
	result.Rect.right = rect1.right < rect2.right ? rect1.right : rect2.right;
	result.Rect.top = rect1.top > rect2.top ? rect1.top : rect2.top;
	result.Rect.bottom = rect1.bottom < rect2.bottom ? rect1.bottom : rect2.bottom;

	// xác định phía va chạm
	D3DXVECTOR2 collisionCenter = D3DXVECTOR2(
		(result.Rect.left + result.Rect.right) / 2.0f,
		(result.Rect.top + result.Rect.bottom) / 2.0f);
	// lấy vector từ e1 đến RECT va chạm
	D3DXVECTOR2 dir = collisionCenter - e1->GetPosition();
	D3DXVec2Normalize(&dir, &dir);

	float a1 = 0.6427876f; // cos(50 độ)
	float a2 = 0.7660444f; // cos(40 độ)

	if (dir.y < 0)
	{
		if (dir.x <= a1 && dir.x >= -a1)
		{
			result.Side = CS_Top;
		}
		else if (dir.x > a2)
		{
			result.Side = CS_Right;
		}
		else if (dir.x < -a2)
		{
			result.Side = CS_Left;
		}
	}
	else
	{
		if (dir.x <= a1 && dir.x >= -a1)
		{
			result.Side = CS_Bottom;
		}
		else if (dir.x > a2)
		{
			result.Side = CS_Right;
		}
		else if (dir.x < -a2)
		{
			result.Side = CS_Left;
		}
	}

	return result;
}
