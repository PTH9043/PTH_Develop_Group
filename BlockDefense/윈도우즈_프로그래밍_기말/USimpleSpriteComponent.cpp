#include "USimpleSpriteComponent.h"

#include"AActor.h"

USimpleSpriteComponent::USimpleSpriteComponent(UGame* game, AActor* actor, UPicture * picture, UPicture* alphaPicture)
{
	this->uGame = game; 
	this->aOwner = actor;
	uPicture = picture;
	uAlphaPicture = alphaPicture;
	iComponentID = COMP_ID_SPRITE_SIMPLE;
}

void USimpleSpriteComponent::Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap)
{
	if (uPicture != nullptr)
	{
		Vector2 pos = aOwner->GetPos();
		double width = aOwner->GetWidth(), height = aOwner->GetHeight();		
		pos.x -= width / 2; pos.y -= height / 2;
		static_cast<HBITMAP>(SelectObject(memDC2, uAlphaPicture->GetBitmap()));
		StretchBlt(memDC1, static_cast<int>(pos.x), static_cast<int>(pos.y),
			static_cast<int>(width), static_cast<int>(height), memDC2, 0, 0, uAlphaPicture->GetWidth(), uAlphaPicture->GetHeight(), SRCAND);
		static_cast<HBITMAP>(SelectObject(memDC2, uPicture->GetBitmap()));
		StretchBlt(memDC1, static_cast<int>(pos.x), static_cast<int>(pos.y),
			static_cast<int>(width), static_cast<int>(height), memDC2, 0, 0, uPicture->GetWidth(), uPicture->GetHeight(), SRCPAINT);
	}
}
