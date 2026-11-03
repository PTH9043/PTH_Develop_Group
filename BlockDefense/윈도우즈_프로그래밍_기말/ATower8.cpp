#include "ATower8.h"
#include"USimpleSpriteComponent.h"

ATower8::ATower8(UGame* game, int updateOrder)
{
	this->uGame = game; this->iUpdateOrder = updateOrder;
	Initialize();
}

void ATower8::Initialize()
{
	ATower::Initialize();
	uSImpleSpriteComp = new USimpleSpriteComponent(uGame, this,
		uGame->GetTexture(L"Tower8.bmp"), uGame->GetTexture(L"Tower_alpha2.bmp"));
	uTowerStatus = UTowerStatus(25, 7, 20, 12, 12);
	iPayGold = 6500;
	iPayReinForceGold = 1950;
}

void ATower8::Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap)
{
	ATower::Draw(memDC1, memDC2, bitmap);
}
