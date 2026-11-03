#include "ATower.h"
#include"ABullet.h"
#include"USimpleSpriteComponent.h"
#include"UReinForceManager.h"
#include"UInfoManager.h"
#include"UStageManager.h"
#include"UEnemyManager.h"
#include"ABackground.h"
#include"AEnemy.h"

ATower::ATower(UGame* game, int updateOrder)
{
	this->uGame = game; this->iUpdateOrder = updateOrder;
}


void ATower::Initialize()
{
	dWidth = 48; dHeight = 48;
	clBulletList = new CustomList<ABullet*>();
	hRangeColor = CreateSolidBrush(RGB(255, 255, 0));
	dWaitTime = 0.0;
}

void ATower::Update(float deltaSecond)
{
	EnemyInRange(uGame->GetStageManager()->GetEnemyList());
	int len = clBulletList->GetLength();
	for (int i = 0; i < len; i++)
	{
		ABullet* bullet = clBulletList->GetData(i);
		bullet->Update(deltaSecond);
	}
}

void ATower::Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap)
{
	Rotate(memDC1);
	if (bIsInit == true || bIsClicked == true)
	{
		UpdateRange();
		SetROP2(memDC1, R2_XORPEN);
		static_cast<HBRUSH>(SelectObject(memDC1, hRangeColor));
		Ellipse(memDC1, rRangeRect.left, rRangeRect.top, rRangeRect.right, rRangeRect.bottom);
	}
	if (uSImpleSpriteComp != nullptr)	uSImpleSpriteComp->Draw(memDC1, memDC2, bitmap);
	HDC memDC3 = CreateCompatibleDC(memDC2);
	int len = clBulletList->GetLength();
	for (int i = 0; i < len; i++)
	{
		ABullet* bullet = clBulletList->GetData(i);
		bullet->Draw(memDC1, memDC3, bitmap);
	}
	DeleteDC(memDC3);
	ResetRotate(memDC1);
}

void ATower::ClickMouseButton(Vector2 mousePosition)
{
	if (UCollision::PointInCircle(v2Pos, dWidth, dHeight, mousePosition) == true && uGame->GetBackground()->IsPutonState() == false)
	{
		if (uGame->GetReinForceManager()->IsStart() == true)
			uGame->GetReinForceManager()->SetSelectedTower(this);
		else if (uGame->GetInfoManager()->IsStart() == true)
			uGame->GetInfoManager()->SetSelectedTower(this);
		else bIsClicked = true;
	}
	else bIsClicked = false;
}

void ATower::Drag(Vector2 mousePosition)
{
	if(bIsInit == true) v2Pos = mousePosition;
}

void ATower::UpdateRange()
{
	rRangeRect = RECT
	{
		static_cast<int>((v2Pos.x - DRAW_SUPPLEMENT - dWidth / 2 - 80 - (20 * uTowerStatus.GetAttackRange()))),
		static_cast<int>((v2Pos.y  - DRAW_SUPPLEMENT - dHeight / 2 - 80 - (20 * uTowerStatus.GetAttackRange()))),
		static_cast<int>((v2Pos.x  + DRAW_SUPPLEMENT + dWidth / 2 + 80 + (20 * uTowerStatus.GetAttackRange()))),
		static_cast<int>((v2Pos.y  + DRAW_SUPPLEMENT + dHeight / 2 + 80 + (20 * uTowerStatus.GetAttackRange())))
	};
}

void ATower::Clear()
{
	DELETE_MEMORY(uSImpleSpriteComp); DELETE_MEMORY(clBulletList); 
	DeleteObject(hRangeColor);
}

void ATower::EnemyInRange(CustomList<class AEnemy*>* enemyList)
{
	dWaitTime += (DELTA_TIME + (uTowerStatus.GetAttackSpeed() * 0.5));
	int len = enemyList->GetLength();
	if (len > 0)
	{
		for (int i = 0; i < len; i++)
		{
			AEnemy* enemy = enemyList->GetData(i);
			if (enemy->GetHp() > 0)
			{
				double width = static_cast<double>(rRangeRect.right - rRangeRect.left);
				double height = static_cast<double>(rRangeRect.bottom - rRangeRect.top);
				if (UCollision::PointInCircle(v2Pos, width, height, enemy->GetPos()))
				{
					if (dWaitTime >= 1500)
					{
						dWaitTime = 0.0;
						ABullet* bullet = new ABullet(uGame, this, enemy);
						bullet->Initialize();
						clBulletList->Insert(bullet);
						break;
					}
				}
			}
		}
	}
}
