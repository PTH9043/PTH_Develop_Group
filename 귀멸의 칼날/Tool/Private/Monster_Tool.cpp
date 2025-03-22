#include "Monster_Tool.h"
#include "GameInstance.h"
#include "ImGuiClass_Manager_Tool.h"
#include "MapViewer_Tool.h"

CMonster_Tool::CMonster_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject_Tool(_pDevice, _pContext)
{
}

CMonster_Tool::CMonster_Tool(const CMonster_Tool& _rhs)
	: CGameObject_Tool(_rhs)
{
}

CMonster_Tool* CMonster_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CMonster_Tool* pInstance = new CMonster_Tool(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CMonster_Tool::Create To Failed");
	}
	return pInstance;
}

CGameObject* CMonster_Tool::Clone(void* pArg)
{
	CMonster_Tool* pInstance = new CMonster_Tool(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CMonster_Tool::Clone To Failed");
	}
	return pInstance;
}

HRESULT CMonster_Tool::NativeConstruct_Prototype(void)
{
	return S_OK;
}

HRESULT CMonster_Tool::NativeConstruct_Clone(void* pArg)
{
	// 컴포넌트 세팅 
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CImGuiClass_Manager_Tool* pImGuiInstance = GET_INSTANCE(CImGuiClass_Manager_Tool);

	// 모델가져오기 
	_uint i = 0;
	CImGuiClass_Tool* pMapViewer = pImGuiInstance->Get_ImGuiClass(IMGCLASS_MAPVIEWER);
	m_pModelCom = dynamic_cast<CModel*>(dynamic_cast<CMapViewer_Tool*>(pMapViewer)->Get_MonsterModel()->Clone(&i));
	m_Components.emplace(COM_MODEL, m_pModelCom);
	Safe_AddRef(m_pModelCom);
	// 랜더러
	if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	// 셰이더
	//if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXANIMMODEL, (CComponent**)&m_pShaderCom)))
	//	return E_FAIL;
	if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXMODEL, (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	// 콜라이더
	CCollider::COLLIDERDESC collider;
	collider.vScale = _float3{ 1.f,1.f,1.f };
	collider.vTranslation = _float3{ 0.f, 1.f, 0.f };
	if (FAILED(__super::Add_Component(COM_COLLCOLLIDER, LEVEL_STATIC, PROTO_COMP_AABBCOLLIDER, (CComponent**)&m_pColliderCom, &collider)))
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

	RELEASE_INSTANCE(CImGuiClass_Manager_Tool);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CMonster_Tool::Tick(const _double& _dTimeDelta)
{
	//m_pModelCom->Update_Animation(0.0);
	m_pTransformCom->Set_Scale(_float3{ 10.f, 10.f, 10.f });
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CMonster_Tool::Late_Tick(const _double& _dTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CMonster_Tool::Render(void)
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
		
		//m_pModelCom->Render(i, m_pShaderCom, 0);
		m_pModelCom->Render_NonAnim(i, m_pShaderCom, 2);
	}

//#ifdef _DEBUG // _DEBUG
//	m_pColliderCom->Render_Debug();
//#endif

	return S_OK;
}

HRESULT CMonster_Tool::SetUp_ConstantTable(void)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4 WorldMatrix = m_pTransformCom->Get_WorldFloat4x4_TP();
	_float4x4 ViewMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW);
	_float4x4 ProjMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CMonster_Tool::Delete_Self(void)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Add_DeleteObject(LEVEL_TOOL, m_pModelCom->Get_FileData()->strFileName.c_str(), this);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CMonster_Tool::Free(void)
{
	__super::Free();
}
