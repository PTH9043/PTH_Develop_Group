#pragma once
#include"UComponent.h"

class USpriteComponent : public UComponent
{
public:
	USpriteComponent() {}
	USpriteComponent(UGame* game, AActor* actor);
	virtual ~USpriteComponent() {}

	virtual void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) {}
};

