#pragma once
#include"UComponent.h"

class UMovementComponent : public UComponent
{
public:
	UMovementComponent() {}
	UMovementComponent(class UGame* game, class AActor* actor);
	virtual ~UMovementComponent() { Clear(); }

	virtual void Move();

	void SetSpeed(int speed) { this->iSpeed = speed; }
	int GetSpeed() const { return iSpeed; }
protected:
	int iSpeed = 0;
};

