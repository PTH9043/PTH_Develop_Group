#include "MapGroupObject.h"
#include "GameInstance.h"
#include "MeshContainer_Instance.h"
#include "Battle_Manager.h"
#include "CutScene_Manager.h"

CMapGroupObject::CMapGroupObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPawn(pDevice, pContext)
{
}

CMapGroupObject::CMapGroupObject(const CMapGroupObject& _rhs)
	: CPawn(_rhs)
{
}

CMapGroupObject* CMapGroupObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapGroupObject* pInstance = new CMapGroupObject(pDevice, pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CMapGroupObject::Create To Failed");
#endif
	}
	return pInstance;
}

CGameObject* CMapGroupObject::Clone(void* _pArg)
{
	CMapGroupObject* pInstance = new CMapGroupObject(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CMapGroupObject::Clone To Failed");
#endif
	}
	return pInstance;
}

HRESULT CMapGroupObject::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapGroupObject::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		CModel_Instance::tagModelInstanceDesc desc;
		desc.iModelInstanceCount = reinterpret_cast<MAPOBJDESC*>(pArg)->InstanceCount;
		desc.iGroupNumber = 1;
		if (LEVEL_GAMEPLAY == g_iCurrentLevel)
		{
			if (FAILED(__super::Add_Component(COM_INSTANCEMODEL, LEVEL_GAMEPLAY,
				reinterpret_cast<MAPOBJDESC*>(pArg)->ModelName, (CComponent**)&m_pModelInstanceCom, &desc)))
				return E_FAIL;
		}
		else if (LEVEL_STORY == g_iCurrentLevel)
		{
			if (FAILED(__super::Add_Component(COM_INSTANCEMODEL, LEVEL_STORY,
				reinterpret_cast<MAPOBJDESC*>(pArg)->ModelName, (CComponent**)&m_pModelInstanceCom, &desc)))
				return E_FAIL;
		}
		else if (LEVEL_BATTLE == g_iCurrentLevel)
		{
			if (FAILED(__super::Add_Component(COM_INSTANCEMODEL, LEVEL_BATTLE,
				reinterpret_cast<MAPOBJDESC*>(pArg)->ModelName, (CComponent**)&m_pModelInstanceCom, &desc)))
				return E_FAIL;
		}
		else if (LEVEL_MINIGAME == g_iCurrentLevel)
		{
			if (FAILED(__super::Add_Component(COM_INSTANCEMODEL, LEVEL_MINIGAME,
				reinterpret_cast<MAPOBJDESC*>(pArg)->ModelName, (CComponent**)&m_pModelInstanceCom, &desc)))
				return E_FAIL;
		}
		else if (LEVEL_RATTAN == g_iCurrentLevel)
		{
			if (FAILED(__super::Add_Component(COM_INSTANCEMODEL, LEVEL_RATTAN,
				reinterpret_cast<MAPOBJDESC*>(pArg)->ModelName, (CComponent**)&m_pModelInstanceCom, &desc)))
				return E_FAIL;
		}
		m_pModelInstanceCom->Update_Value(reinterpret_cast<MAPOBJDESC*>(pArg)->Matrix, 0);

		m_ObjDesc.ColliderSize = reinterpret_cast<MAPOBJDESC*>(pArg)->ColliderSize;
		m_ObjDesc.InstanceCount = reinterpret_cast<MAPOBJDESC*>(pArg)->InstanceCount;
	}

	m_bIsFilter = false;
	if (LEVEL_STORY == g_iCurrentLevel)
		m_bIsFilter = true;

	if (!lstrcmp(reinterpret_cast<MAPOBJDESC*>(pArg)->ModelName, L"SM_a_001_CliffJump_01a"))
		m_bIsFilter = false;


	//CCollider::COLLIDERDESC ColliderDesc;
	//// 콜라이더 만들기 
	//for (_uint i = 0; i < m_ObjDesc.InstanceCount; ++i)
	//{
	//	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	//	ColliderDesc.vScale = m_ObjDesc.ColliderSize;
	//	ColliderDesc.vTranslation = _float3{ 0.f,  m_ObjDesc.ColliderSize.y, 0.f };
	//	CComponent* OBBCollider = CGameInstance::Get_Instance()->Clone_Component(LEVEL_STATIC, PROTO_COMP_OBBCOLLIDER, &ColliderDesc);

	//	_tchar* ColliderName = new _tchar[MAX_PATH];
	//	wsprintf(ColliderName, L"Com_Collider_%d", i);
	//	m_vecColliderName.push_back(ColliderName);

	//	m_Components.emplace(m_vecColliderName[i], OBBCollider);
	//	Safe_AddRef(OBBCollider);
	//	m_vecCollider.push_back(static_cast<CCollider*>(OBBCollider));
	//}

	//m_ObjDesc.Matrix = reinterpret_cast<MAPOBJDESC*>(pArg)->Matrix;

	return S_OK;
}

