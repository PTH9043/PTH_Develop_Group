#pragma once
#include"UGame.h"

class UComponent
{
public:
	UComponent(){}
	UComponent(class UGame* game, class AActor* actor);
	virtual ~UComponent() { Clear(); }

	virtual void Update(float deltaSecond){}

	int GetComponentID() const { return iComponentID; }
protected:
	virtual void Clear() {}
protected:
	int iComponentID = 0;
	class UGame* uGame = nullptr;
	class AActor* aOwner = nullptr;
};

