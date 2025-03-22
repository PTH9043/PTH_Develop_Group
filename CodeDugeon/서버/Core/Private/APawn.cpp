#include "CoreDefines.h"
#include "APawn.h"
#include "AAnimController.h"
#include "ACell.h"
#include "ATransform.h"
#include "ANavigation.h"
#include "ACoreInstance.h"

namespace Core {

	APawn::APawn(OBJCON_CONSTRUCTOR, SESSIONID _ID, SESSIONTYPE _SessionType) : 
		AGameObject(OBJCON_CONDATA, _ID, _SessionType), m_spAnimController{nullptr}, m_spNavigation{nullptr}, m_isDamaged{false},
		m_isDead{false}
	{
		m_spNavigation = GetCoreInstance()->CloneNavi();
	}

	void APawn::SetActive(const _bool _isActive)
	{
		__super::SetActive(_isActive);
		if (nullptr != m_spAnimController)
		{
			m_spAnimController->SetOwnerPawnActiveStrong(_isActive);
		}
	}

	void APawn::RestrictPositionToNavi()
	{
		SHPTR<ANavigation> spNavigation = m_spNavigation;
		SHPTR<ATransform> spTransform = GetTransform();

		Vector3 vPosition{ spTransform->GetPos() };
		SHPTR<ACell> newCell{};

		if (false == spNavigation->IsMove(vPosition, REF_OUT newCell))
		{
			Vector3 closestPoint = spNavigation->ClampPositionToCell(vPosition);
			spTransform->SetPos(Vector3(closestPoint.x, vPosition.y, closestPoint.z));
			vPosition = spTransform->GetPos();
		}

		_float newHeight = spNavigation->ComputeHeight(vPosition);
		spTransform->SetPos(Vector3(vPosition.x, newHeight, vPosition.z));
	}

	void APawn::Placement(_int _CellIndex)
	{
		SHPTR<ANavigation> spNavigation = m_spNavigation;
		SHPTR<ACell> spCell = spNavigation->FindCell(_CellIndex);
		GetTransform()->SetPos(spCell->GetCenterPos());
	}

	Vector3 APawn::BringCellIndextoPosition()
	{
		SHPTR<ANavigation> spNavigation = m_spNavigation;
		SHPTR<ACell> spCell = spNavigation->FindCell(GetTransform()->GetPos());
		if (nullptr == spCell)
		{
			return GetTransform()->GetPos();
		}
		SetCellIndex(spCell->GetIndex());
		return spCell->GetCenterPos();
	}

	bool APawn::IsHit(APawn* _pPawn, const _double& _dTimeDelta)
	{
		return false;
	}

	void APawn::HealHp(const _float _fHeal)
	{
		m_CharStatus.fHp += _fHeal;
		if (m_CharStatus.fHp >= m_CharStatus.fSaveHp)
			m_CharStatus.fHp = m_CharStatus.fSaveHp;
	}

	void APawn::DamageToEnemy(const _float _fDamge)
	{
		m_CharStatus.fHp -= _fDamge;
		if (m_CharStatus.fHp <= 0)
		{
			m_isDead = true;
		}
	}

	void APawn::Free()
	{
	}

}