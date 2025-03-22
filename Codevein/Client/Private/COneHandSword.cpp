#include "CMainView.h"
#include "CImGui_Manager.h"
#include "COneHandSword.h"
#include "CGameInstance.h"

COneHandSword::COneHandSword(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CWeapon(_pDevice, _pContext)
{
	m_eWeaponType = WEAPON_ONEHAND;
}

COneHandSword::COneHandSword(const COneHandSword& _rhs)
	: CWeapon(_rhs)
{
}

HRESULT COneHandSword::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT COneHandSword::NativeConstruct_Clone(void* pArg)
{

	if (nullptr != pArg)
	{
		m_tOneHandSwordDesc = *static_cast<ONEHANDSWRODDESC*>(pArg);
		_wstring str = m_tOneHandSwordDesc.tItemDesc.strModelName;
		m_tItemDesc.strModelName = str;
		// Find HierackyNode
		Find_HierackyNode(m_tOneHandSwordDesc.tEquipDesc.pOwnerObject, m_tOneHandSwordDesc.tEquipDesc.strAttachNodeName);
	}

	if (FAILED(Ready_Component()))
		return E_FAIL;

	Setting_WeaponPivotPos(XMLoadFloat3(&m_tOneHandSwordDesc.tWeaponDesc.v3PivotColliderPos), 
		XMLoadFloat3(&m_tOneHandSwordDesc.tWeaponDesc.v3PivotColliderSize));

	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void COneHandSword::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
}

void COneHandSword::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

HRESULT COneHandSword::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT COneHandSword::Ready_Component()
{
	if (FAILED(__super::Ready_Component()))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(COM_MODEL, g_iCurrentLevel, 
		m_tOneHandSwordDesc.tItemDesc.strModelName.c_str(), (CComponent**)&m_pModelCom)))
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

HRESULT COneHandSword::SetUp_ConstantTable()
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}
#pragma region USE_TOOL_METHOD
_bool COneHandSword::Extern_EditValue()
{
	return _bool();
}

_bool COneHandSword::Extern_HierakeyView(const string& _str)
{
	return __super::Extern_HierakeyView(_str);
}

HRESULT COneHandSword::Create_Self(CGameObject** _ppOut)
{
	return E_NOTIMPL;
}

HRESULT COneHandSword::Delete_Self()
{
	return E_NOTIMPL;
}

HRESULT COneHandSword::Save_Data()
{
	return E_NOTIMPL;
}

HRESULT COneHandSword::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
	return E_NOTIMPL;
}

_bool COneHandSword::Is_Picking(_float3* _pOut)
{
	return _bool();
}

void COneHandSword::Reset_Data()
{
}
#pragma endregion USE_TOOL_METHOD

COneHandSword* COneHandSword::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	COneHandSword* pInstance = new COneHandSword(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("COneHandSword::Create To Failed");
	}
	return pInstance;
}

CGameObject* COneHandSword::Clone(void* pArg)
{
	COneHandSword* pInstance = new COneHandSword(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("COneHandSword::Clone To Failed");
	}
	return pInstance;
}

void COneHandSword::Free()
{
	__super::Free();
}
