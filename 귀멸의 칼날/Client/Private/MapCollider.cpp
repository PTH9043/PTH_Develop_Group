#include "MapCollider.h"

#include "Character.h"
#include "Battle_Manager.h"
#include "Camera_Manager.h"
#include "Level_Story.h"
#include "GameInstance.h"
#include "Story_Tanjiro.h"
#include "Camera_Free.h"

CMapCollider::CMapCollider(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	:CGameObject(_pDevice, _pContext)
{
}

CMapCollider::CMapCollider(const CMapCollider& _rhs)
	:CGameObject(_rhs)
{
}

HRESULT CMapCollider::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CMapCollider::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_vScale = _float3{ 5.f, 10.f, 58.f };
	m_vPos = _float3{ -743.f, 178.f, 327.f };


	return S_OK;
}

void CMapCollider::Tick(const _double& _dTimeDelta)
{
#ifdef _DEBUG
	if (nullptr != m_TmpCollider)
	{
		_float3 vRotRadian = _float3(XMConvertToRadians(m_vRot.x), XMConvertToRadians(m_vRot.y), XMConvertToRadians(m_vRot.z));
		_matrix mat = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z) * XMMatrixRotationRollPitchYaw(vRotRadian.x, vRotRadian.y, vRotRadian.z) * XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z);
		m_TmpCollider->Tick(mat);
	}
#endif // _DEBUG
	
	CBattle_Manager* pBattleInstance = GET_INSTANCE(CBattle_Manager);
	{
		if (nullptr == m_pStoryPlayer)
		{
			CCharacter* pStoryPlayer = pBattleInstance->Get_BattleCharacter(CBattle_Manager::CHAR_AI);
			if (nullptr != pStoryPlayer)
			{
				m_pStoryPlayer = pStoryPlayer;
				Safe_AddRef(pStoryPlayer);
			}
		}
	}
	RELEASE_INSTANCE(CBattle_Manager);


	Check_Collision();
}

void CMapCollider::Late_Tick(const _double& _dTimeDelta)
{
#ifdef _DEBUG
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
#endif
}

HRESULT CMapCollider::Render()
{
#ifdef _DEBUG
	for (_uint i = 0; i < COLLIDER_END; i++)
	{
		for (auto& iter : m_MapCollider[i])
			iter.second->Render_Debug();
	}
	if (nullptr != m_TmpCollider)
		m_TmpCollider->Render_Debug();
#endif // _DEBUG

	return S_OK;
}

void CMapCollider::Extern_ImGuiClass()
{
	ImGui::DragFloat3("Pos", &m_vPos.x);
	ImGui::DragFloat3("Scale", &m_vScale.x);
	ImGui::DragFloat3("Rot", &m_vRot.x);

}

void CMapCollider::Check_Collision()
{
	if (nullptr == m_pStoryPlayer)
		return;

	CTransform* pStoryTransform = m_pStoryPlayer->Get_Transform();
	if (nullptr == pStoryTransform)
		return;

	_vector vStoryPlayerPos = pStoryTransform->Get_Pos();
	for (_uint i = 0; i < COLLIDER_END; i++)
	{
		for (auto& iter : m_MapCollider[i])
		{
			// 일정 거리 이하인 콜라이더들만 충돌 검사
			CCollider* pCurCollider = iter.second;
			_vector vColliderPos = pCurCollider->Get_CurPos();
			if (XMVectorGetX(XMVector3Length(vStoryPlayerPos - vColliderPos)) < 100.f)
			{
				if (BATTLE_COLLIDER == i)
				{
					Check_BattleCollider(iter.first, iter.second);
				}
				else if (JUMP_COLLIDER == i)
				{
					Check_JumpCollider(iter.first, iter.second);
				}
				else if (SLIDE_COLLIDER == i)
				{
					Check_SlideCollider(iter.first, iter.second);
				}
				else if (ENDING_COLLIDER == i)
				{
					Check_EndingCollider(iter.first, iter.second);
				}
			}
		}
	}

}

