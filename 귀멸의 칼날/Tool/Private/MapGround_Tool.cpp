#include "MapGround_Tool.h"
#include "GameInstance.h"
#include "ImGuiClass_Manager_Tool.h"
#include "MapViewer_Tool.h"

CMapGround_Tool::CMapGround_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject_Tool(_pDevice, _pContext)
{
}

CMapGround_Tool::CMapGround_Tool(const CMapGround_Tool& _rhs)
	: CGameObject_Tool(_rhs)
{
}

CMapGround_Tool* CMapGround_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CMapGround_Tool* pInstance = new CMapGround_Tool(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CMapGround_Tool::Create To Failed");
	}
	return pInstance;
}

CGameObject* CMapGround_Tool::Clone(void* pArg)
{
	CMapGround_Tool* pInstance = new CMapGround_Tool(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CMapGround_Tool::Clone To Failed");
	}
	return pInstance;
}

HRESULT CMapGround_Tool::NativeConstruct_Prototype(void)
{
	return S_OK;
}

HRESULT CMapGround_Tool::NativeConstruct_Clone(void* pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CImGuiClass_Manager_Tool* pImGuiInstance = GET_INSTANCE(CImGuiClass_Manager_Tool);

	if (nullptr != pArg)
	{
		_tchar SkyName[MAX_PATH] = L"";
		lstrcpy(SkyName, reinterpret_cast<_tchar*>(pArg));
		if (!lstrcmp(SkyName, PROTO_COMP_MODEL_UROKODAKIIWA_SKY))
		{

			if (FAILED(__super::Add_Component(COM_MODEL, LEVEL_TOOL, PROTO_COMP_MODEL_UROKODAKIIWA_SKY, (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}
		else if (!lstrcmp(SkyName, PROTO_COMP_MODEL_STORY_SKY))
		{

			if (FAILED(__super::Add_Component(COM_MODEL, LEVEL_TOOL, PROTO_COMP_MODEL_STORY_SKY, (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}
		else if (!lstrcmp(SkyName, PROTO_COMP_MODEL_RATTAN_SKY))
		{

			if (FAILED(__super::Add_Component(COM_MODEL, LEVEL_TOOL, PROTO_COMP_MODEL_RATTAN_SKY, (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}
		m_iObjType = 1;
	}
	else
	{
		_uint i = 0;
		CImGuiClass_Tool* pMapViewer = pImGuiInstance->Get_ImGuiClass(IMGCLASS_MAPVIEWER);
		m_pModelCom = dynamic_cast<CModel*>(dynamic_cast<CMapViewer_Tool*>(pMapViewer)->Get_GroundModel()->Clone(&i));
		m_Components.emplace(COM_MODEL, m_pModelCom);
		Safe_AddRef(m_pModelCom);
		m_iObjType = 0;
	}

	if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXMODEL, (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// 이미지 바꾸기용 ==============================================================================
	PATHS Path = { L"Model", L"MapModel", L"MapGround", L"Textures" };
	m_pTextureCom = CTexture::Create(m_pDevice, m_pContext, &Path);
	lstrcpy(m_GroundName, TEXT("storyNewC04"));
	//================================================================================================
	Ready_Transform(COM_TRANSFORM);

	RELEASE_INSTANCE(CImGuiClass_Manager_Tool);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CMapGround_Tool::Tick(const _double& _dTimeDelta)
{
	// 이미지 바꾸기용 ======================================================================================
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_DIKeyPressing(DIK_N))
	{
		if (pGameInstance->Get_DIKeyDown(DIK_M))
		{
			Safe_Release(m_pTextureCom);
			//m_pTextureCom = nullptr;
			PATHS Path = { L"Model", L"MapModel", L"MapGround", L"Textures" };
			m_pTextureCom = CTexture::Create(m_pDevice, m_pContext, &Path);

			lstrcpy(m_GroundName, TEXT("storyNewC02"));
			m_pTextureCom->SetUp_ConstantTable_FileName(m_pShaderCom, "g_DiffuseTexture", m_GroundName);

			m_bImageChangeCheck = true;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
	//=========================================================================================================
}

void CMapGround_Tool::Late_Tick(const _double& _dTimeDelta)
{
	if (0 == m_iObjType)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	else if (1 == m_iObjType)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CMapGround_Tool::Render(void)
{
	if (nullptr == m_pModelCom || nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (0 == m_iObjType)
	{
		_uint iNumMeshContainers = m_pModelCom->Get_NumMeshContainers();
		for (_uint i = 0; i < iNumMeshContainers; ++i)
		{
			if (false == m_bImageChangeCheck)
			{
				if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE)))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pTextureCom->SetUp_ConstantTable_FileName(m_pShaderCom, "g_DiffuseTexture", m_GroundName)))
					return E_FAIL;
			}

			m_pModelCom->Render_NonAnim(i, m_pShaderCom, 0);
		}
	}
	else if (1 == m_iObjType)
	{
		_uint iNumMeshContainers = m_pModelCom->Get_NumMeshContainers();
		for (_uint i = 0; i < iNumMeshContainers; ++i)
		{
			if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE)))
				return E_FAIL;
			m_pModelCom->Render_NonAnim(i, m_pShaderCom, 6);
		}
	}

	return S_OK;
}

HRESULT CMapGround_Tool::SetUp_ConstantTable(void)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4 WorldMatrix = m_pTransformCom->Get_WorldFloat4x4_TP();
	_float4x4 ViewMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW);
	_float4x4 ProjMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ);
	_float camFar = pGameInstance->Get_CamFar();

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_CamFar", &camFar, sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

_bool CMapGround_Tool::Extern_HierakeyView(const string& _str)
{
	return true;
}

HRESULT CMapGround_Tool::Delete_Self(void)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (0 == m_iObjType)
		pGameInstance->Add_DeleteObject(LEVEL_TOOL, LAYER_MAPGROUND, this);
	else if (1 == m_iObjType)
		pGameInstance->Add_DeleteObject(LEVEL_TOOL, LAYER_MAPSKY, this);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CMapGround_Tool::Free(void)
{
	__super::Free();

	Safe_Release(m_pTextureCom);
}
