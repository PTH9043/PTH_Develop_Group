#include "ATower2.h"
#include"USimpleSpriteComponent.h"

ATower2::ATower2(UGame* game, int updateOrder)
{
	this->uGame = game; this->iUpdateOrder = updateOrder;
	Initialize();
}

void ATower2::Initialize()
{
	ATower::Initialize();
	uSImpleSpriteComp = new USimpleSpriteComponent(uGame, this,
		uGame->GetTexture(L"Tower2.bmp"), uGame->GetTexture(L"Tower_alpha1.bmp"));
	uTowerStatus = UTowerStatus(2, 1, 1.5, 1, 0.5);
	iPayGold = 50;
	iPayReinForceGold = 15;
}

void ATower2::Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap)
{
	ATower::Draw(memDC1, memDC2, bitmap);
}
