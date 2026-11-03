#include "AActor.h"
#include"UComponent.h"

AActor::AActor(UGame* game, int updateOrder)
{
	this->uGame = game;
	this->iUpdateOrder = updateOrder;
}

AActor::~AActor() { Clear(); }

bool AActor::IsInPos(Vector2 pos1, double width1, double height1, Vector2 pos2, int len)
{
	double pmWidth = width1 / len, pmHeight = height1 / len;
	Vector2 mPos = Vector2{ pos1.x - pmWidth, pos1.y - pmHeight };
	Vector2 pPos = Vector2{ pos1.x + pmWidth, pos1.y + pmHeight };

	if ((mPos.x <= pos2.x && mPos.y <= pos2.y) && (pos2.x <= pPos.x && pos2.y <= pPos.y)) return TRUE;
	return FALSE;

	return false;
}

void AActor::Rotate(HDC memDC1)
{
	float angle = static_cast<float>((dAngle * 3.14) / 180.0);
	double cosine = cos(angle), sine = sin(angle);
	SetGraphicsMode(memDC1, GM_ADVANCED);
	XFORM xform;
	xform.eM11 = static_cast<float>(cosine); xform.eM12 = static_cast<float>(sine);
	xform.eM21 = static_cast<float>(-sine); xform.eM22 = static_cast<float>(cosine);
	xform.eDx = static_cast<float>(v2Pos.x - cosine * v2Pos.x + sine * v2Pos.y);
	xform.eDy = static_cast<float>(v2Pos.y - cosine * v2Pos.y - sine * v2Pos.x);
	SetWorldTransform(memDC1, &xform);
}

void AActor::ResetRotate(HDC memDC1)
{
	SetGraphicsMode(memDC1, GM_COMPATIBLE);
	XFORM xform = XFORM{ 1, 0, 0, 1, 0, 0 };
	SetWorldTransform(memDC1, &xform);
}
