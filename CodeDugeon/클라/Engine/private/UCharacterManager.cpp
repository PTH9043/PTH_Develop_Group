#include "EngineDefine.h"
#include "UCharacterManager.h"
#include "UCharacter.h"
#include "UTransform.h"
#include "UPlayer.h"

UCharacterManager::UCharacterManager() : m_spCurrentPlayer{nullptr}
{
}

void UCharacterManager::RegisterCurrentPlayer(CSHPTRREF<UCharacter> _spCurrentPlayer)
{
	assert(nullptr != _spCurrentPlayer);
	m_spCurrentPlayer = _spCurrentPlayer;
	m_CollisionPawnList.insert(std::static_pointer_cast<UPawn>(_spCurrentPlayer));
	m_PlayerContainer.insert(std::static_pointer_cast<UPlayer>(_spCurrentPlayer));
}

void UCharacterManager::AddCollisionPawnList(CSHPTRREF<UPawn> _spPawn)
{
	assert(nullptr != _spPawn);
	m_CollisionPawnList.insert(_spPawn);
}

void UCharacterManager::RemoveCollisionPawn(CSHPTRREF<UPawn> _spPawn)
{
	m_CollisionPawnList.erase(_spPawn);
}

void UCharacterManager::RemovePlayerInContainer(CSHPTRREF<UPlayer> _spPlayer)
{
	m_PlayerContainer.erase(_spPlayer);
}

void UCharacterManager::AddPlayerInContainer(SHPTR<UPlayer> _spPlayer)
{
	m_PlayerContainer.insert(_spPlayer);
}

SHPTR<UPlayer> UCharacterManager::FindPlayerToNetworkID(_int _iNetworkID)
{
	for (auto& iter : m_PlayerContainer)
	{
		if (iter->GetNetworkID() == _iNetworkID)
			return iter;
	}

	return nullptr;
}

SHPTR<UPlayer> UCharacterManager::FindPlayerToDistance(const _float3& _vPos)
{
	SHPTR<UPlayer> spPlayer;
	_float fDistance = 99999999999;

	for (auto& iter : m_PlayerContainer)
	{
		_float fComputeDistance = iter->GetTransform()->ComputeDistance(_vPos);
		if (fComputeDistance <= fDistance)
		{
			fDistance = fComputeDistance;
			spPlayer = iter;
		}
	}
	return spPlayer;
}

void UCharacterManager::TickCollider(const _double& _dTimeDelta)
{
	for (auto& Pawn : m_CollisionPawnList)
	{
		for (auto& Coll : m_CollisionPawnList)
		{
			if (Pawn == Coll)
				continue;
			if (_float3::Distance(Pawn->GetTransform()->GetPos(), Coll->GetTransform()->GetPos()) < 40)
			{
				Pawn->IsHit(Coll, _dTimeDelta);
			}
		}
	}
}

void UCharacterManager::ClearData()
{
	m_spCurrentPlayer = nullptr;
	m_CollisionPawnList.clear();
}

void UCharacterManager::Free()
{
}