void CMapCollider::Check_BattleCollider(_string _tagCollider, CCollider* _pBattleCollider)
{
	if (nullptr == m_pStoryPlayer)
		return;

	CCollider* pStoryCollider = (CCollider*)m_pStoryPlayer->Get_Component(COM_COLLIDER_HIT);
	if (nullptr == pStoryCollider)
		return;

	CCamera_Manager* pCameraInstance = GET_INSTANCE(CCamera_Manager);
	CBattle_Manager* pBattleInstance = GET_INSTANCE(CBattle_Manager);
	{
		// 이미 배틀상태의 경우 또는 한번 충돌된 경우 무시
		if (pBattleInstance->IsStoryBattle() || _pBattleCollider->Is_Collision())
		{
			RELEASE_INSTANCE(CCamera_Manager);
			RELEASE_INSTANCE(CBattle_Manager);
			return;
		}

		CCharacter* pPlayer = pBattleInstance->Get_BattleCharacter(CBattle_Manager::CHAR_PLAYER);
		if (nullptr == pPlayer)
		{
			RELEASE_INSTANCE(CGameInstance);
			RELEASE_INSTANCE(CBattle_Manager);
			return;
		}

		// 배틀 콜라이더 충돌 후 시네마틱 카메라 활성화
		if (_pBattleCollider->Is_Collision(pStoryCollider))
		{
			if ("Battle_Oni1" == _tagCollider)
			{
				pCameraInstance->Set_ActiveCamera(CM_CAMERA_ONI1);
				pPlayer->Set_Navigation(PROTO_COMP_NAVIGATION_STORY_ONI1);
				pPlayer->Get_Transform()->Update_Navigation_Cell(XMVectorSet(-767.f, 175.7f, 318.f, 0.f));
			}
			else if ("Battle_Oni2" == _tagCollider)
			{
				pCameraInstance->Set_ActiveCamera(CM_CAMERA_ONI2);
				pPlayer->Set_Navigation(PROTO_COMP_NAVIGATION_STORY_ONI2);
				pPlayer->Get_Transform()->Update_Navigation_Cell(XMVectorSet(-1079.f, 227.f, 736.f, 0.f));
			}
			else if ("Battle_Oni3" == _tagCollider)
			{
				pCameraInstance->Set_ActiveCamera(CM_CAMERA_ONI3);
				pPlayer->Set_Navigation(PROTO_COMP_NAVIGATION_STORY_ONI3);
				pPlayer->Get_Transform()->Update_Navigation_Cell(XMVectorSet(-1684.f, 154.f, 1444.f, 0.f));
			}
			pBattleInstance->Set_IsStoryBattle(true);
			
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			{
				pGameInstance->Stop_All();
				pGameInstance->Play_BGM(L"BGM_ONI2", 0.5f);
			}
			RELEASE_INSTANCE(CGameInstance);
		}
	}
	RELEASE_INSTANCE(CCamera_Manager);
	RELEASE_INSTANCE(CBattle_Manager);
}

void CMapCollider::Check_JumpCollider(_string _tagCollider, CCollider* _pJumpCollider)
{
	if (nullptr == m_pStoryPlayer)
		return;

	CCollider* pStoryCollider = (CCollider*)m_pStoryPlayer->Get_Component(COM_COLLIDER_HIT);
	if (nullptr == pStoryCollider)
		return;

	if (_pJumpCollider->Is_Collision(pStoryCollider))
	{
		_vector vJumpLook = XMVectorSetY(-1.f * _pJumpCollider->Get_TransformMatrix().r[0], 0.f);
		dynamic_cast<CStory_Tanjiro*>(m_pStoryPlayer)->Collision_Jump(vJumpLook);
	}
}

void CMapCollider::Check_SlideCollider(_string _tagCollider, CCollider* _SlideCollider)
{
	if (nullptr == m_pStoryPlayer)
		return;

	CCollider* pStoryCollider = (CCollider*)m_pStoryPlayer->Get_Component(COM_COLLIDER_HIT);
	if (nullptr == pStoryCollider)
		return;

	if (_SlideCollider->Is_Collision(pStoryCollider))
	{
		_vector vSlideLook = XMVectorSetY(-1.f * _SlideCollider->Get_TransformMatrix().r[0], 0.f);
		dynamic_cast<CStory_Tanjiro*>(m_pStoryPlayer)->Collision_Slide(vSlideLook);
	}
}

