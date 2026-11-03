#include "UMovementComponent.h"
#include"AActor.h"

UMovementComponent::UMovementComponent(UGame* game, AActor* actor)
{
	this->uGame = game;
	this->aOwner = actor;
}

void UMovementComponent::Move()
{
	if (aOwner->GetDir().x != 0 || aOwner->GetDir().y != 0)
	{
		Vector2 pos = aOwner->GetPos();
		pos.x += aOwner->GetDir().x * iSpeed * 0.001 * DELTA_TIME;
		pos.y += aOwner->GetDir().y * iSpeed * 0.001 * DELTA_TIME;
		aOwner->SetPos(pos);
	}
}
