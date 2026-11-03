#include "CCharacter.h"
#include "CAABBCollider.h"
#include "COBBCollider.h"
#include "CTransform.h"
#include "CGameInstance.h"

shared_ptr<CActor> CCharacter::Clone(const VOIDDATAS& _tDatas)
{
	return shared_ptr<CActor>();
}

void CCharacter::Free()
{
	Safe_Free(m_pDamagedCollider);
	Safe_Free(m_pFirstCheckCollider);
	__super::Free();
}

HRESULT CCharacter::NativeConstruct()
{
	return S_OK;
}

HRESULT CCharacter::NativeConstruct(VOIDDATAS _tDatas)
{
	if (FAILED(__super::NativeConstruct(_tDatas))) {
		return E_FAIL;
	}

	Set_Active(true);
	return S_OK;
}

HRESULT CCharacter::CreateComponent()
{
	if (FAILED(__super::CreateComponent())) {
		return E_FAIL;
	}
	CComponent::COMPDESC tDesc;
	tDesc.pActor = ThisShared<CPawn>();
	{
		CCollider::COLLDESC tCollDesc;
		tCollDesc.vPosition = Get_Transform()->Get_Pos();
		tCollDesc.vScale = Get_Transform()->Get_Scale();
		m_pDamagedCollider = CloneCompData< COBBCollider>(PROTO_COMP_OBBCOLLIDER, VOIDDATAS{ &tDesc, &tCollDesc });
		Add_Component(COM_COLLIDER, m_pDamagedCollider);
	}
	{
		CCollider::COLLDESC tCollDesc;
		tCollDesc.vPosition = Get_Transform()->Get_Pos();
		tCollDesc.vScale = Get_Transform()->Get_Scale();
		m_pFirstCheckCollider = CloneCompData< CAABBCollider>(PROTO_COMP_ABBCOLLIDER, VOIDDATAS{ &tDesc, &tCollDesc });
		Add_Component(COM_FIRSTCHECKCOLL, m_pFirstCheckCollider);
	}
	return S_OK;
}

HRESULT CCharacter::Tick(const _double& _dTimeDelta)
{
	return __super::Tick(_dTimeDelta);
}

HRESULT CCharacter::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
#ifdef _DEBUG
	m_pDamagedCollider->Add_RendererDebugActor(Get_Renderer());
	m_pFirstCheckCollider->Add_RendererDebugActor(Get_Renderer());
#endif
	return S_OK;
}

HRESULT CCharacter::Render(const HDC& _hDC)
{
	return __super::Render(_hDC);
}

_bool CCharacter::IsDamaged(const shared_ptr<CCollider>& _pAtkCollider, const shared_ptr<CCharacter>& _pChar)
{
	RETURN_CHECK(false == Is_Active(), false);
	RETURN_CHECK(nullptr == _pChar, false);
	RETURN_CHECK(false == _pChar->Is_Active(), false);
	RETURN_CHECK(nullptr == _pAtkCollider, false);
	RETURN_CHECK(false == m_pFirstCheckCollider->IsCollider(_pAtkCollider), false);
	RETURN_CHECK(false == m_pDamagedCollider->IsCollider(_pAtkCollider), false);

	Damaged(_pChar);
	return true;
}

_bool CCharacter::IsDamaged(const shared_ptr<CCollider>& _pFirstCheckCollider,
	const shared_ptr<CCollider>& _pAtkCollider, const shared_ptr<CCharacter>& _pChar)
{
	RETURN_CHECK(false == Is_Active(), false);
	RETURN_CHECK(nullptr == _pChar, false);
	RETURN_CHECK(false == _pChar->Is_Active(), false);
	RETURN_CHECK(nullptr == _pFirstCheckCollider, false);
	RETURN_CHECK(nullptr == _pAtkCollider, false);
	RETURN_CHECK(false == m_pFirstCheckCollider->IsCollider(_pFirstCheckCollider), false);
	RETURN_CHECK(false == m_pDamagedCollider->IsCollider(_pAtkCollider), false);

	Damaged(_pChar);
	return true;
}

void CCharacter::Damaged(const shared_ptr<CCharacter>& _pChar)
{
	DEBUG_MSG("Damaged Check");
}

void CCharacter::UpdateColliderScale()
{
	if (nullptr != Get_Transform()) {
		m_pDamagedCollider->Set_Scale(Get_Transform()->Get_Scale());
		_float3 vScale = Get_Transform()->Get_Scale();
		_float3 vSaveScale = vScale;
		vScale *= 1.6f;
		vScale.y = vSaveScale.y * 1.2f;
		m_pFirstCheckCollider->Set_Scale(vScale);
	}
}
