#include "CoreDefines.h"
#include "AGameObject.h"
#include "ACollider.h"
#include "ATransform.h"
#include "ASpace.h"
#include "ACoreInstance.h"
#include "ANavigation.h"
#include "ACell.h"

namespace Core
{
	AGameObject::AGameObject(OBJCON_CONSTRUCTOR, SESSIONID _ID, SESSIONTYPE _SessionType) : 
		ACoreObject(OBJCON_CONDATA), m_SessionID{_ID}, m_SessionType{_SessionType}, m_SpaceIndex{0},
		m_spTransform{ nullptr }, m_CellIndex{0}, m_fMoveSpeed{0.f}, m_fRunSpeed{0.f}
		, m_isActive{false}, m_isPermanentDisable{false}, m_CopyHead{}
	{
		MemoryInitialization(m_CopyBuffer.data(), MAX_BUFFER_LENGTH);
	}

	_bool AGameObject::Start(const VOIDDATAS& _ReceiveDatas)
	{
		m_spTransform = Create<ATransform>();
		return true;
	}

	void AGameObject::RunningPermanentDisableSituation()
	{
	}

	void AGameObject::InsertColliderContainer(const COLLIDERINFO& _ColliderInfo)
	{
		m_ColliderContainer.push_back(Create<ACollider>((ACollider::TYPE)_ColliderInfo.iColliderType,
			ACollider::COLLIDERDESC{ _ColliderInfo.vPos, _ColliderInfo.vScale }));
	}

	void AGameObject::BringSpaceIndex(SHPTR<ASpace> _spSpace)
	{
		RETURN_CHECK(nullptr == _spSpace, ;);
		m_SpaceIndex = _spSpace->GetSpaceIndex();
	}

	_float AGameObject::OtherCharacterToDistance(SHPTR<ATransform> _spOtherTransform)
	{
		SHPTR<ATransform> spTransform = GetTransform();
		assert(nullptr != spTransform && nullptr != _spOtherTransform);

		_float fDistance = 0.f;

		fDistance = DirectX::XMVectorGetX(DirectX::XMVector3Length(spTransform->GetPos()
			- _spOtherTransform->GetPos()));

		return fDistance;
	}

	_float AGameObject::OtherCharacterDirToLook(SHPTR<ATransform> _spOtherTransform)
	{
		SHPTR<ATransform> spTransform = GetTransform();
		assert(nullptr != _spOtherTransform && nullptr != spTransform);

		Vector3 v3Look;
		v3Look = (_spOtherTransform->GetPos() - spTransform->GetPos());
		return v3Look.Dot(spTransform->GetLook());
	}

	_float AGameObject::OhterCharacterDirToRight(SHPTR<ATransform> _spOtherTransform)
	{
		SHPTR<ATransform> spTransform = GetTransform();
		assert(nullptr != _spOtherTransform && nullptr != spTransform);

		Vector3 v3Look;
		v3Look = (_spOtherTransform->GetPos() - spTransform->GetPos());
		return v3Look.Dot(spTransform->GetRight());
	}

	_float AGameObject::OtherCharacterDirToLookConverter(SHPTR<ATransform> _spOtherTransform)
	{
		_float fDot = DirectX::XMConvertToDegrees(acosf(OtherCharacterDirToLook(_spOtherTransform)));
		_float fValue = OhterCharacterDirToRight(_spOtherTransform);

		if (fValue < 0)
			fDot = -fDot;

		return fDot;
	}

	Vector3 AGameObject::OtherCharacterDirToLookVectorF3(SHPTR<ATransform>_spOtherTransform)
	{
		Vector3 vDirection = Vector3(0.f, 0.f, 0.f);
		assert(nullptr != _spOtherTransform);
		vDirection = _spOtherTransform->GetPos() - GetTransform()->GetPos();
		vDirection.Normalize();
		return vDirection;
	}

	_bool AGameObject::IsCanSee(Vector3 _OtherPos)
	{
		SHPTR<ATransform> spTransform = GetTransform();
		Vector3 vPos = spTransform->GetPos();
		vPos.y = 0.f;
		_OtherPos.y = 0.f;
		return Vector3::DistanceSquared(vPos, _OtherPos) < SEE_RANGE;
	}

	_bool AGameObject::IsCanSee(SHPTR<ATransform> _spTransform)
	{
		assert(nullptr != _spTransform);
		return IsCanSee(_spTransform->GetPos());
	}

	const _bool AGameObject::IsPermanentDisable() const
	{
		return m_isPermanentDisable;
	}

	void AGameObject::SetActive(const _bool _isActive)
	{
		while (true)
		{
			_bool value = m_isActive.load();

			if (true == CAS_VALUE(m_isActive, value, _isActive))
			{
				if (true == m_isActive)
				{
					CallActiveEnable();
				}
				else
				{
					CallActiveDisable();
				}
				break;
			}
		}
	}

	void AGameObject::ActivePermanentDisable()
	{
		while (true)
		{
			_bool value = m_isActive.load();

			if (true == CAS_VALUE(m_isPermanentDisable, value, true))
			{
				break;
			}
		}
		LastBehavior();
	}

	void AGameObject::LastBehavior()
	{
	}

	void AGameObject::CallActiveEnable()
	{
	}

	void AGameObject::CallActiveDisable()
	{
	}

	void AGameObject::RestrictPositionToNavi()
	{
	}

	void AGameObject::Free()
	{
	}
}