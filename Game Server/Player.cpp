#include "Player.h"
#include "GameCollision.h"

Player::Player(int _ID)
{
	ID = _ID;
	Type = ET_Player;
	width = 30;
	height = 30;

	switch (_ID)
	{
	case 0:
		position = D3DXVECTOR2(55.f, 55.f);
		break;
	case 1:
		position = D3DXVECTOR2(745.f, 55.f);
		break;
	case 2:
		position = D3DXVECTOR2(55.f, 745.f);
		break;
	case 3:
		position = D3DXVECTOR2(745.f, 745.f);
		break;
	default:
		break;
	}

	for (int i = 0; i < 30; i++)
	{
		positionList.push_back(position);
		shootDirList.push_back(D_Stand);
	}

	LastReceiveTime = (int)GetTickCount();
	LastShootTime = (int)GetTickCount();
}

void Player::Update(float _dt)
{
	if (IsDelete)
	{
		switch (ID)
		{
		case 0:
			position = D3DXVECTOR2(55.f, 55.f);
			break;
		case 1:
			position = D3DXVECTOR2(745.f, 55.f);
			break;
		case 2:
			position = D3DXVECTOR2(55.f, 745.f);
			break;
		case 3:
			position = D3DXVECTOR2(745.f, 745.f);
			break;
		}
		SetDirection(D_Stand);

		count_Spawn -= _dt;
		if (count_Spawn <= 0)
		{
			count_Spawn = 2.0f;

			// spawn NPC tại đây
			IsDelete = false;
			ApplyShield();
			level = 1;
		}
	}
	else
	{
		count_Shoot -= _dt;
	}

	position += velocity * _dt;
	// delete in the begin, add in the end
	positionList.erase(positionList.begin());
	positionList.push_back(position);

	shootDirList.erase(shootDirList.begin());
	shootDirList.push_back(shootDirection);

	count_Shield -= _dt;
}

void Player::Update_Rollback(float _dt)
{
	position += velocity * _dt;

	positionList.push_back(position);
	shootDirList.push_back(shootDirection);
}

void Player::CheckCollision(Entity * e)
{
	if (IsDelete)
		return;



	if (e->Type == ET_MetalBrick || e->Type == ET_NormalBrick || e->Type == ET_Boundary || e->Type == ET_Water)
	{
		CollisionResult cR = GameCollision::Get_CollisionResult(this, e);
		if (cR.IsCollided)
		{
			if (cR.Side == CS_Left)
			{
				position.x += (float)(cR.Rect.right - cR.Rect.left) + 1;
			}
			else if (cR.Side == CS_Right)
			{
				position.x -= (float)(cR.Rect.right - cR.Rect.left) - 1;
			}
			else if (cR.Side == CS_Top)
			{
				position.y += (float)(cR.Rect.bottom - cR.Rect.top) + 1;
			}
			else if (cR.Side == CS_Bottom)
			{
				position.y -= (float)(cR.Rect.bottom - cR.Rect.top) - 1;
			}
		}
	}
}

void Player::Write(OutputMemoryBitStream & _os)
{
	int x = int(position.x * 10);
	int y = int(position.y * 10);

	_os.Write(x, NBit_Position);
	_os.Write(y, NBit_Position);

	_os.Write(direction, NBit_Direction);


	_os.Write(shootDirection, NBit_Direction);
	_os.Write(IsDelete);
	if (count_Shield > 0)
	{
		_os.Write(true); // đang có shield
	}
	else
	{
		_os.Write(false); // đang ko có shiled
	}
	_os.Write(level, 3);
	_os.Write(Score, NBit_Position);
}	

void Player::SetPositionInPreviousFrame(int _preFrame)
{
	// xóa các frame từ lúc nhận input cho đến hiện tại
	position = positionList[30 - _preFrame - 1];
	for (int i = 0; i < _preFrame; i++)
	{
		positionList.pop_back();
		shootDirList.pop_back();
	}
}

void Player::SetDirection(Direction _dir)
{
	direction = _dir;
	
	if (direction != D_Stand)
	{
		shootDirection = direction;
	}
}

Bullet* Player::SpawnBulletInPreviousFrame(int _preFrame)
{
	if (count_Shoot > 0)
		return nullptr;

	count_Shoot = 0.5f;

	for (auto bullet : bulletList)
	{
		if (bullet->IsDelete == true)
		{
			bullet->Spawn(positionList[30 - _preFrame - 1], shootDirList[30 - _preFrame - 1], level);
			return bullet;
			break;
		}
	}

	return nullptr; // ERROR
}

void Player::ApplyVelocity()
{
	switch (direction)
	{
	case D_Stand:
		velocity = D3DXVECTOR2(0.f, 0.f);
		break;
	case D_Left:
		velocity = D3DXVECTOR2(-speed, 0.f);
		break;
	case D_Right:
		velocity = D3DXVECTOR2(speed, 0.f);
		break;
	case D_Up:
		velocity = D3DXVECTOR2(0.f, -speed);
		break;
	case D_Down:
		velocity = D3DXVECTOR2(0.f, speed);
		break;
	}
}

void Player::ChangeHP(int amount)
{
	if (amount < 0 && count_Shield > 0)
		return;

	HP += amount;
	if (HP <= 0)
	{
		HP = 2;
		IsDelete = true;
	}
}

void Player::WriteScorePosition(OutputMemoryBitStream & os)
{
	int x = int(ScorePosition.x * 10);
	int y = int(ScorePosition.y * 10);

	os.Write(x, NBit_Position);
	os.Write(y, NBit_Position);
}
