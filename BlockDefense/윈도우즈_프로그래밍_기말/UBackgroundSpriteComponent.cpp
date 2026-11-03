#include "UBackgroundSpriteComponent.h"
#include"AActor.h"

UBackgroundSpriteComponent::UBackgroundSpriteComponent(UGame* game, AActor* actor, UPicture* picture)
{
	this->uGame = game;
	this->aOwner = actor;
	uPicture = picture;
}

void UBackgroundSpriteComponent::Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap)
{
	Vector2 pos = aOwner->GetPos();
	double width = aOwner->GetWidth(), height = aOwner->GetHeight();
	pos.x -= width / 2; pos.y -= height / 2;
	SelectObject(memDC2, uPicture->GetBitmap());
	StretchBlt(memDC1, static_cast<int>(pos.x), static_cast<int>(pos.y),
		static_cast<int>(width), static_cast<int>(height), memDC2, 0, 0, uPicture->GetWidth(), uPicture->GetHeight(), SRCCOPY);
}
