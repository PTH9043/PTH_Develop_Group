#include "TeOni_Rock.h"
#include "GameInstance.h"
#include "Camera_Manager.h"
#include "Camera_Free.h"

CTeOni_Rock::CTeOni_Rock(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CItem(_pDevice, _pContext)
{
}

CTeOni_Rock::CTeOni_Rock(const CItem& _rhs)
	: CItem(_rhs)
{
}

HRESULT CTeOni_Rock::NativeConstruct_Prototype(void)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_eItemType = ITEM_BULLET;
	return S_OK;
}

HRESULT CTeOni_Rock::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pTeOni = pGameInstance->Find_GameObject(g_iCurrentLevel, LAYER_MONSTER, 0);
	if (nullptr != pTeOni)
		m_pOniTransform = pTeOni->Get_Transform();
	Safe_AddRef(m_pOniTransform);

	m_pPlayer = pGameInstance->Find_GameObject(g_iCurrentLevel, LAYER_PLAYER, 0);
	Safe_AddRef(m_pPlayer);

	RELEASE_INSTANCE(CGameInstance);

	_vector vPos = m_pOniTransform->Get_Pos();
	_vector vRight = XMVector3Normalize(m_pOniTransform->Get_Right());
	_vector vUp = XMVector3Normalize(m_pOniTransform->Get_Up());

	if (nullptr != pArg)
	{
		int i = *reinterpret_cast<_uint*>(pArg);
		if (0 == i)
			m_pTransformCom->Set_Pos(vPos + vRight * 30.f + vUp * 10.f);
		else
			m_pTransformCom->Set_Pos(vPos + vRight * -30.f + vUp * 10.f);

	}

	m_pTransformCom->Update_Nav(m_pNavigationCom);

	return S_OK;
}

void CTeOni_Rock::Tick(const _double& _dTimeDelta)
{
	m_dDeleteTime += _dTimeDelta;

	// 날리기
	if (nullptr != m_pTransformCom && nullptr != m_pPlayer)
	{
		if (false == m_bDirCheck)
		{
			_float3 vPlayer = m_pPlayer->Get_Transform()->Get_PosF3();
			_float3 vMyPos = m_pTransformCom->Get_PosF3();
			_float3 vNewPos = { vPlayer.x, vMyPos.y, vPlayer.z };
			m_vDir = XMVector3Normalize(XMLoadFloat3(&vNewPos) - m_pTransformCom->Get_Pos());
			m_bDirCheck = true;
		}
		else
		{
			_float3 vPos = m_pTransformCom->Get_PosF3();
			_float3 vRight = _float3{ 1.f, 0.f, 0.f };
			_float3 vUp = _float3{ 0.f, 1.f, 0.f };
			_float3 vLook = _float3{ 0.f, 0.f, 1.f };
			_float4x4 mat = _float4x4{ 1.f, 0.f, 0.f, 0.f,
										0.f, 1.f, 0.f, 0.f,
										0.f, 0.f, 1.f, 0.f,
										vPos.x, vPos.y, vPos.z, 1.f };

			if (0 != m_pTransformCom->Get_Scale().x)
				m_pTransformCom->Rotate_Turn(-m_pTransformCom->Get_Right(), _dTimeDelta, 1.f);

			if (false == m_pTransformCom->Move_Pos_NotNavigateY(m_pTransformCom->Get_Pos() + m_vDir * (_float)(_dTimeDelta * 180.f))
				&& 1 <= m_dDeleteTime)
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				pGameInstance->SetEffectStateCall(L"TEONI_STONE_1", m_pTransformCom->Get_WorldMatrix());
				pGameInstance->SetEffectStateCall(L"TEONI_STONE_6", XMLoadFloat4x4(&mat));

				pGameInstance->Play_Sound_ByCondition(TEXT("TEONI_ROCK_N"), 0.6f);
				RELEASE_INSTANCE(CGameInstance);

				m_pTransformCom->Set_Scale(_float3{ 0.f, 0.f, 0.f });
				Delete_Self();
			}
			if (true == m_pColliderCom->Is_Collision(static_cast<CCollider*>(m_pPlayer->Get_Component(COM_COLLIDER_HIT))))
			{
				CCamera_Manager* pCameraManager = GET_INSTANCE(CCamera_Manager);
				static_cast<CCamera_Free*>(pCameraManager->Get_ActiveCamera())->Start_CameraShake(0.4f, 1.5f);
				RELEASE_INSTANCE(CCamera_Manager);

				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				pGameInstance->SetEffectStateCall(L"TEONI_STONE_1", m_pTransformCom->Get_WorldMatrix());
				pGameInstance->SetEffectStateCall(L"TEONI_STONE_6", XMLoadFloat4x4(&mat));

				pGameInstance->Play_Sound_ByCondition(TEXT("TEONI_ROCK_P"), 0.6f);
				RELEASE_INSTANCE(CGameInstance);

				m_pTransformCom->Set_Scale(_float3{ 0.f, 0.f, 0.f });
				Delete_Self();
			}
		}
	}

	__super::Tick(_dTimeDelta);

	if (nullptr != m_pColliderCom)
	{
		m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
	}
}

