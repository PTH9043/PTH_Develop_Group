#include "ATower5.h"
#include"USimpleSpriteComponent.h"

ATower5::ATower5(UGame* game, int updateOrder)
{
	this->uGame = game; this->iUpdateOrder = updateOrder;
	Initialize();
}

void ATower5::Initialize()
{
	ATower::Initialize();
	uSImpleSpriteComp = new USimpleSpriteComponent(uGame, this,
		uGame->GetTexture(L"Tower5.bmp"), uGame->GetTexture(L"Tower_alpha1.bmp"));
	uTowerStatus = UTowerStatus(8, 4, 5, 2, 2);
	iPayGold = 1000;
	iPayReinForceGold = 300;
}

void ATower5::Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap)
{
	ATower::Draw(memDC1, memDC2, bitmap);
}
