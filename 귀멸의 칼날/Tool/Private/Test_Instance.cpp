#include "Test_Instance.h"
#include "MainView_Tool.h"
#include "GameInstance.h"


CTest_Instance::CTest_Instance(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CPawn_Tool(_pDevice, _pContext)
{}

CTest_Instance::CTest_Instance(const CTest_Instance& _rhs)	: CPawn_Tool(_rhs)
{}

HRESULT CTest_Instance::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CTest_Instance::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CTest_Instance::Tick(const _double& _dTimeDelta)
{
	if (nullptr != m_pModelInstance)
	{
		m_pModelInstance->Update_Animation(_dTimeDelta);
	}
}

void CTest_Instance::Late_Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	__super::Late_Tick(_dTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CTest_Instance::Render()
{
	if (nullptr == m_pModelInstance)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint iNumMeshContainers = m_pModelInstance->Get_NumMeshContainers();

	for (_uint i = 0; i < iNumMeshContainers; ++i)
	{
		if (FAILED(m_pModelInstance->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE)))
			return E_FAIL;

		m_pModelInstance->Render( i, m_pShaderCom, 0);
	}
	return S_OK;
}

HRESULT CTest_Instance::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4 WorldMatrixTP = m_pTransformCom->Get_WorldFloat4x4_TP();
	_float4x4 ViewMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW);
	_float4x4 ProjMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrixTP, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CTest_Instance::Ready_Components()
{
	if (FAILED(Ready_Transform(COM_TRANSFORM)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, 
		PROTO_COMP_RENDERER, (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, 
		PROTO_COMP_SHADER_VTXANIMMODEL_INSTANCE, (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	CModel_Instance::MODEL_INSTNACE_DESC tDesc;
	tDesc.iModelInstanceCount = 150;
	tDesc.iGroupNumber = 5;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(COM_MODEL, LEVEL_TOOL,
		L"ProtoTest", (CComponent**)&m_pModelInstance, &tDesc)))
		return E_FAIL;


	for (_uint i = 0; i < 5; ++i)
	{
		TRANSFORMVECTOR Vector;
		for (_uint j = 0; j < 30; ++j)
		{
			Vector.push_back(CTransform::Create(m_pDevice, m_pContext));
		}
		m_TransformVector.push_back(Vector);
	}


	//TRANSFORMVECTOR  vTransform;
	//{
	//	for (auto& iter : m_TransformVector[0])
	//		iter->Set_Pos(_float3(rand() % 100, 0, rand() % 100). Get_Vector());

	//	m_pModelInstance->Update_Value(m_TransformVector[0], 0);
	//}
	//{
	//	for (auto& iter : m_TransformVector[1])
	//		iter->Set_Pos(_float3(rand() % 100, 10, rand() % 100).Get_Vector());

	//	m_pModelInstance->Update_Value(m_TransformVector[1], 1);
	//}
	//{
	//	for (auto& iter : m_TransformVector[2])
	//		iter->Set_Pos(_float3(rand() % 100, 20, rand() % 100).Get_Vector());

	//	m_pModelInstance->Update_Value(m_TransformVector[2], 2);
	//}
	//{
	//	for (auto& iter : m_TransformVector[3])
	//		iter->Set_Pos(_float3(rand() % 100, 30, rand() % 100).Get_Vector());

	//	m_pModelInstance->Update_Value(m_TransformVector[3], 3);
	//}
	//{
	//	for (auto& iter : m_TransformVector[4])
	//		iter->Set_Pos(_float3(rand() % 100, 40, rand() % 100).Get_Vector());

	//	m_pModelInstance->Update_Value(m_TransformVector[4], 4);
	//}
	//m_pModelInstance->Set_Animation(0, 30);
	//m_pModelInstance->Set_Animation(1, 12);
	//m_pModelInstance->Set_Animation(2, 40);
	//m_pModelInstance->Set_Animation(3, 60);
	//m_pModelInstance->Set_Animation(5, 80);
	return S_OK;
}

CTest_Instance* CTest_Instance::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CTest_Instance* pInstance = new CTest_Instance(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
#ifdef _DEBUG
		MSG_BOX("CTest_Instance::Create To Failed");
#endif
	}
	return pInstance;
}

CGameObject* CTest_Instance::Clone(void* pArg)
{
	CTest_Instance* pInstance = new CTest_Instance(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
#ifdef _DEBUG
		MSG_BOX("CTest_Instance::Clone To Failed");
#endif
	}
	return pInstance;
}

void CTest_Instance::Free()
{
	for (auto& iter : m_TransformVector)
		for (auto& value : iter)
			Safe_Release(value);

	Safe_Release(m_pModelInstance);
	__super::Free();
}
