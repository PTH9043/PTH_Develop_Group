#include "MapGround.h"
#include "GameInstance.h"
#include "Battle_Manager.h"
#include "Level_Story.h"
#include "CutScene_Manager.h"

CMapGround::CMapGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPawn(pDevice, pContext)
{
}

CMapGround::CMapGround(const CMapGround& _rhs)
	: CPawn(_rhs)
{
}

CMapGround* CMapGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapGround* pInstance = new CMapGround(pDevice, pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CMapGround::Create To Failed");
#endif
	}
	return pInstance;
}

CGameObject* CMapGround::Clone(void* _pArg)
{
	CMapGround* pInstance = new CMapGround(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CMapGround::Clone To Failed");
#endif
	}
	return pInstance;
}

HRESULT CMapGround::NativeConstruct_Prototype(void)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapGround::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		_tchar Name[MAX_PATH] = L"";
		lstrcpy(Name, reinterpret_cast<_tchar*>(pArg));
		if (!lstrcmp(Name, PROTO_COMP_MODEL_UROKODAKIIWA_SKY))
		{
			if (LEVEL_BATTLE == g_iCurrentLevel)
			{
				if (FAILED(__super::Add_Component(COM_MODEL, LEVEL_BATTLE, PROTO_COMP_MODEL_UROKODAKIIWA_SKY, (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			m_eGroundType = GROUNDTYPE_SKY;
		}
		else if (!lstrcmp(Name, PROTO_COMP_MODEL_STORY_SKY))
		{
			if (LEVEL_STORY == g_iCurrentLevel)
			{
				if (FAILED(__super::Add_Component(COM_MODEL, LEVEL_STORY, PROTO_COMP_MODEL_STORY_SKY, (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}
			else if (LEVEL_GAMEPLAY == g_iCurrentLevel)
			{
				if (FAILED(__super::Add_Component(COM_MODEL, LEVEL_GAMEPLAY, PROTO_COMP_MODEL_STORY_SKY, (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}
			else if (LEVEL_BATTLE == g_iCurrentLevel)
			{
				if (FAILED(__super::Add_Component(COM_MODEL, LEVEL_BATTLE, PROTO_COMP_MODEL_STORY_SKY, (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}
			m_eGroundType = GROUNDTYPE_SKY;
		}
		else if (!lstrcmp(Name, PROTO_COMP_MODEL_UROKODAKIIWA_GROUND))
		{
			if (LEVEL_BATTLE == g_iCurrentLevel)
			{
				if (FAILED(__super::Add_Component(COM_MODEL, LEVEL_BATTLE, PROTO_COMP_MODEL_UROKODAKIIWA_GROUND, (CComponent**)&m_pModelCom)))
					return E_FAIL;

				if (FAILED(__super::Add_Component(COM_NAVIGATION, LEVEL_BATTLE, PROTO_COMP_NAVIGATION_UROKODAKI, (CComponent**)&m_pNavigationCom)))
					return E_FAIL;

				//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				//pGameInstance->SetEffectStateCall(L"snow02", m_pTransformCom->Get_WorldMatrix());
				//RELEASE_INSTANCE(CGameInstance);
			}

			m_eGroundType = GROUNDTYPE_GROUND;
			m_iMapCount = 1;
		}
		else if (!lstrcmp(Name, PROTO_COMP_MODEL_STORY_GROUND))
		{
			if (LEVEL_STORY == g_iCurrentLevel)
			{
				if (FAILED(__super::Add_Component(COM_MODEL, LEVEL_STORY, PROTO_COMP_MODEL_STORY_GROUND, (CComponent**)&m_pModelCom)))
					return E_FAIL;

				if (FAILED(__super::Add_Component(COM_NAVIGATION, LEVEL_STORY, PROTO_COMP_NAVIGATION_STORY, (CComponent**)&m_pNavigationCom)))
					return E_FAIL;
			}

			m_pTransformCom->Bind_WorldMatrix(m_pTransformCom->Get_WorldMatrix() * XMMatrixRotationY(-90.f));
			m_eGroundType = GROUNDTYPE_GROUND;
			m_iMapCount = 2;
		}
		else if (!lstrcmp(Name, PROTO_COMP_MODEL_RATTAN_GROUND))
		{
			if (LEVEL_RATTAN == g_iCurrentLevel)
			{
				if (FAILED(__super::Add_Component(COM_MODEL, LEVEL_RATTAN, PROTO_COMP_MODEL_RATTAN_GROUND, (CComponent**)&m_pModelCom)))
					return E_FAIL;

				if (FAILED(__super::Add_Component(COM_NAVIGATION, LEVEL_RATTAN, PROTO_COMP_NAVIGATION_RATTAN, (CComponent**)&m_pNavigationCom)))
					return E_FAIL;
			}
			if (LEVEL_MINIGAME == g_iCurrentLevel)
			{
				if (FAILED(__super::Add_Component(COM_MODEL, LEVEL_MINIGAME, PROTO_COMP_MODEL_RATTAN_GROUND, (CComponent**)&m_pModelCom)))
					return E_FAIL;

				if (FAILED(__super::Add_Component(COM_NAVIGATION, LEVEL_MINIGAME, PROTO_COMP_NAVIGATION_RATTAN, (CComponent**)&m_pNavigationCom)))
					return E_FAIL;
			}

			m_eGroundType = GROUNDTYPE_GROUND;
			m_iMapCount = 4;
		}
		else if (!lstrcmp(Name, PROTO_COMP_MODEL_RATTAN_SKY))
		{
			if (LEVEL_RATTAN == g_iCurrentLevel)
			{
				if (FAILED(__super::Add_Component(COM_MODEL, LEVEL_RATTAN, PROTO_COMP_MODEL_RATTAN_SKY, (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}
			else if (LEVEL_MINIGAME == g_iCurrentLevel)
			{
				if (FAILED(__super::Add_Component(COM_MODEL, LEVEL_MINIGAME, PROTO_COMP_MODEL_RATTAN_SKY, (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			m_eGroundType = GROUNDTYPE_SKY;
		}
		else if (!lstrcmp(Name, PROTO_COMP_MODEL_TEONI_GROUND))
		{
			if (LEVEL_GAMEPLAY == g_iCurrentLevel)
			{
				if (FAILED(__super::Add_Component(COM_MODEL, LEVEL_GAMEPLAY, PROTO_COMP_MODEL_TEONI_GROUND, (CComponent**)&m_pModelCom)))
					return E_FAIL;

				if (FAILED(__super::Add_Component(COM_NAVIGATION, LEVEL_GAMEPLAY, PROTO_COMP_NAVIGATION_STORY_TEONI, (CComponent**)&m_pNavigationCom)))
					return E_FAIL;
			}
			else if (LEVEL_BATTLE == g_iCurrentLevel)
			{
				if (FAILED(__super::Add_Component(COM_MODEL, LEVEL_BATTLE, PROTO_COMP_MODEL_TEONI_GROUND, (CComponent**)&m_pModelCom)))
					return E_FAIL;

				if (FAILED(__super::Add_Component(COM_NAVIGATION, LEVEL_BATTLE, PROTO_COMP_NAVIGATION_STORY_TEONI, (CComponent**)&m_pNavigationCom)))
					return E_FAIL;
			}

			m_eGroundType = GROUNDTYPE_GROUND;
			m_iMapCount = 5;
		}
		else if (!lstrcmp(Name, PROTO_COMP_MODEL_TAMAYO_GROUND))
		{
			if (LEVEL_BATTLE == g_iCurrentLevel)
			{
				if (FAILED(__super::Add_Component(COM_MODEL, LEVEL_BATTLE, PROTO_COMP_MODEL_TAMAYO_GROUND, (CComponent**)&m_pModelCom)))
					return E_FAIL;

				if (FAILED(__super::Add_Component(COM_NAVIGATION, LEVEL_BATTLE, PROTO_COMP_NAVIGATION_TAMAYO, (CComponent**)&m_pNavigationCom)))
					return E_FAIL;

				//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				//pGameInstance->SetEffectStateCall(L"pink02", m_pTransformCom->Get_WorldMatrix());
				//RELEASE_INSTANCE(CGameInstance);
			}

			m_eGroundType = GROUNDTYPE_GROUND;
			m_iMapCount = 6;
		}
		else if (!lstrcmp(Name, PROTO_COMP_MODEL_ASAKUSA_GROUND))
		{
			if (LEVEL_BATTLE == g_iCurrentLevel)
			{
				if (FAILED(__super::Add_Component(COM_MODEL, LEVEL_BATTLE, PROTO_COMP_MODEL_ASAKUSA_GROUND, (CComponent**)&m_pModelCom)))
					return E_FAIL;

				if (FAILED(__super::Add_Component(COM_NAVIGATION, LEVEL_BATTLE, PROTO_COMP_NAVIGATION_UROKODAKI, (CComponent**)&m_pNavigationCom)))
					return E_FAIL;
			}

			m_eGroundType = GROUNDTYPE_GROUND;
			m_iMapCount = 3;
		}
	}
	else
		return E_FAIL;

	{
		// Sky 안개 색상 정하기
		if (LEVEL_STORY == g_iCurrentLevel)
		{
			m_fogColor = _float4(0.05f, 0.15f, 0.3f, 1.f);
		}
		else if (LEVEL_GAMEPLAY == g_iCurrentLevel)
		{
			m_fogColor = _float4(0.05f, 0.2f, 0.4f, 1.f);
		}
		else if (LEVEL_BATTLE == g_iCurrentLevel)
		{
			CBattle_Manager::BATTLEMAP_TYPE mapType = CBattle_Manager::Get_Instance()->Get_BattleInfo().eBattleMap;
			if (CBattle_Manager::MAP_TAMAYO == mapType)
				m_fogColor = _float4(0.05f, 0.1f, 0.4f, 1.f);
			else if (CBattle_Manager::MAP_UROKODAKI == mapType)
				m_fogColor = _float4(0.2f, 0.25f, 0.35f, 1.f);
			else if (CBattle_Manager::MAP_HUJIKASANE == mapType)
				m_fogColor = _float4(0.05f, 0.2f, 0.4f, 1.f);
		}
	}


	return S_OK;
}

void CMapGround::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);


	//if (m_pRendererCom->Get_DebugRender()->bEffect /*&&false == m_bEffectCreateCheck*/)
	{
		if (1 == m_iMapCount && m_pRendererCom->Get_DebugRender()->bEffect)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->SetEffectStateCall(L"snow02", m_pTransformCom->Get_WorldMatrix());
			RELEASE_INSTANCE(CGameInstance);
			m_pRendererCom->Get_DebugRender()->bEffect = false;
		}
		else if (6 == m_iMapCount && m_pRendererCom->Get_DebugRender()->bEffect)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->SetEffectStateCall(L"pink02", m_pTransformCom->Get_WorldMatrix());
			RELEASE_INSTANCE(CGameInstance);
			m_pRendererCom->Get_DebugRender()->bEffect = false;
		}

		//m_bEffectCreateCheck = true;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pPlayer;
	if (true == CBattle_Manager::Get_Instance()->IsStoryBattle())
	{
		pPlayer = pGameInstance->Find_GameObject(g_iCurrentLevel, LAYER_PLAYER, 0);
	}
	else
	{
		pPlayer = pGameInstance->Find_GameObject(g_iCurrentLevel, LAYER_STORYPLAYER, 0);
	}
	if (nullptr != pPlayer)
	{
		_float3 fPos = pPlayer->Get_Transform()->Get_PosF3();
		if (GROUNDTYPE_SKY == m_eGroundType /*&& LEVEL_BATTLE == g_iCurrentLevel*/)
			m_pTransformCom->Set_PosF3(_float3{ fPos.x, fPos.y - 20.f, fPos.z });
	}


	RELEASE_INSTANCE(CGameInstance);
}

void CMapGround::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		if (GROUNDTYPE_GROUND == m_eGroundType)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		else if (GROUNDTYPE_SKY == m_eGroundType)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
	}
}

HRESULT CMapGround::Render(void)
{
	if (nullptr == m_pModelCom || nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (GROUNDTYPE_GROUND == m_eGroundType)
	{
		_uint iNumMeshContainers = m_pModelCom->Get_NumMeshContainers();
		for (_uint i = 0; i < iNumMeshContainers; ++i)
		{
			if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE)))
				return E_FAIL;
			//if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_NormalTexture", i, TextureType_NORMALS)))
			//	return E_FAIL;
			m_pModelCom->Render_NonAnim(i, m_pShaderCom, 0);
		}

#ifdef _DEBUG
		if (nullptr != m_pNavigationCom)
			m_pNavigationCom->Render_Debug();
#endif // _DEBUG

	}
	else if (GROUNDTYPE_SKY == m_eGroundType)
	{

		if (FAILED(m_pShaderCom->Set_RawValue("fogColor", &m_fogColor, sizeof(_float4))))
			return E_FAIL;


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

HRESULT CMapGround::RenderLight(void)
{
	if (FAILED(__super::RenderLight()))
		return E_FAIL;

	return S_OK;
}

void CMapGround::Extern_ImGuiClass()
{
	ImGui::DragFloat4("SkyFog", &m_fogColor.x);

}

HRESULT CMapGround::Ready_Component(void)
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXMODEL, (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapGround::SetUp_ConstantTable(void)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (nullptr == m_pTransformCom || nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		_float4x4 WorldMatrix, ViewMatrix, ProjMatrix, PrevViewMatrix;
		WorldMatrix = m_pTransformCom->Get_WorldFloat4x4_TP();
		ViewMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW);
		ProjMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ);
		PrevViewMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PREVVIEW);
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
		{
			bIsDrawMotionBlur = pManager->Is_PlayCutScene();
		}
		RELEASE_INSTANCE(CCutScene_Manager);

		if (FAILED(m_pShaderCom->Set_RawValue("g_ObjectMotionBlur", &bIsDrawMotionBlur, sizeof(_bool))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		bool bFilter = false;
		if (FAILED(m_pShaderCom->Set_RawValue("g_bFilter", &bFilter, sizeof(_bool))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CMapGround::Free(void)
{
	Safe_Release(m_pNavigationCom);
	__super::Free();
}
