#include "ABackground.h"
#include "UBackgroundSpriteComponent.h"
#include"UPlayerManager.h"
#include"ATower.h"

ABackground::ABackground(UGame* game, int updateOrder)
{
	this->uGame = game;
	this->iUpdateOrder = updateOrder;
}

ABackground::~ABackground(){ Clear();}

void ABackground::Update(float deltaSecond)
{
	if(uBackgroundSpriteComp != nullptr)
		uBackgroundSpriteComp->Update(deltaSecond);
	TowerInRect();
}

void ABackground::Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap)
{
	if (uBackgroundSpriteComp != nullptr)
		uBackgroundSpriteComp->Draw(memDC1, memDC2, bitmap);
	if (bIsPutonState == true)
	{
		int len = clTowerPosList->GetLength();
		 static_cast<HBRUSH>(SelectObject(memDC1, hCanPutonTowerBrush));
		for (int i = 0; i < len; i++)
		{
			UPosition * pos = clTowerPosList->GetData(i);
			if (pos->IsDraw() == true)
			{
				RECT r = pos->GetCollisionRect();
				Rectangle(memDC1, r.left, r.top, r.right, r.bottom);
			}
		}
	}
}

void ABackground::Initialize()
{
	clWayPointList = new CustomList<Vector2>();
	clTowerPosList = new CustomList<UPosition*>();
	hCanPutonTowerBrush = CreateSolidBrush(RGB(255, 255, 0));
	AddPos();
}

void ABackground::TowerInRect()
{
	ATower* tower = uGame->GetPlayerManager()->GetSelectedTower();
	if (tower != nullptr)
	{
		if (uGame->IsRightMouseClick() == true)
			uGame->GetPlayerManager()->SetSelectedTower(nullptr);
		else
		{
			int len = clTowerPosList->GetLength();
			for (int j = 0; j < len; j++)
			{
				UPosition * pos = clTowerPosList->GetData(j);
				if (pos->IsPutonTower() == false)
				{
					if (UCollision::PointInRect(pos->GetCollisionRect(), tower->GetPos()) == true)
					{
						if (uGame->IsLeftMouseClick() == true && UCollision::PointInRect(pos->GetCollisionRect(), uGame->GetMousePosition()))
						{
							ATower* newTower = tower->Copy();
							newTower->SetPos(pos->GetPos());
							newTower->SetInit(false);
							newTower->UpdateRange();
							uGame->GetPlayerManager()->AddTower(newTower);
							uGame->GetPlayerManager()->SetGold(uGame->GetPlayerManager()->GetGold() - tower->GetPayGold());
							uGame->GetPlayerManager()->SetSelectedTower(nullptr);
							clTowerPosList->GetData(j)->SetDraw(false);
							clTowerPosList->GetData(j)->SetPutonTower(true);
							newTower->SetPosIndex(j);
							bIsPutonState = false;
							uGame->UpdateHaveGold();
							break;
						}
					}
				}
			}
		}
	}
}

void ABackground::Clear()
{
	AActor::Clear();
	DELETE_MEMORY(clWayPointList); DELETE_MEMORY(uBackgroundSpriteComp);
	DELETE_MEMORY(clTowerPosList); DeleteObject(hCanPutonTowerBrush);
}
