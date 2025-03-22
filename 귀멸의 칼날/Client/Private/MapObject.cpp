#include "MapObject.h"
#include "GameInstance.h"
#include "MeshContainer.h"

#include "Battle_Manager.h"
#include "CutScene_Manager.h"

CMapObject::CMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPawn(pDevice, pContext)
{
}

CMapObject::CMapObject(const CMapObject& _rhs)
	: CPawn(_rhs)
{
}

CMapObject* CMapObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapObject* pInstance = new CMapObject(pDevice, pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CMapObject::Create To Failed");
#endif
	}
	return pInstance;
}

CGameObject* CMapObject::Clone(void* _pArg)
{
	CMapObject* pInstance = new CMapObject(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CMapObject::Clone To Failed");
#endif
	}
	return pInstance;
}

HRESULT CMapObject::NativeConstruct_Prototype(void)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapObject::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		_matrix WorldMat = XMLoadFloat4x4(&reinterpret_cast<MAPOBJDESC*>(pArg)->fMatrix);
		m_pTransformCom->Bind_WorldMatrix(WorldMat);
		m_pTransformCom->Set_Scale(reinterpret_cast<MAPOBJDESC*>(pArg)->vSize);

		// 모델이름 받아온걸로 모델 컴포넌트 추가
		if (LEVEL_GAMEPLAY == g_iCurrentLevel)
		{
			if (FAILED(__super::Add_Component(COM_MODEL, LEVEL_GAMEPLAY,
				reinterpret_cast<MAPOBJDESC*>(pArg)->ModelName, (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}
		else if (LEVEL_STORY == g_iCurrentLevel)
		{
			if (FAILED(__super::Add_Component(COM_MODEL, LEVEL_STORY,
				reinterpret_cast<MAPOBJDESC*>(pArg)->ModelName, (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}
		else if (LEVEL_BATTLE == g_iCurrentLevel)
		{
			if (FAILED(__super::Add_Component(COM_MODEL, LEVEL_BATTLE,
				reinterpret_cast<MAPOBJDESC*>(pArg)->ModelName, (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}
		else if (LEVEL_RATTAN == g_iCurrentLevel)
		{
			if (FAILED(__super::Add_Component(COM_MODEL, LEVEL_RATTAN,
				reinterpret_cast<MAPOBJDESC*>(pArg)->ModelName, (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}
		else if (LEVEL_MINIGAME == g_iCurrentLevel)
		{
			if (FAILED(__super::Add_Component(COM_MODEL, LEVEL_MINIGAME,
				reinterpret_cast<MAPOBJDESC*>(pArg)->ModelName, (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}
	}
	else
		return E_FAIL;

	// 콜라이더 딱 맞추게 계산할것
	for (auto& mesh : m_pModelCom->Get_MeshContainers())
	{
		m_vMin = _float3{ min(m_vMin.x, mesh->Get_MinVertex().x), min(m_vMin.y, mesh->Get_MinVertex().y), min(m_vMin.z, mesh->Get_MinVertex().z) };
		m_vMax = _float3{ max(m_vMax.x, mesh->Get_MaxVertex().x), max(m_vMax.y, mesh->Get_MaxVertex().y), max(m_vMax.z, mesh->Get_MaxVertex().z) };
	}

	_float fscale = 0.f;
	_float3 vtranslation = _float3{ 0.f, (m_vMax.y - m_vMin.y) / 2.f, 0.f };

	fscale = max(fscale, (m_vMax.x - m_vMin.x) / 2.f);
	fscale = max(fscale, (m_vMax.y - m_vMin.y) / 2.f);
	fscale = max(fscale, (m_vMax.z - m_vMin.z) / 2.f);

	CCollider::COLLIDERDESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vScale = _float3{ fscale, fscale, fscale };
	ColliderDesc.vTranslation = vtranslation;

	if (FAILED(__super::Add_Component(COM_COLLIDER_SPHERE, LEVEL_STATIC, PROTO_COMP_SPHERECOLLIDER, (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

void CMapObject::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
}

void CMapObject::Late_Tick(const _double& _dTimeDelta)
{
	if (false == m_bActive)
		return;
	//__super::Late_Tick(_dTimeDelta);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		if(LEVEL_STORY == g_iCurrentLevel)
		{
			if (true == pGameInstance->Is_Rendering(m_pColliderCom->Get_CurPos(), m_pColliderCom->Get_CurScaleV3().x / 2.f))
			{
				if (nullptr != m_pRendererCom)
				{
#ifdef _DEBUG
					m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
#else
					m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
					m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
#endif
				}
			}
		}
		else
		{
			if (nullptr != m_pRendererCom)
			{
#ifdef _DEBUG
				m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
#else
				m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
				m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
#endif
			}
		}
	}

#ifdef _DEBUG

	if (nullptr != m_pColliderCom && nullptr != m_pRendererCom)
		m_pRendererCom->Add_Components(CRenderer::DEBUG_MAP, m_pColliderCom);

#endif // DEBUG

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CMapObject::Render(void)
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
			if (true == m_pModelCom->Check_Texture(i , TextureType_EMISSIVE))
			{
				if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE)))
					return E_FAIL;
				if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_NormalTexture", i, TextureType_NORMALS)))
					return E_FAIL;
				if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_SubDiffuseTexture", i, TextureType_SPECULAR)))
					return E_FAIL;
				if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_EmissiveTexture", i, TextureType_EMISSIVE)))
					return E_FAIL;
				_float fIntensity = 3.f;
				if (m_pShaderCom->Set_RawValue("fEmissiveIntensity", &fIntensity, sizeof(_float)))
					return E_FAIL;

				m_pModelCom->Render_NonAnim(i, m_pShaderCom, 15);
			}
			else if(true == m_pModelCom->Check_Texture(i, TextureType_NORMALS))
			{
				if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE)))
					return E_FAIL;
				if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_NormalTexture", i, TextureType_NORMALS)))
					return E_FAIL;

				m_pModelCom->Render_NonAnim(i, m_pShaderCom, 2);
			}
			else
			{
				if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE)))
					return E_FAIL;

				m_pModelCom->Render_NonAnim(i, m_pShaderCom, 0);
			}
		}
	}

#ifdef _DEBUG // _DEBUG
	//if(true == m_bColliderRenderCheck)
	//	m_pColliderCom->Render_Debug();
#endif

	return S_OK;
}

HRESULT CMapObject::RenderLight(void)
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
	}
	RELEASE_INSTANCE(CGameInstance);

	_bool bIsDrawMotionBlur = false;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ObjectMotionBlur", &bIsDrawMotionBlur, sizeof(_bool))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}


	if (nullptr != m_pModelCom)
	{
		_uint iNumMeshContainers = m_pModelCom->Get_NumMeshContainers();

		for (_uint i = 0; i < iNumMeshContainers; ++i)
		{
			if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE)))
				return E_FAIL;

			m_pModelCom->Render_NonAnim(i, m_pShaderCom, 5);
		}
	}
	return S_OK;
}

void CMapObject::Extern_ImGuiClass(void)
{
	//static float ObjPos[3] = { 0.f, 0.f, 0.f };
	//{
	//	ObjPos[0] = m_pTransformCom->Get_PosF3().x; 
	//	ObjPos[1] = m_pTransformCom->Get_PosF3().y; 
	//	ObjPos[2] = m_pTransformCom->Get_PosF3().z; 
	//}

	//ImGui::TextColored(ImVec4(0.6f, 0.9f, 1.0f, 1.0f), u8"오브젝트 위치");
	//ImGui::Text(CMethod::Convert_Number_S(ObjPos[0], 3).c_str());
	//ImGui::SameLine();
	//ImGui::Text(" / ");
	//ImGui::SameLine();
	//ImGui::Text(CMethod::Convert_Number_S(ObjPos[1], 3).c_str());
	//ImGui::SameLine();
	//ImGui::Text(" / ");
	//ImGui::SameLine();
	//ImGui::Text(CMethod::Convert_Number_S(ObjPos[2], 3).c_str());

}

HRESULT CMapObject::Ready_Component(void)
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXMODEL, (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(COM_TEXTURE, LEVEL_STATIC, PROTO_COMP_FILTER_TEXTURE, (CComponent**)&m_pFilterTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapObject::SetUp_ConstantTable(void)
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



		{
			bool bFilter = false;
			if (LEVEL_STORY == g_iCurrentLevel)
				bFilter = true;
			if (FAILED(m_pShaderCom->Set_RawValue("g_bFilter", &bFilter, sizeof(_bool))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}

			if (bFilter)
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

void CMapObject::Free(void)
{
	__super::Free();

	Safe_Release(m_pFilterTexture);
}
