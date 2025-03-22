#include "../Public/MapLight.h"
#include "GameInstance.h"

CMapLight::CMapLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPawn(pDevice, pContext)
{
}

CMapLight::CMapLight(const CMapLight& _rhs)
	: CPawn(_rhs)
{
}

CMapLight* CMapLight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapLight* pInstance = new CMapLight(pDevice, pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CMapLight::Create To Failed");
#endif
	}
	return pInstance;
}

CGameObject* CMapLight::Clone(void* _pArg)
{
	CMapLight* pInstance = new CMapLight(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CMapLight::Clone To Failed");
#endif
	}
	return pInstance;
}

HRESULT CMapLight::NativeConstruct_Prototype(void)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapLight::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		memcpy(&m_tMapLight, pArg, sizeof(MAPLIGHTDESC));
	}

	return S_OK;
}

void CMapLight::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);

	m_pTransformCom->Set_Scale(m_tMapLight.vScale);
	//m_pTransformCom->Rotate_TurnToNotFix(_float3(0.f, (_float)_dTimeDelta * 4.f, 0.f));
	m_pTransformCom->Set_PosF3(m_tMapLight.vPos);
	m_pTransformCom->Rotate_Fix(XMLoadFloat3(&m_tMapLight.vRot), m_tMapLight.fAngle);
}

void CMapLight::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}
}

HRESULT CMapLight::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (nullptr != m_pModelCom)
	{
		_uint iNumMeshContainers = m_pModelCom->Get_NumMeshContainers();

		for (_uint i = 0; i < iNumMeshContainers; ++i)
		{
			if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE)))
				return E_FAIL;

			m_pModelCom->Render_NonAnim(i, m_pShaderCom, 9);
		}
	}

	return S_OK;
}

void CMapLight::Extern_ImGuiClass(void)
{
	ImGui::DragFloat3("Scale", &m_tMapLight.vScale.x);
	ImGui::DragFloat3("Pos", &m_tMapLight.vPos.x);
	ImGui::DragFloat3("Rot", &m_tMapLight.vRot.x);
	ImGui::DragFloat("Angle", &m_tMapLight.fAngle);
}

HRESULT CMapLight::Ready_Component(void)
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXMODEL, (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(COM_MODEL, g_iCurrentLevel, PROTO_COMP_MODEL_MAPLIGHT, (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapLight::SetUp_ConstantTable(void)
{
	if (Is_Nullptr(m_pShaderCom) || Is_Nullptr(m_pTransformCom))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		_float4x4 WorldMatrix, ViewMatrix, ProjMatrix;
		WorldMatrix = m_pTransformCom->Get_WorldFloat4x4_TP();
		ViewMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW);
		ProjMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ);
		_float camFar = pGameInstance->Get_CamFar();

		if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Set_RawValue("g_CamFar", &camFar, sizeof(_float))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CMapLight::Free(void)
{
	__super::Free();
}