void CMapGroupObject::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);

	//for (_uint i = 0; i < m_ObjDesc.InstanceCount; ++i)
	//{
	//	m_vecCollider[i]->Tick(m_ObjDesc.Matrix[i]);
	//}
}

void CMapGroupObject::Late_Tick(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		if (nullptr != m_pRendererCom)
		{
#ifdef _DEBUG
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
#else
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
#endif
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CMapGroupObject::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (nullptr != m_pModelInstanceCom)
	{
		_uint iNumMeshContainers = m_pModelInstanceCom->Get_NumMeshContainers();

		for (_uint i = 0; i < iNumMeshContainers; ++i)
		{
			if (true == m_pModelInstanceCom->Check_Texture(i, TextureType_NORMALS))
			{
				if (FAILED(m_pModelInstanceCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE)))
					return E_FAIL;
				if (FAILED(m_pModelInstanceCom->Bind_SRV(m_pShaderCom, "g_NormalTexture", i, TextureType_NORMALS)))
					return E_FAIL;

				m_pModelInstanceCom->Render_NonAnim(i, m_pShaderCom, 1);
			}
			else
			{
				if (FAILED(m_pModelInstanceCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE)))
					return E_FAIL;

				m_pModelInstanceCom->Render_NonAnim(i, m_pShaderCom, 3);
			}
		}
	}

	//#ifdef _DEBUG // _DEBUG
	//	for (_uint i = 0; i < m_ObjDesc.InstanceCount; ++i)
	//	{
	//		if (nullptr != m_vecCollider[i] && nullptr != m_pRendererCom)
	//			m_vecCollider[i]->Render_Debug();
	//	}
	//#endif

	return S_OK;
}

