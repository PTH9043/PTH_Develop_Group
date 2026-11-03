#pragma once
#include "USpriteComponent.h"

class USimpleSpriteComponent : public USpriteComponent
{
public:
	USimpleSpriteComponent(){}
	USimpleSpriteComponent(class UGame* game, class AActor* actor, UPicture* picture, UPicture* uAlphaPicture);
	virtual ~USimpleSpriteComponent() { Clear(); }

	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;
protected:

	void Clear() override { USpriteComponent::Clear(); DELETE_MEMORY(uPicture); }
public:
	UPicture* GetPicture()  { return uPicture; }
protected:
	UPicture* uPicture = nullptr;
	UPicture* uAlphaPicture = nullptr;
};