void CTeOni_Rock::Late_Tick(const _double& _dTimeDelta)
{
	// __super::Late_Tick(_dTimeDelta);

	if (m_bIsColliderActive == true)
	{
		__super::Late_Tick(_dTimeDelta);
	}

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CTeOni_Rock::Render(void)
{
	//if (FAILED(__super::Render()))
	//    return E_FAIL;

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

			m_pModelCom->Render_NonAnim(i, m_pShaderCom, 13);
		}
	}

#ifdef _DEBUG
	Coll_Render();
#endif // _DEBUG

	return S_OK;
}

HRESULT CTeOni_Rock::RenderLight(void)
{
	if (FAILED(__super::RenderLight()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTeOni_Rock::SetUp_ConstantTable(void)
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	_float4 edgecolor = _float4(1.f, 0.f, 0.f, 1.f);
	if (FAILED(m_pShaderCom->Set_RawValue("g_vEdgeGlowColor", &edgecolor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTeOni_Rock::SetUp_Components(void)
{
	if (FAILED(__super::SetUp_Components()))
		return E_FAIL;

	// 모델 세팅 
	if (FAILED(Add_Component(COM_MODEL, g_iCurrentLevel,
		PROTO_COMP_MODEL_ITEM_TEONIROCK, (CComponent**)&m_pModelCom)))
		return E_FAIL;

	// 콜라이더 세팅 
	CCollider::COLLIDERDESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vScale = _float3{ 15.f, 15.f, 15.f };
	ColliderDesc.vTranslation = _float3{ 0.f, 0.f, 0.f };
	if (FAILED(Add_Component(COM_COLLIDER_SPHERE, LEVEL_STATIC,
		PROTO_COMP_SPHERECOLLIDER, (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	// 네비게이션
	if (LEVEL_STORY == g_iCurrentLevel)
	{
		if (FAILED(Add_Component(COM_NAVIGATION, g_iCurrentLevel, PROTO_COMP_NAVIGATION_STORY, (CComponent**)&m_pNavigationCom)))
			return E_FAIL;
	}
	else if (LEVEL_GAMEPLAY == g_iCurrentLevel)
	{
		if (FAILED(Add_Component(COM_NAVIGATION, g_iCurrentLevel, PROTO_COMP_NAVIGATION_STORY_TEONI, (CComponent**)&m_pNavigationCom)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CTeOni_Rock::SetUp_LoadData(void* _pArg)
{
	return S_OK;
}

HRESULT CTeOni_Rock::Delete_Self(void)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Add_DeleteObject(g_iCurrentLevel, LAYER_PROJECTILE, this);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CTeOni_Rock* CTeOni_Rock::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CTeOni_Rock* pInstance = new CTeOni_Rock(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CTeOni_Rock::Create To Failed");
#endif
	}
	return pInstance;
}

CGameObject* CTeOni_Rock::Clone(void* pArg)
{
	CTeOni_Rock* pInstance = new CTeOni_Rock(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CTeOni_Rock::Clone To Failed");
#endif
	}
	return pInstance;
}

void CTeOni_Rock::Free(void)
{
	Safe_Release(m_pPlayer);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pOniTransform);
	__super::Free();
}