HRESULT CMapGroupObject::RenderLight(void)
{
	if (FAILED(__super::RenderLight()))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		_float4x4 WorldMatrix, ViewMatrix, ProjMatrix;
		WorldMatrix = m_pTransformCom->Get_WorldFloat4x4_TP();
		ViewMatrix = pGameInstance->Get_LightTransformFloat4x4_TP(CPipeLine::D3DTS_VIEW);
		ProjMatrix = pGameInstance->Get_LightTransformFloat4x4_TP(CPipeLine::D3DTS_PROJ);
		_float lightFar = pGameInstance->Get_LightFar();

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
		if (FAILED(m_pShaderCom->Set_RawValue("g_LightFar", &lightFar, sizeof(_float))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		_bool bIsDrawMotionBlur = false;
		if (FAILED(m_pShaderCom->Set_RawValue("g_ObjectMotionBlur", &bIsDrawMotionBlur, sizeof(_bool))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	RELEASE_INSTANCE(CGameInstance);

	if (nullptr != m_pModelInstanceCom)
	{
		_uint iNumMeshContainers = m_pModelInstanceCom->Get_NumMeshContainers();

		for (_uint i = 0; i < iNumMeshContainers; ++i)
		{
			if (FAILED(m_pModelInstanceCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE)))
				return E_FAIL;

			m_pModelInstanceCom->Render_NonAnim(i, m_pShaderCom, 2);
		}
	}
	return S_OK;
}

void CMapGroupObject::Extern_ImGuiClass(void)
{
}

HRESULT CMapGroupObject::Ready_Component(void)
{
	if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXMODELINSTANCE, (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(COM_TEXTURE, LEVEL_STATIC, PROTO_COMP_FILTER_TEXTURE, (CComponent**)&m_pFilterTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapGroupObject::SetUp_ConstantTable(void)
{
	if (Is_Nullptr(m_pShaderCom) || Is_Nullptr(m_pTransformCom))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		_float4x4 WorldMatrix, ViewMatrix, ProjMatrix, PrevViewMatrix;
		WorldMatrix = m_pTransformCom->Get_WorldFloat4x4_TP();
		ViewMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW);
		ProjMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ);
		PrevViewMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PREVVIEW);
		_float camFar = pGameInstance->Get_CamFar();
		_float3 vCamPosition = pGameInstance->Get_CamPositionF3();

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

		if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float3))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		if (FAILED(m_pFilterTexture->SetUp_ConstantTable_Index(m_pShaderCom, "g_FilterTexture", 1)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Set_RawValue("g_PrevWorldMatrix", &m_PrevWorldMatrix, sizeof(_float4x4))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Set_RawValue("g_PrevViewMatrix", &PrevViewMatrix, sizeof(_float4x4))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		m_PrevWorldMatrix = WorldMatrix;
		_bool bIsDrawMotionBlur = false;
		CCutScene_Manager* pManager = GET_INSTANCE(CCutScene_Manager);
		CBattle_Manager* pBattleManager = GET_INSTANCE(CBattle_Manager);
		{
			bIsDrawMotionBlur = pManager->Is_PlayCutScene() | pBattleManager->Is_MotionBlurOn();
		}
		RELEASE_INSTANCE(CCutScene_Manager);
		RELEASE_INSTANCE(CBattle_Manager);

		if (FAILED(m_pShaderCom->Set_RawValue("g_ObjectMotionBlur", &bIsDrawMotionBlur, sizeof(_bool))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Set_RawValue("g_bFilter", &m_bIsFilter, sizeof(_bool))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		{
			if (m_bIsFilter)
			{
				CTransform* pPlayerTransform = nullptr;
				if (LEVEL_STORY == g_iCurrentLevel && false == CBattle_Manager::Get_Instance()->IsStoryBattle())
					pPlayerTransform = (CTransform*)pGameInstance->Get_Component(g_iCurrentLevel, LAYER_STORYPLAYER, COM_TRANSFORM, 0);
				else
					pPlayerTransform = (CTransform*)pGameInstance->Get_Component(g_iCurrentLevel, LAYER_PLAYER, COM_TRANSFORM, 0);

				if (nullptr != pPlayerTransform)
				{
					_float3 vPlayerPos = pPlayerTransform->Get_PosF3();
					_vector vProjPos = XMVector3TransformCoord(vPlayerPos.Get_Vector(), pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
					_float fViewZ = XMVectorGetZ(vProjPos);

					if (FAILED(m_pShaderCom->Set_RawValue("g_fPlayerViewZ", &fViewZ, sizeof(_float))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return E_FAIL;
					}
				}
			}
		}
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CMapGroupObject::Free(void)
{
	__super::Free();
	Safe_Release(m_pModelInstanceCom);
	Safe_Release(m_pFilterTexture);
	//for (auto& iter : m_vecCollider)
	//{
	//	Safe_Release(iter);
	//}

	//for (auto& iter : m_vecColliderName)
	//{
	//	Safe_Delete_Array(iter);
	//}
}
