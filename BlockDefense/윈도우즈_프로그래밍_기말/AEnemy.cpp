#include "AEnemy.h"
#include"UStageManager.h"
#include"ABackground.h"

AEnemy::AEnemy(UGame* game, int updateOrder)
{
	this->uGame = game; this->iUpdateOrder = updateOrder;
}

void AEnemy::Initialize()
{
	v2Pos = Vector2{ -100, -100 };
	this->dSpeed = 200;
}

void AEnemy::Update(float deltaSecond)
{
	if (bIsDestory == false)
	{
		Move(deltaSecond);
		Destory();
	}
}

void AEnemy::Move(float deltaSecond)
{
	CustomList<Vector2>* posList = uGame->GetBackground()->GetWayPointList();
	if (posList != nullptr && bIsDestory == false)
	{
		int len = posList->GetLength();
		if (len > 0)
		{
			if (bIsArrive == true)
			{
				if (iPosIndex == len - 1) bIsDestory = true;
				int frontIndex = iPosIndex + 1;
				v2Dir = posList->GetData(frontIndex) - v2Pos;
				v2Dir.Normalize();
				iPosIndex++;
				bIsArrive = false;
			}
			else
			{
				v2Pos.x += v2Dir.x * deltaSecond * dSpeed; v2Pos.y += v2Dir.y * deltaSecond * dSpeed;
				if (IsInPos(v2Pos, dWidth, dHeight, posList->GetData(iPosIndex), 10))	bIsArrive = true;
			}
		}
	}
}

void AEnemy::Destory()
{
	if (dHp <= 0)
	{
		bIsDestory = true;
	}
}