void CMapCollider::Check_EndingCollider(_string _tagCollider, CCollider* _EndingCollider)
{
	if (nullptr == m_pStoryPlayer)
		return;

	CCollider* pStoryCollider = (CCollider*)m_pStoryPlayer->Get_Component(COM_COLLIDER_HIT);
	if (nullptr == pStoryCollider)
		return;

	if (_EndingCollider->Is_Collision(pStoryCollider))
	{
		CCamera* pCamera = CCamera_Manager::Get_Instance()->Get_ActiveCamera();
		if (nullptr != pCamera)
		{
			dynamic_cast<CCamera_Free*>(pCamera)->Set_StoryEnding(true);
			CGameInstance::Get_Instance()->Play_Sound_ByCondition(L"Teoni_Find", 0.3f);
			m_pRendererCom->Start_FadeInOut(0.01f, true, 1);
		}
	}
}

HRESULT CMapCollider::SetUp_Components()
{
	if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	_float3 vScale;
	_float3 vRot;
	_float3 vPos;

	{
		// 오니 배틀1
		vScale = _float3(5.f, 10.f, 55.f); vRot = _float3(0.f, -23.f, 0.f); vPos = _float3(-729.f, 178.f, 334.f);
		if (FAILED(Add_Collider(BATTLE_COLLIDER, _string("Battle_Oni1"), vScale, vRot, vPos)))
			return E_FAIL;

		// 오니 배틀2
		vScale = _float3(5.f, 10.f, 55.f); vRot = _float3(0.f, -113.f, 0.f); vPos = _float3(-1060.f, 227.f, 677.f);
		if (FAILED(Add_Collider(BATTLE_COLLIDER, _string("Battle_Oni2"), vScale, vRot, vPos)))
			return E_FAIL;

		// 오니 배틀3
		vScale = _float3(5.f, 10.f, 88.f); vRot = _float3(0.f, -57.f, 0.f); vPos = _float3(-1719.f, 155.f, 1403.f);
		if (FAILED(Add_Collider(BATTLE_COLLIDER, _string("Battle_Oni3"), vScale, vRot, vPos)))
			return E_FAIL;
	}

	{
		// 점프업1
		vScale = _float3(5.f, 10.f, 58.f); vRot = _float3(0.f, 44.f, 0.f); vPos = _float3(-930.f, 173.f, 358.f);
		if (FAILED(Add_Collider(JUMP_COLLIDER, _string("JumpUp1"), vScale, vRot, vPos)))
			return E_FAIL;

		// 점프다운1
		vScale = _float3(5.f, 10.f, 58.f); vRot = _float3(0.f, -136.f, 0.f); vPos = _float3(-955.f, 188.f, 381.f);
		if (FAILED(Add_Collider(JUMP_COLLIDER, _string("JumpDown1"), vScale, vRot, vPos)))
			return E_FAIL;

		// 점프업2
		vScale = _float3(5.f, 10.f, 62.f); vRot = _float3(0.f, 103.f, 0.f); vPos = _float3(-1007.f, 187.f, 438.f);
		if (FAILED(Add_Collider(JUMP_COLLIDER, _string("JumpUp2"), vScale, vRot, vPos)))
			return E_FAIL;

		// 점프다운2
		vScale = _float3(5.f, 10.f, 62.f); vRot = _float3(0.f, -77.f, 0.f); vPos = _float3(-1003.f, 201.f, 460.f);
		if (FAILED(Add_Collider(JUMP_COLLIDER, _string("JumpDown2"), vScale, vRot, vPos)))
			return E_FAIL;

		// 점프업3
		vScale = _float3(5.f, 10.f, 55.f); vRot = _float3(0.f, 67.f, 0.f); vPos = _float3(-1022.f, 216.f, 590.f);
		if (FAILED(Add_Collider(JUMP_COLLIDER, _string("JumpUp3"), vScale, vRot, vPos)))
			return E_FAIL;

		// 점프다운3
		vScale = _float3(5.f, 10.f, 55.f); vRot = _float3(0.f, -113.f, 0.f); vPos = _float3(-1030.f, 228.f, 611.f);
		if (FAILED(Add_Collider(JUMP_COLLIDER, _string("JumpDown3"), vScale, vRot, vPos)))
			return E_FAIL;
	}

	{
		// 슬라이딩1
		vScale = _float3(5.f, 10.f, 28.f); vRot = _float3(0.f, 12.f, 0.f); vPos = _float3(-1230.672f, 191.413f, 359.253f);
		if (FAILED(Add_Collider(SLIDE_COLLIDER, _string("Slide1"), vScale, vRot, vPos)))
			return E_FAIL;

		// 슬라이딩2
		vScale = _float3(5.f, 10.f, 28.f); vRot = _float3(0.f, -168.f, 0.f); vPos = _float3(-1252.672f, 191.413f, 362.253f);
		if (FAILED(Add_Collider(SLIDE_COLLIDER, _string("Slide2"), vScale, vRot, vPos)))
			return E_FAIL;
	}

	{
		// 엔딩
		vScale = _float3(5.f, 10.f, 70.f); vRot = _float3(0.f, 59.f, 0.f); vPos = _float3(-1404.000f, 227.002f, 564.000f);
		if (FAILED(Add_Collider(ENDING_COLLIDER, _string("Ending"), vScale, vRot, vPos)))
			return E_FAIL;
		
	}

#ifdef _DEBUG
	{
		CCollider::COLLIDERDESC tColliderDesc;
		ZeroMemory(&tColliderDesc, sizeof(CCollider::COLLIDERDESC));
		tColliderDesc.vScale = _float3{ 0.5f, 0.5f, 0.5f };
		tColliderDesc.vTranslation = _float3{ 0.f, 0.f, 0.f };
		m_TmpCollider = (CCollider*)CGameInstance::Get_Instance()->Clone_Component(LEVEL_STATIC, PROTO_COMP_OBBCOLLIDER, &tColliderDesc);
		if (nullptr == m_TmpCollider)
			return E_FAIL;
	}
#endif // _DEBUG

	return S_OK;
}

