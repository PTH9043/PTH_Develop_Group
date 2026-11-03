#include "ATower11.h"
#include"USimpleSpriteComponent.h"

ATower11::ATower11(UGame* game, int updateOrder)
{
	this->uGame = game; this->iUpdateOrder = updateOrder;
	Initialize();
}

void ATower11::Initialize()
{
	ATower::Initialize();
	uSImpleSpriteComp = new USimpleSpriteComponent(uGame, this,
		uGame->GetTexture(L"Tower11.bmp"), uGame->GetTexture(L"Tower_alpha2.bmp"));
	uTowerStatus = UTowerStatus(50, 9, 51, 25, 20);
	iPayGold = 22000;
	iPayReinForceGold = 6600;
}

void ATower11::Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap)
{
	ATower::Draw(memDC1, memDC2, bitmap);
}
