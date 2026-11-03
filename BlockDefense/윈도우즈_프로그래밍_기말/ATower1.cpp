#include "ATower1.h"
#include"USimpleSpriteComponent.h"

ATower1::ATower1(UGame* game, int updateOrder)
{
	this->uGame = game; this->iUpdateOrder = updateOrder;
	Initialize();
}

void ATower1::Initialize()
{
	ATower::Initialize();
	uSImpleSpriteComp = new USimpleSpriteComponent(uGame, this, 
	uGame->GetTexture(L"Tower1.bmp"), uGame->GetTexture(L"Tower_alpha1.bmp"));
	uTowerStatus = UTowerStatus(1, 1, 1, 0.5, 0.5);
	iPayGold = 10;
	iPayReinForceGold = 3;
}

void ATower1::Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap)
{
	ATower::Draw(memDC1, memDC2, bitmap);
}
