#include "ATower6.h"
#include"USimpleSpriteComponent.h"

ATower6::ATower6(UGame* game, int updateOrder)
{
	this->uGame = game; this->iUpdateOrder = updateOrder;
	Initialize();
}

void ATower6::Initialize()
{
	ATower::Initialize();
	uSImpleSpriteComp = new USimpleSpriteComponent(uGame, this,
		uGame->GetTexture(L"Tower6.bmp"), uGame->GetTexture(L"Tower_alpha1.bmp"));
	uTowerStatus = UTowerStatus(13, 5, 10, 6, 5);
	iPayGold = 2000;
	iPayReinForceGold = 600;
}

void ATower6::Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap)
{
	ATower::Draw(memDC1, memDC2, bitmap);
}
