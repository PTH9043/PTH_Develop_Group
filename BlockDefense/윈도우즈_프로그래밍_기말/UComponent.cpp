#include "UComponent.h"
#include"AActor.h"

UComponent::UComponent(UGame* game, AActor* actor)
{
	this->uGame = game;
	this->aOwner = actor;
	this->iComponentID = 0;
}