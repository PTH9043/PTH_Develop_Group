#include "ATower9.h"
#include"USimpleSpriteComponent.h"

ATower9::ATower9(UGame* game, int updateOrder)
{
	this->uGame = game; this->iUpdateOrder = updateOrder;
	Initialize();
}

void ATower9::Initialize()
{
	ATower::Initialize();
	uSImpleSpriteComp = new USimpleSpriteComponent(uGame, this,
		uGame->GetTexture(L"Tower9.bmp"), uGame->GetTexture(L"Tower_alpha2.bmp"));
	uTowerStatus = UTowerStatus(27, 8, 20, 15, 20);
	iPayGold = 10000;
	iPayReinForceGold = 3000;
}

void ATower9::Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap)
{
	ATower::Draw(memDC1, memDC2, bitmap);
}
