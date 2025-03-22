#include "CHalberd.h"
#include "CGameInstance.h"
#include "CCharacter.h"

CHalberd::CHalberd(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CWeapon(_pDevice, _pContext)
{
}

CHalberd::CHalberd(const CHalberd& _rhs)
	: CWeapon(_rhs)
{
}

HRESULT CHalberd::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CHalberd::NativeConstruct_Clone(void* pArg)
{
	if (nullptr != pArg)
	{
		m_tHalberdDesc = *static_cast<HALBERDDESC*>(pArg);
		_wstring str = m_tHalberdDesc.tItemDesc.strModelName;
		m_tItemDesc.strModelName = str;
		// Find HierackyNode
		Find_HierackyNode(m_tHalberdDesc.tEquipDesc.pOwnerObject, m_tHalberdDesc.tEquipDesc.strAttachNodeName);
	}

	if (FAILED(Ready_Component()))
		return E_FAIL;

	Setting_WeaponPivotPos(XMLoadFloat3(&m_tHalberdDesc.tWeaponDesc.v3PivotColliderPos),
		XMLoadFloat3(&m_tHalberdDesc.tWeaponDesc.v3PivotColliderSize));

	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CHalberd::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
}

void CHalberd::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

HRESULT CHalberd::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHalberd::Ready_Component()
{
	if (FAILED(__super::Ready_Component()))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(COM_MODEL, g_iCurrentLevel,
		m_tHalberdDesc.tItemDesc.strModelName.c_str(), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	{
		// CollCollider
		CCollider::COLLIDERDESC tCollDesc;
		tCollDesc.vScale = _float3(0.1f, 0.1f, 0.9f);
		tCollDesc.vTranslation = _float3(0.0f, 0.0f, 1.1f);
		/* For.Com_Shader */
		if (FAILED(__super::Add_Component(COM_COLLCOLLIDER, LEVEL_STATIC,
			PROTO_COMP_OBBCOLLIDER, (CComponent**)&m_pCollCollider, &tCollDesc)))
			return E_FAIL;
	}

	{
		// CollCollider
		CCollider::COLLIDERDESC tCollDesc;
		tCollDesc.vScale = _float3(1.f, 0.2f, 1.f);
		tCollDesc.vTranslation = _float3(0.0f, 0.0f, 1.1f);
		/* For.Com_Shader */
		if (FAILED(__super::Add_Component(COM_FIRSTCHECKCOLLIDER, LEVEL_STATIC,
			PROTO_COMP_SPHERECOLLIDER, (CComponent**)&m_pFirstCheckCollider, &tCollDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CHalberd::SetUp_ConstantTable()
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

_bool CHalberd::Extern_EditValue()
{
	return _bool();
}

_bool CHalberd::Extern_HierakeyView(const string& _str)
{
	return __super::Extern_HierakeyView(_str);
}

HRESULT CHalberd::Create_Self(CGameObject** _ppOut)
{
	return E_NOTIMPL;
}

HRESULT CHalberd::Delete_Self()
{
	return E_NOTIMPL;
}

HRESULT CHalberd::Save_Data()
{
	return E_NOTIMPL;
}

HRESULT CHalberd::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
	return E_NOTIMPL;
}

_bool CHalberd::Is_Picking(_float3* _pOut)
{
	return _bool();
}

void CHalberd::Reset_Data()
{
}

CHalberd* CHalberd::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CHalberd* pInstance = new CHalberd(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CHalberd::Create To Failed");
	}
	return pInstance;
}

CGameObject* CHalberd::Clone(void* pArg)
{
	CHalberd* pInstance = new CHalberd(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CHalberd::Clone To Failed");
	}
	return pInstance;
}

void CHalberd::Free()
{
	__super::Free();
}
