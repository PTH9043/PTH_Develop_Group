#include "ABullet.h"
#include"AEnemy.h"
#include"ATower.h"
#include"USimpleSpriteComponent.h"
#include"UStageManager.h"

ABullet::ABullet(UGame* game, ATower* tower, AEnemy* enemy,  int updateOrder)
{
	this->uGame = game; this->iUpdateOrder = updateOrder;
	this->aTower = tower; this->aTargetEnemy = enemy;
	Initialize();
}

void ABullet::Initialize()
{
	uSimpleSpriteComp = new USimpleSpriteComponent(uGame, this, uGame->GetTexture(L"Bullet.bmp"), uGame->GetTexture(L"Bullet_alpha.bmp"));
	v2Pos = aTower->GetPos();
	dWidth = 25; dHeight = 25;
	dDamage = aTower->GetTowerStatus().GetAttackPower();
}

void ABullet::Update(float deltaTime)
{
	if (bIsDestory == false)
	{
		Move(deltaTime);
		CollisionEnemy();
	}
}

void ABullet::Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap)
{
	if (bIsDestory == false)
	{
		Rotate(memDC1);
		uSimpleSpriteComp->Draw(memDC1, memDC2, bitmap);
		ResetRotate(memDC1);
	}
}

void ABullet::Move(float deltaTime)
{
	Vector2 dir = aTargetEnemy->GetPos() - v2Pos;
	v2Dir = dir; v2Dir.Normalize();
	v2Pos.x += deltaTime * v2Dir.x * 500;
	v2Pos.y += deltaTime * v2Dir.y * 500;
	dAngle = (atan(abs(dir.x) /abs(dir.y)) * 180) / 3.14 + 90;
	if (dAngle >= 360)  dAngle -= 360;
	rCollisioRect = RECT
	{
		static_cast<int>(v2Pos.x - dWidth / 4),
		static_cast<int>(v2Pos.y - dHeight / 8),
		static_cast<int>(v2Pos.x + dWidth / 4),
		static_cast<int>(v2Pos.y + dHeight / 8)
	};
}

void ABullet::CollisionEnemy()
{
	if (bIsDestory == false)
	{
		if (UCollision::CircleToRectangle(aTargetEnemy->GetPos(), aTargetEnemy->GetWidth(), aTargetEnemy->GetHeight(), rCollisioRect))
		{
			bIsDestory = true;
			aTargetEnemy->SetHp(aTargetEnemy->GetHp() - dDamage);
		}
		else
		{
			if (aTargetEnemy->IsDestory() == true)
			{
				int len = uGame->GetStageManager()->GetEnemyList()->GetLength();
				for (int i = 0; i < len; i++)
				{
					AEnemy* enemy = uGame->GetStageManager()->GetEnemyList()->GetData(i);
					if (enemy->IsDestory() == false) { aTargetEnemy = enemy; break; }
				}
			}
		}
	}
}
