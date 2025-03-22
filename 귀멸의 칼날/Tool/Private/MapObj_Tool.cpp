#include "MapObj_Tool.h"
#include "GameInstance.h"
#include "ImGuiClass_Manager_Tool.h"
#include "MapViewer_Tool.h"

CMapObj_Tool::CMapObj_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject_Tool(_pDevice, _pContext)
{
}

CMapObj_Tool::CMapObj_Tool(const CMapObj_Tool& _rhs)
	: CGameObject_Tool(_rhs)
{
}

CMapObj_Tool* CMapObj_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CMapObj_Tool* pInstance = new CMapObj_Tool(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CMapObj01_Tool::Create To Failed");
	}
	return pInstance;
}

CGameObject* CMapObj_Tool::Clone(void* pArg)
{
	CMapObj_Tool* pInstance = new CMapObj_Tool(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CMapObj01_Tool::Clone To Failed");
	}
	return pInstance;
}

HRESULT CMapObj_Tool::NativeConstruct_Prototype(void)
{
	return S_OK;
}

HRESULT CMapObj_Tool::NativeConstruct_Clone(void* pArg)
{
	// 컴포넌트 세팅 
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CImGuiClass_Manager_Tool* pImGuiInstance = GET_INSTANCE(CImGuiClass_Manager_Tool);

	////// 모델가져오기 
	//CImGuiClass_Tool* pMapViewer = pImGuiInstance->Get_ImGuiClass(IMGCLASS_MAPVIEWER);
	//if (FAILED(__super::Add_Component(dynamic_cast<CMapViewer_Tool*>(pMapViewer)->Get_SelectModel()->Get_FileData()->strFileName.c_str(), LEVEL_TOOL, dynamic_cast<CMapViewer_Tool*>(pMapViewer)->Get_SelectModel()->Get_FileData()->strFileName.c_str(), (CComponent**)&m_pShaderCom)))
	//	return E_FAIL;

	// 모델가져오기 
	_uint i = 0;
	CImGuiClass_Tool* pMapViewer = pImGuiInstance->Get_ImGuiClass(IMGCLASS_MAPVIEWER);
	m_pModelCom = dynamic_cast<CModel*>(dynamic_cast<CMapViewer_Tool*>(pMapViewer)->Get_SelectModel()->Clone(&i));
	m_Components.emplace(COM_MODEL, m_pModelCom);
	Safe_AddRef(m_pModelCom);
	// 랜더러
	if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	// 셰이더
	if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXMODEL, (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	Ready_Transform(COM_TRANSFORM);

	if (nullptr != pArg)
	{
		_float3 vPos;
		memcpy(&vPos, pArg, sizeof(_float3));
		_vector fPosition = XMLoadFloat3(&vPos);
		m_pTransformCom->Set_Pos(fPosition);
	}
	else
	{
		_vector vPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		m_pTransformCom->Set_Pos(vPos);
	}

	// 초기 회전값 세팅 

	RELEASE_INSTANCE(CImGuiClass_Manager_Tool);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CMapObj_Tool::Tick(const _double& _dTimeDelta)
{
}

void CMapObj_Tool::Late_Tick(const _double& _dTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CMapObj_Tool::Render(void)
{
	if (nullptr == m_pModelCom || nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint iNumMeshContainers = m_pModelCom->Get_NumMeshContainers();
	for (_uint i = 0; i < iNumMeshContainers; ++i)
	{
		if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_NormalTexture", i, TextureType_NORMALS)))
			return E_FAIL;
		m_pModelCom->Render_NonAnim(i, m_pShaderCom, 2);
	}

	return S_OK;
}

HRESULT CMapObj_Tool::SetUp_ConstantTable(void)
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

HRESULT CMapObj_Tool::Delete_Self(void)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Add_DeleteObject(LEVEL_TOOL, m_pModelCom->Get_FileData()->strFileName.c_str(), this);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CMapObj_Tool::Set_RotationXYZ(_float3 _fXYZ)
{
	m_tGobDesc.v3Rotate.x = _fXYZ.x;
	m_tGobDesc.v3Rotate.y = _fXYZ.y;
	m_tGobDesc.v3Rotate.z = _fXYZ.z;
}

void CMapObj_Tool::Free(void)
{
	__super::Free();
}