HRESULT CMapCollider::Add_Collider(MAPCOLLIDERTYPE eType, _string strColliderTag, _float3 vScale, _float3 vRot, _float3 vPos)
{
	if (nullptr != Find_Collider(eType, strColliderTag))
	{
		MSG_BOX("Failed To Add MapCollider : Already Exist");
		return E_FAIL;
	}

	CCollider::COLLIDERDESC tColliderDesc;
	ZeroMemory(&tColliderDesc, sizeof(CCollider::COLLIDERDESC));
	tColliderDesc.vScale = _float3{ 0.5f, 0.5f, 0.5f };
	tColliderDesc.vTranslation = _float3{ 0.f, 0.f, 0.f };
	CCollider* pCollider = (CCollider*)CGameInstance::Get_Instance()->Clone_Component(LEVEL_STATIC, PROTO_COMP_OBBCOLLIDER, &tColliderDesc);
	if (nullptr == pCollider)
		return E_FAIL;

	_matrix TranslateMat = XMMatrixScalingFromVector(vScale.Get_Vector()) * 
							XMMatrixRotationRollPitchYaw(XMConvertToRadians(vRot.x), XMConvertToRadians(vRot.y), XMConvertToRadians(vRot.z)) *
							XMMatrixTranslationFromVector(vPos.Get_Vector());
	pCollider->Tick(TranslateMat);
	m_MapCollider[eType].emplace(strColliderTag, pCollider);

	return S_OK;
}

CCollider* CMapCollider::Find_Collider(MAPCOLLIDERTYPE eType, _string strColliderTag)
{
	for (auto& iter : m_MapCollider[eType])
	{
		if (strColliderTag == iter.first)
		{
			return iter.second;
		}
	}
	return nullptr;
}

CMapCollider* CMapCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapCollider* pInstance = new CMapCollider(pDevice, pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CMapCollider::Create To Failed");
#endif
	}
	return pInstance;
}

CGameObject* CMapCollider::Clone(void* _pArg)
{
	CMapCollider* pInstance = new CMapCollider(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CMapCollider::Clone To Failed");
#endif
	}
	return pInstance;
}

void CMapCollider::Free()
{
	__super::Free();

	Safe_Release(m_pStoryPlayer);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_TmpCollider);

	for (_uint i = 0; i < COLLIDER_END; i++)
	{
		for (auto& iter : m_MapCollider[i])
			Safe_Release(iter.second);
		m_MapCollider[i].clear();
	}

#ifdef _DEBUG
	Safe_Release(m_TmpCollider);
#endif // _DEBUG

}