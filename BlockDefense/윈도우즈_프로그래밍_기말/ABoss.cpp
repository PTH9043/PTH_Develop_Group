#include "ABoss.h"

ABoss::ABoss(UGame* game, double hp, HBRUSH color, int updateOrder)
{
	this->uGame = game; this->dHp = hp; this->hColor = color; this->iUpdateOrder = updateOrder;
	Initialize();
}

void ABoss::Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap)
{
	if (bIsDestory == false)
	{
		Rotate(memDC1);
		SelectObject(memDC1, hColor);
		rCollisionRect = RECT{
			static_cast<int>(v2Pos.x - dWidth / 2),
			static_cast<int>(v2Pos.y - dHeight / 2),
			static_cast<int>(v2Pos.x + dWidth / 2),
			static_cast<int>(v2Pos.y + dHeight / 2)
		};
		Ellipse(memDC1, rCollisionRect.left, rCollisionRect.top, rCollisionRect.right, rCollisionRect.bottom);
		ResetRotate(memDC1);
	}
}
