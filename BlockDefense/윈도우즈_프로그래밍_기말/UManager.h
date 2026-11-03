#pragma once
#include"UGame.h"

class UManager
{
public:
	UManager(){}
	UManager(UGame* game) { this->uGame = game; }
	virtual ~UManager(){ }
	 
	virtual void Initialize(){}

	virtual void Reset(){}
protected:
	UGame* uGame = nullptr;
};

