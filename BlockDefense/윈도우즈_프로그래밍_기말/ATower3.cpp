#include "ATower3.h"
#include"USimpleSpriteComponent.h"

ATower3::ATower3(UGame* game, int updateOrder)
{
	this->uGame = game; this->iUpdateOrder = updateOrder;
	Initialize();
}

void ATower3::Initialize()
{
	ATower::Initialize();
	uSImpleSpriteComp = new USimpleSpriteComponent(uGame, this,
		uGame->GetTexture(L"Tower3.bmp"), uGame->GetTexture(L"Tower_alpha1.bmp"));
	uTowerStatus = UTowerStatus(2.5, 2, 1.5, 1, 1);
	iPayGold = 200;
	iPayReinForceGold = 60;
}

void ATower3::Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap)
{
	ATower::Draw(memDC1, memDC2, bitmap);
}
