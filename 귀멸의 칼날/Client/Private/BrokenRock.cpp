#include "BrokenRock.h"
#include "GameInstance.h"
#include "Battle_Manager.h"

CBrokenRock::CBrokenRock(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CItem(_pDevice, _pContext)
{
}

CBrokenRock::CBrokenRock(const CItem& _rhs)
	: CItem(_rhs)
{
}

HRESULT CBrokenRock::NativeConstruct_Prototype(void)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_eItemType = ITEM_WORLD;
	return S_OK;
}

HRESULT CBrokenRock::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		_matrix WorldMat = *reinterpret_cast<_fmatrix*>(pArg);
		m_pTransformCom->Bind_WorldMatrix(WorldMat);
	}

	//m_pTransformCom->Rotate_Fix(m_pTransformCom->Get_Up(), 90.f);

	//m_bRenderCheck = true;

	_matrix mat = XMMatrixRotationY(XMConvertToRadians(15.f)) * XMMatrixTranslation(-11.f, 0.f, -37.f);
	m_pTransformCom->Bind_WorldMatrix(mat);

	return S_OK;
}

void CBrokenRock::Tick(const _double& _dTimeDelta)
{
	CBattle_Manager* pInstance = GET_INSTANCE(CBattle_Manager);
	{
		if (false == m_bRenderCheck && pInstance->Get_BattleInfo().bBattleEnd)
			m_bRenderCheck = true;
	}
	RELEASE_INSTANCE(CBattle_Manager);

	//__super::Tick(_dTimeDelta);
}

void CBrokenRock::Late_Tick(const _double& _dTimeDelta)
{
	if (false == m_bRenderCheck)
		return;
		
	__super::Late_Tick(_dTimeDelta);
}

HRESULT CBrokenRock::Render(void)
{
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (nullptr != m_pModelCom)
	{
		_uint iNumMeshContainers = m_pModelCom->Get_NumMeshContainers();

		for (_uint i = 0; i < iNumMeshContainers; ++i)
		{
			if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE)))
				return E_FAIL;
			if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_NormalTexture", i, TextureType_NORMALS)))
				return E_FAIL;

			m_pModelCom->Render_NonAnim(i, m_pShaderCom, 2);
		}
	}

	return S_OK;
}

HRESULT CBrokenRock::RenderLight(void)
{
	if (FAILED(__super::RenderLight()))
		return E_FAIL;

	return S_OK;
}

void CBrokenRock::Extern_ImGuiClass()
{
	/*ImGui::DragFloat3("Pos", &vPos.x);
	
	ImGui::DragFloat("RotY", &fRotY);


	_matrix mat = XMMatrixRotationY(XMConvertToRadians(fRotY)) * XMMatrixTranslation(vPos.x, vPos.y, vPos.z);
	m_pTransformCom->Bind_WorldMatrix(mat);*/
}

HRESULT CBrokenRock::SetUp_ConstantTable(void)
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBrokenRock::SetUp_Components(void)
{
	if (FAILED(__super::SetUp_Components()))
		return E_FAIL;

	// ¸ðµ¨ ¼¼ÆÃ 
	if (FAILED(Add_Component(COM_MODEL, g_iCurrentLevel,
		PROTO_COMP_MODEL_BROKENROCK, (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBrokenRock::SetUp_LoadData(void* _pArg)
{
	return S_OK;
}

HRESULT CBrokenRock::Delete_Self(void)
{
	return S_OK;
}

CBrokenRock* CBrokenRock::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CBrokenRock* pInstance = new CBrokenRock(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CBrokenRock::Create To Failed");
#endif
	}
	return pInstance;
}

CGameObject* CBrokenRock::Clone(void* pArg)
{
	CBrokenRock* pInstance = new CBrokenRock(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CBrokenRock::Clone To Failed");
#endif
	}
	return pInstance;
}

void CBrokenRock::Free(void)
{
	__super::Free();
}
