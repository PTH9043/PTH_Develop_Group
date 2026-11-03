#pragma once
#include"UComponent.h"

class UCollisionComponent : public UComponent
{
public:
	UCollisionComponent(){}
	virtual ~UCollisionComponent() { Clear(); }
};

