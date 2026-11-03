#include "UPlayerManager.h"
#include"ATower.h"

UPlayerManager::UPlayerManager(UGame* game)
{
	this->uGame = game; 
	Initialize();
}

void UPlayerManager::Initialize()
{
	clTowerList = new CustomList<class ATower*>();
	iGold = 500;
}

void UPlayerManager::Update(float deltaSecond)
{
	int len = clTowerList->GetLength();
	for (int i = 0; i < len; i++)
	{
		ATower* tower = clTowerList->GetData(i);
		tower->Update(deltaSecond);
	}
	if (aSelectedTower != nullptr)	aSelectedTower->Update(deltaSecond);
}

void UPlayerManager::Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap)
{
	int len = clTowerList->GetLength();
	for (int i = 0; i < len; i++)
	{
		ATower* tower = clTowerList->GetData(i);
		tower->Draw(memDC1, memDC2, bitmap);
	}
	if (aSelectedTower != nullptr)	aSelectedTower->Draw(memDC1, memDC2, bitmap);
}

void UPlayerManager::ClickLeftMouserButton(Vector2 mousePosition)
{
	int len = clTowerList->GetLength();
	for (int i = 0; i < len; i++)
	{
		ATower* tower = clTowerList->GetData(i);
		tower->ClickMouseButton(mousePosition);
	}
	if (aSelectedTower != nullptr)	aSelectedTower->ClickMouseButton(mousePosition);
}

void UPlayerManager::MouseDrag(Vector2 mousePosition)
{
	int len = clTowerList->GetLength();
	for (int i = 0; i < len; i++)
	{
		ATower* tower = clTowerList->GetData(i);
		tower->Drag(mousePosition);
	}
	if (aSelectedTower != nullptr)	aSelectedTower->Drag(mousePosition);
}

void UPlayerManager::Clear()
{
	int len = clTowerList->GetLength();
	for (int i = 0; i < len; i++)
		clTowerList->GetData(i)->GetBulletList()->Clear();
}
