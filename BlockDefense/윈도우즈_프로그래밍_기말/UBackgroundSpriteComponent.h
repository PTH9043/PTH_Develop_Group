#pragma once
#include "USpriteComponent.h"

class UBackgroundSpriteComponent : public USpriteComponent
{
public:
	UBackgroundSpriteComponent() {}
	UBackgroundSpriteComponent(class UGame* game, class AActor* actor, UPicture* picture);
	virtual ~UBackgroundSpriteComponent() { Clear(); }

	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;
protected:

	void Clear() override { USpriteComponent::Clear(); DELETE_MEMORY(uPicture); }

public:
	UPicture* GetPicture() { return uPicture; }
protected:
	UPicture* uPicture = nullptr;
};

