#include "ATower7.h"
#include"USimpleSpriteComponent.h"

ATower7::ATower7(UGame* game, int updateOrder)
{
	this->uGame = game; this->iUpdateOrder = updateOrder;
	Initialize();
}

void ATower7::Initialize()
{
	ATower::Initialize();
	uSImpleSpriteComp = new USimpleSpriteComponent(uGame, this,
		uGame->GetTexture(L"Tower7.bmp"), uGame->GetTexture(L"Tower_alpha2.bmp"));
	uTowerStatus = UTowerStatus(17, 6, 12, 10, 10);
	iPayGold = 4000;
	iPayReinForceGold = 1200;
}

void ATower7::Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap)
{
	ATower::Draw(memDC1, memDC2, bitmap);
}
