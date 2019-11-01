#include "GameCollision.h"

bool GameCollision::IsCollideInNextFrame(Entity *_en1, Entity *_en2, float _dt) {

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