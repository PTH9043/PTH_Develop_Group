#include "ATower10.h"
#include"USimpleSpriteComponent.h"

ATower10::ATower10(UGame* game, int updateOrder)
{
	this->uGame = game; this->iUpdateOrder = updateOrder;
	Initialize();
}

void ATower10::Initialize()
{
	ATower::Initialize();
	uSImpleSpriteComp = new USimpleSpriteComponent(uGame, this,
		uGame->GetTexture(L"Tower10.bmp"), uGame->GetTexture(L"Tower_alpha2.bmp"));
	uTowerStatus = UTowerStatus(35, 8.5, 36.5, 25, 15);
	iPayGold = 15000;
	iPayReinForceGold = 4500;
}

void ATower10::Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap)
{
	ATower::Draw(memDC1, memDC2, bitmap);
}
