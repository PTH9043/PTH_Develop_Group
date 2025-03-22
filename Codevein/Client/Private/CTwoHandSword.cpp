#include "CMainView.h"
#include "CImGui_Manager.h"
#include "CTwoHandSword.h"
#include "CGameInstance.h"

CTwoHandSword::CTwoHandSword(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CWeapon(_pDevice, _pContext)
{
	m_eWeaponType = WEAPON_TWOHAND;
}

CTwoHandSword::CTwoHandSword(const CTwoHandSword& _rhs)
	: CWeapon(_rhs)
{
}

HRESULT CTwoHandSword::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTwoHandSword::NativeConstruct_Clone(void* pArg)
{
	if (nullptr != pArg)
	{
		m_tTwoHandSwordDesc = *static_cast<TWOHANDSWORDDESC*>(pArg);
		m_tItemDesc.strModelName = m_tTwoHandSwordDesc.tItemDesc.strModelName;;
		// Find HierackyNode
		Find_HierackyNode(m_tTwoHandSwordDesc.tEquipDesc.pOwnerObject,
		m_tTwoHandSwordDesc.tEquipDesc.strAttachNodeName);
	}

	if (FAILED(Ready_Component()))
		return E_FAIL;

	Setting_WeaponPivotPos(XMLoadFloat3(&m_tTwoHandSwordDesc.tWeaponDesc.v3PivotColliderPos),
		XMLoadFloat3(&m_tTwoHandSwordDesc.tWeaponDesc.v3PivotColliderSize));

	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CTwoHandSword::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
}

void CTwoHandSword::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

HRESULT CTwoHandSword::Render()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint iNumMeshContainers = m_pModelCom->Get_NumMeshContainers();

	for (_uint i = 0; i < iNumMeshContainers; ++i)
	{
		if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TEXTYPE::TextureType_DIFFUSE)))
			return E_FAIL;

		//if (FAILED(m_pVIBufferCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
		//	return E_FAIL;
		//if (FAILED(m_pVIBufferCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
		//	return E_FAIL;

		m_pModelCom->Render_NonAnim(i, m_pShaderCom, 1);
	}

	return S_OK;
}

HRESULT CTwoHandSword::Ready_Component()
{
	if (FAILED(__super::Ready_Component()))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(COM_MODEL, g_iCurrentLevel,
		m_tTwoHandSwordDesc.tItemDesc.strModelName.c_str(), (CComponent**)&m_pModelCom)))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CTwoHandSword::SetUp_ConstantTable()
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

#pragma region USE_TOOL_METHOD
_bool CTwoHandSword::Extern_EditValue()
{
	return _bool();
}

_bool CTwoHandSword::Extern_HierakeyView(const string& _str)
{
	return __super::Extern_HierakeyView(_str);
}

HRESULT CTwoHandSword::Create_Self(CGameObject** _ppOut)
{
	return E_NOTIMPL;
}

HRESULT CTwoHandSword::Delete_Self()
{
	return E_NOTIMPL;
}

HRESULT CTwoHandSword::Save_Data()
{
	return E_NOTIMPL;
}

HRESULT CTwoHandSword::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
	return E_NOTIMPL;
}

_bool CTwoHandSword::Is_Picking(_float3* _pOut)
{
	return _bool();
}

void CTwoHandSword::Reset_Data()
{
}
#pragma endregion USE_TOOL_METHOD
CTwoHandSword* CTwoHandSword::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CTwoHandSword* pInstance = new CTwoHandSword(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CTwoHandSword::Create To Failed");
	}
	return pInstance;
}

CGameObject* CTwoHandSword::Clone(void* pArg)
{
	CTwoHandSword* pInstance = new CTwoHandSword(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CTwoHandSword::Clone To Failed");
	}
	return pInstance;
}

void CTwoHandSword::Free()
{
	__super::Free();
}
