#include "USpriteComponent.h"

USpriteComponent::USpriteComponent(UGame* game, AActor* actor)
{
	this->uGame = game;
	this->aOwner = actor;
	this->iComponentID = 0;
}