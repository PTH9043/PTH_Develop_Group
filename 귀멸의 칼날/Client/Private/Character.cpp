#include "Character.h"
#include "GameInstance.h"
#include "BonesCollider.h"
#include "Controller.h"
#include "Animator.h"
#include "Battle_Manager.h"
#include "Sword.h"
#include "AnimationClip.h"
#include "Camera_Manager.h"
#include "Camera.h"
#include "Camera_Free.h"

#include "MainView.h"
#include "ImGuiClass_Manager.h"
#include "CutSceneModul.h"
#include "CutScene_Manager.h"


CCharacter::CCharacter(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CPawn(_pDevice, _pContext)
{
}

CCharacter::CCharacter(const CCharacter& _rhs)
	: CPawn(_rhs)
{
}

void CCharacter::Set_Navigation(const _tchar* pNavigationTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		CNavigation* pNavigation = (CNavigation*)pGameInstance->Clone_Component(g_iCurrentLevel, pNavigationTag);
		if (nullptr == pNavigation)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
		Safe_Release(m_pNavigationCom);

		m_pNavigationCom = pNavigation;
		m_pTransformCom->Update_Nav(m_pNavigationCom);
	}
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CCharacter::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_ePawnType = PAWN_CHARACTER;
	return S_OK;
}

HRESULT CCharacter::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;
	// Load Bone Data
	Load_BonesData();
	// Update NavigationCom
	if (nullptr != m_pNavigationCom)
	{
		m_pTransformCom->Update_Nav(m_pNavigationCom);
		m_pNavigationCom->Find_Cell(m_pTransformCom->Get_Pos());
	}
	// 사운드 등록
	Initialize_Sound();

	

	return S_OK;
}

void CCharacter::Tick(const _double& _dTimeDelta)
{
	if (false == m_bActiveCutScene)
	{
		Coll_Init();

		if (nullptr != m_pAnimAttackCollider)
		{
			// Animator에서 발생한 콜라이더 이벤트의 값을 받아온다. ( 위치값, 스케일 등 )
			_vector vPos = m_pAnimator->Get_ColliderPos();

			// 위치를 바꾼다.
			m_pAnimAttackCollider->Set_Translate(vPos);
			m_pAnimAttackCollider->Set_Scale(m_pAnimator->Get_ColliderScale());
		}
		// Awake가 true라면 Awake 수치를 줄인다.
		if (true == m_tBattleData.bIsAwake)
			m_tBattleData.Decrease_AwakeValue(0.1f);

		if (false == m_tBattleData.Is_CheckMaxCharge())
		{
			// 차지 타이머의 시간을 돌린다.
			m_tBattleData.tChargeTimer.Is_Over(_dTimeDelta);

			if (true == m_tBattleData.tChargeTimer.bPass)
			{
				// 차지중일때와 아닐떄의 차지 속도를 다르게한다.
				if ("CHARGE" == m_strAnimType)
					m_tBattleData.Increase_Charge(0.5f);
				else
					m_tBattleData.Increase_Charge(0.1f);
			}
		}
		// 게이지가 가득 찼는데 Charge중이었다면 CHARGE_END 명령을 내려준다.
		else
		{
			if ("CHARGE" == m_strAnimType)
				Input_Trigger("CHARGE_END");
		}

		// 히트 카운트가 0보다 크다면 체크한다.
		if (0 < m_tBattleData.iHitCount)
		{
			if (true == m_tBattleData.tComboTimer.Is_Over(_dTimeDelta))
			{
				// 콤보 타이머가 넘으면 reset시킨다.
				m_tBattleData.tComboTimer.Reset_Timer();
				// 히트 카운트를 0으로 초기화한다.
				m_tBattleData.iHitCount = 0;
			}
		}
		else if (2 > m_tBattleData.iHitCount)
		{
			m_tBattleData.bIsOverHit = false;
		}

		// 밀치기 공격에 맞았다면 밀쳐진다.
		Hit_PushedState(_dTimeDelta);
		// 애니메이션 본 트랜스폼이나, 이벤트 이동값을 받아와 이동시킨다. ( 네비게이션은 경우에 따라 타게 됨 )
		Restrict_Nav(_dTimeDelta);
		if (nullptr != m_pTransformCom)
			Coll_Tick(m_pTransformCom->Get_WorldMatrix());
	}
}

void CCharacter::Late_Tick(const _double& _dTimeDelta)
{
	if (false == m_bActiveCutScene)
	{
		// Late Tick
		__super::Late_Tick(_dTimeDelta);
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		{
			if (true == pGameInstance->Is_Rendering(m_pAttackColliderCom->Get_CurPos(), m_pAttackColliderCom->Get_Scale().x * 1.2f))
			{
				if (nullptr != m_pRendererCom)
				{
#ifdef _DEBUG
					if (pGameInstance->Get_DIKeyPressing(DIK_G))
						m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
					else
						m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

#else
					m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
					//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
#endif
				}
			}
		}
		RELEASE_INSTANCE(CGameInstance);

		if (nullptr != m_pTransformCom)
			m_vPreviousPosition = m_pTransformCom->Get_Pos();

		// 네비게이션 컴포넌트 있으면 테스트 
		if (nullptr != m_pNavigationCom)
		{
			// 공중에 뜰 경우에 m_bIsAir를 True로 바꾼다. ( False는 다른곳에서 해줘야함 )
			if (m_pTransformCom->Get_PosF3().y > m_pNavigationCom->Compute_Height(m_pTransformCom->Get_Pos()) + 1.f)
				m_bIsAir = true;
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
			//        m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
#endif
		}
	}
}

HRESULT CCharacter::Render()
{
	// 모델 Glow
	if (CGameInstance::Get_Instance()->Get_DIKeyPressing(DIK_G))
	{
		if (FAILED(SetUp_ConstantTable()))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_vGlowColor", &m_vGlowColor, sizeof(_float4))))
			return E_FAIL;

		if (nullptr != m_pModelCom)
		{
			_uint iNumMeshContainers = m_pModelCom->Get_NumMeshContainers();

			for (_uint i = 0; i < iNumMeshContainers; ++i)
			{
				m_pModelCom->Render(i, m_pShaderCom, 3);
			}
		}
	}
	// 기본 Render
	else
	{
		if (FAILED(__super::Render()))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_vEdgeGlowColor", &m_vEdgeGlowColor, sizeof(_float4))))
			return E_FAIL;

		if (nullptr != m_pModelCom)
		{
			_uint iNumMeshContainers = m_pModelCom->Get_NumMeshContainers();

			if (true == m_pAnimator->Is_MeshActive())
			{
				for (_uint i = 0; i < iNumMeshContainers; ++i)
				{

					if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TEXTYPE::TextureType_DIFFUSE)))
						return E_FAIL;

					m_pModelCom->Bind_SRV(m_pShaderCom, "g_OutLineTexture", i, TextureType_SPECULAR);

					m_pModelCom->Render(i, m_pShaderCom, 0);
				}
			}
		}
	}


	return S_OK;
}

HRESULT CCharacter::RenderLight()
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

	if (nullptr != m_pModelCom)
	{
		_uint iNumMeshContainers = m_pModelCom->Get_NumMeshContainers();

		for (_uint i = 0; i < iNumMeshContainers; ++i)
		{
			if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TEXTYPE::TextureType_DIFFUSE)))
				return E_FAIL;

			m_pModelCom->Render(i, m_pShaderCom, 2);
		}
	}

	return S_OK;
}

void CCharacter::Extern_ImGuiClass()
{

	ImGui::DragFloat4("GlowColor", &m_vGlowColor.x, 0.001f, 0.f, 1.f);
	ImGui::DragFloat4("EdgeGlowColor", &m_vEdgeGlowColor.x, 0.001f, 0.f, 1.f);

	_bool bIsPass = m_tBattleData.tComboTimer.bPass;
	ImGui::Checkbox("ComboTimer Progress", &bIsPass);
	_string strComboTimer = "Combo Timer : ";
	strComboTimer += CMethod::Convert_Number_S((_double)m_tBattleData.tComboTimer.fTimer, 2);
	ImGui::Text(strComboTimer.c_str());
	_string strHitCount = "Hit Count : ";
	strHitCount += CMethod::Convert_Number_S(m_tBattleData.iHitCount);
	ImGui::Text(strHitCount.c_str());
}

void CCharacter::Start_Scene(CCharacter* _Enemy)
{
	if (nullptr == _Enemy)
		return;

	for (auto& iter : m_CutSceneModulMap)
		iter.second->Start_CutScene(this, _Enemy);
}

_float CCharacter::OtherCharacterToDistance(CGameObject* _pOther)
{
	if (nullptr == _pOther || nullptr == m_pTransformCom)
		return 0.f;

	_float fDistance = 0.f;
	CTransform* _pTransform = static_cast<CTransform*>(_pOther->Get_Transform());
	if (nullptr == _pTransform)
		return 0.f;

	fDistance = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_Pos()
		- _pTransform->Get_Pos()));

	return fDistance;
}

_float CCharacter::OtherCharacterDirToLook(CGameObject* _pOther)
{
	_float fDot = 0.f;
	if (nullptr == _pOther)
		return fDot;

	CTransform* _pTransform = static_cast<CTransform*>(_pOther->Get_Transform());
	if (nullptr == _pTransform)
		return 0.f;

	_float3 v3Look;
	v3Look.Set_Vector(_pTransform->Get_Pos() - m_pTransformCom->Get_Pos());
	fDot = v3Look.Dot(m_pTransformCom->Get_Look());
	return fDot;
}

_float CCharacter::OhterCharacterDirToRight(CGameObject* _pOther)
{
	_float fDot = 0.f;

	if (nullptr == _pOther)
		return fDot;

	CTransform* _pTransform = static_cast<CTransform*>(_pOther->Get_Transform());
	if (nullptr == _pTransform)
		return 0.f;

	_float3 v3Look;
	v3Look.Set_Vector(_pTransform->Get_Pos() - m_pTransformCom->Get_Pos());
	fDot = v3Look.Dot(m_pTransformCom->Get_Right());
	return fDot;
}

_float CCharacter::OtherCharacterDirToLookConverter(CGameObject* _pOther)
{
	_float fDot = XMConvertToDegrees(acosf(OtherCharacterDirToLook(_pOther)));
	_float fValue = OhterCharacterDirToRight(_pOther);

	if (fValue < 0)
		fDot = -fDot;

	return fDot;
}

_float3 CCharacter::OtherCharacterDirToLookVectorF3(CGameObject* _pOther)
{
	_float3 v3Pos = _float3(0.f, 0.f, 0.f);

	if (nullptr == _pOther)
		return v3Pos;

	CTransform* _pTransform = static_cast<CTransform*>(_pOther->Get_Transform());
	if (nullptr == _pTransform)
		return v3Pos;

	v3Pos = _pTransform->Get_Pos() - m_pTransformCom->Get_Pos();
	return v3Pos.Normalize();
}

_float CCharacter::OtherCharacterToDistance(CTransform* _pOther)
{
	if (nullptr == _pOther || nullptr == m_pTransformCom)
		return 0.f;

	if (nullptr == _pOther)
		return 0.f;

	_float fDistance = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_Pos()
		- _pOther->Get_Pos()));

	return fDistance;
}

_float CCharacter::OtherCharacterDirToLook(CTransform* _pOther)
{
	_float fDot = 0.f;
	if (nullptr == _pOther)
		return fDot;

	if (nullptr == _pOther)
		return 0.f;

	_float3 v3Look;
	v3Look.Set_Vector(_pOther->Get_Pos() - m_pTransformCom->Get_Pos());
	fDot = v3Look.Dot(m_pTransformCom->Get_Look());
	return fDot;
}

_float CCharacter::OhterCharacterDirToRight(CTransform* _pOther)
{
	_float fDot = 0.f;

	if (nullptr == _pOther)
		return fDot;

	if (nullptr == _pOther)
		return 0.f;

	_float3 v3Look;
	v3Look.Set_Vector(_pOther->Get_Pos() - m_pTransformCom->Get_Pos());
	fDot = v3Look.Dot(m_pTransformCom->Get_Right());
	return fDot;
}

_float CCharacter::OtherCharacterDirToLookConverter(CTransform* _pOther)
{
	_float fDot = XMConvertToDegrees(acosf(OtherCharacterDirToLook(_pOther)));
	_float fValue = OhterCharacterDirToRight(_pOther);

	if (fValue < 0)
		fDot = -fDot;

	return fDot;
}

_float3 CCharacter::OtherCharacterDirToLookVectorF3(CTransform* _pOther)
{
	_float3 v3Pos = _float3(0.f, 0.f, 0.f);

	if (nullptr == _pOther)
		return v3Pos;

	if (nullptr == _pOther)
		return v3Pos;

	v3Pos = _pOther->Get_Pos() - m_pTransformCom->Get_Pos();
	return v3Pos.Normalize();
}

HRESULT CCharacter::Add_CutScene(FILEDATA* _pFileData)
{
	CCutSceneModul* pInstance = CCutSceneModul::Create(m_pDevice, m_pContext);

	if (nullptr != pInstance)
	{
		if (FAILED(pInstance->Load_Data(_pFileData)))
		{
			Safe_Release(pInstance);
			return E_FAIL;
		}

		_string str = pInstance->Get_InputTrigger();
		m_CutSceneModulMap.emplace(pair<_string, CCutSceneModul*>(str, pInstance));
	}

	return S_OK;
}

_uint CCharacter::Judge_Direction(const _float& _fDot)
{
	_uint eDir = 0;
	// Forward
	if (20.f >= _fDot && _fDot >= -20.f)
		eDir = DIR_FORWARD;
	// Left Forward
	else if (_fDot >= 20.f && _fDot <= 80.f)
		eDir = DIR_FORELEFT | DIR_LEFT;
	else if (_fDot >= 80.f && _fDot <= 120.f)
		eDir = DIR_LEFT;
	else if (_fDot >= 120.f && _fDot <= 160.f)
		eDir = DIR_BACKLEFT | DIR_LEFT;
	else if (_fDot >= 160.f && _fDot <= -160.f)
		eDir = DIR_BACK;
	else if (_fDot >= -160.f && _fDot <= -120.f)
		eDir = DIR_BACKRIGHT | DIR_RIGHT;
	else if (_fDot >= -120.f && _fDot <= -80.f)
		eDir = DIR_RIGHT;
	else if (_fDot >= -80.f && _fDot <= -20.f)
		eDir = DIR_FORRIGHT | DIR_RIGHT;

	if (isnan(_fDot))
		eDir = DIR_FORWARD;

	return eDir;
}

_bool CCharacter::Is_JudgeFallState()
{
	if (m_fPrevJumpHeight >= m_fCurrentJumpHeight)
		return true;
	return false;
}

_bool CCharacter::Is_JudgeMoveState()
{
	_float3 vMovePos = m_pTransformCom->Get_PosF3() - m_vPreviousPosition;
	if (!CMethod::Is_Vector_Zero(vMovePos.Get_Vector()))
		return true;
	return false;
}

void CCharacter::Push(CCharacter* _pOther)
{
	_bool bMyMove = Is_JudgeMoveState();
	_bool bOtherMove = _pOther->Is_JudgeMoveState();

	if (Get_BoneCollider().size() < 1 || _pOther->Get_BoneCollider().size() < 1)
		return;

	CBonesCollider* pMyCollider = Get_BoneCollider()[0];
	CBonesCollider* pOtherCollider = _pOther->Get_BoneCollider()[0];
	_float fDistance = 0.f;
	// 충돌 했는지 판단해준다. 충돌시 충돌된 거리 반환
	if (!pMyCollider->Compute_Distance_Sphere(pOtherCollider, fDistance))
		return;

	_vector vnOtherToMy = XMVector3Normalize(XMVectorSetY(pMyCollider->Get_Collider()->Get_CurPos() - pOtherCollider->Get_Collider()->Get_CurPos(), 0.f));
	_vector vPushPos = XMVectorZero();
	// 상대방이 움직일때만 밀린다.
	if (bOtherMove)
	{
		if (bMyMove)
			vPushPos = vnOtherToMy * fDistance * 0.5f;
		else
			vPushPos = vnOtherToMy * fDistance;
	}
	// 손오니와 싸울때 플레이어는 무조건 밀린다.
	if (LEVEL_GAMEPLAY == g_iCurrentLevel)
		vPushPos = vnOtherToMy * fDistance;

	_vector vTargetPos = m_pTransformCom->Get_Pos() + vPushPos;

	_float3 vSlideDir = m_pNavigationCom->Compute_SlideVector(m_pTransformCom->Get_Pos(), vTargetPos);

	if (0.f != vSlideDir.x || 0.f != vSlideDir.y || 0.f != vSlideDir.z)
	{
		if (bMyMove)
			vPushPos = vSlideDir.Get_Vector() * fDistance * 0.5f;
		else
			vPushPos = vSlideDir.Get_Vector() * fDistance;

		// 손오니와 싸울때 플레이어는 무조건 밀린다.
		if (LEVEL_GAMEPLAY == g_iCurrentLevel)
			vPushPos = vSlideDir.Get_Vector() * fDistance;
		vTargetPos = m_pTransformCom->Get_Pos() + vPushPos;
	}
	m_pTransformCom->Move_Pos_NotNavigateY(vTargetPos);

	Coll_Tick(m_pTransformCom->Get_WorldMatrix());
}

void CCharacter::Check_AttackCollision(CCharacter* _pOther)
{
	// 상태 체크를하여 콜라이더 체크를 할지 말지 결정한다.
	if (false == Check_Enemy_State(_pOther))
	{
		// 현재 Enemy의 상태가 공격에 맞을 수 없는 상태인데 대쉬를 사용했다면 대쉬를 끝낸다.
		if ("DASH" == m_strAnimType || "AIR_DASH" == m_strAnimType)
			m_pAnimator->Input_Trigger("SUCCESS_DASH");

		return;
	}

	CCollider* pOtherHitCollider = static_cast<CCollider*>(_pOther->Get_Component(COM_COLLIDER_HIT));
	if (Is_Nullptr(pOtherHitCollider))
		return;
	{
		// 내가 때릴수 있고, 상대방이 맞을 수 있는지 확인        
		if (true == m_pAnimator->Is_AttackColliderActive() && false == _pOther->m_bIsHit)
		{
			// 공격 성공 시 true
			if (m_pAnimAttackCollider->Is_Collision(pOtherHitCollider))
			{
				Success_AnimAttack(_pOther);
			}
		}
		// 공격 콜라이더가 꺼져야 상대가 다시 맞을 수 있는 상태로 변경
		else if (false == m_pAnimator->Is_AttackColliderActive())
		{
			_pOther->m_bIsHit = false;
		}
	}
}

void CCharacter::Hit_ByProjectile(_float _fPushForce, _float _fHeight, _float _fDamage, _float _fDelayTime, CCharacter* _pEnemyCharcter)
{
	// 맞았다는것을 체크해준다.
	m_bIsHit = true;

	// 때린 대상의 애니메이터를 얻어온다.
	CAnimator* pEnemyAnimator = _pEnemyCharcter->Get_Animator();
	CTransform* pEnemyTransform = _pEnemyCharcter->Get_Transform();

	_vector vDirToAttackCharcter = pEnemyTransform->Get_Pos() - m_pTransformCom->Get_Pos();
	// 내적
	_vector vDot = XMVector3Dot(XMVector3Normalize(m_pTransformCom->Get_Look()), XMVector3Normalize(vDirToAttackCharcter));
	// Degree값으로 구한다.
	_float fDegree = CMethod::Compute_Degree_XZ_Plane(m_pTransformCom->Get_LookF3(), vDirToAttackCharcter); // XMConvertToDegrees(acosf(XMVectorGetX(vDot)));

	// 인풋이 성공했는지 확인하는 불 변수
	_bool bIsInput = false;

	// 콜라이더의 타입
	_string strColliderType = "HIT_FRONT_S";

	{
		if (-45.f > fDegree && -135.f < fDegree)
			strColliderType = "HIT_LEFT";
		else if (45.f < fDegree && 135.f > fDegree)
			strColliderType = "HIT_RIGHT";
		else if (135.f <= fabs(fDegree))
			strColliderType = "HIT_BACK_S";
		bIsInput = m_pAnimator->Input_Trigger(strColliderType);
	}

	// 트리거의 인풋에 성공했다면 다음 애니메이션 ( 피격 ) 이 가진 넉백 정보를 가져온다.
	if (true == bIsInput)
	{
		// 공격한 객체의 히트 카운트를 늘린다.
		_pEnemyCharcter->m_tBattleData.iHitCount += 1;
		// 콤보 타이머를 0으로 초기화한다.
		_pEnemyCharcter->m_tBattleData.tComboTimer.Reset_Timer();

		m_tBattleData.Decrease_HP(_fDamage);

		// 인풋에 성공했는데 Upper나 Strike등의 공중에 뜨는 애니메이션이 아니었다면 DEAD 트리거를 넣는다.
		// TODO: 나중에 죽을경우에는 시간을 좀 오래 멈추거나 클라이언트 속도를 줄이는등의 작업이 필요하다.
		if (true == m_tBattleData.Is_Die())
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			// 막타 사운드를 재생한다.
			pGameInstance->Play_SoundW(TEXT("Makta"));

			RELEASE_INSTANCE(CGameInstance);

			if (true == m_pAnimator->Input_Trigger("DEAD"))
				return;
		}

		// 공격한 객체의 히트 카운트가 2가 넘거나 같다면 맞은 객체의 Look을 때린 객체로 보게만든다.
		if (2 <= _pEnemyCharcter->m_tBattleData.iHitCount || "STRIKE" == strColliderType || "HIT_UPPER" == strColliderType || "HIT_DOWN" == strColliderType
			|| "SPIN" == strColliderType || "GUARD_NORMAL" == strColliderType || "GUARD_STRONG" == strColliderType)
		{
			// 자신으로부터 때린 객체로 향하는 Look 벡터를 만든다.
			_vector vLook = pEnemyTransform->Get_Pos() - m_pTransformCom->Get_Pos();
			vLook = XMVectorSetY(vLook, 0.f);

			// 상대방을 바라본다.
			m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + vLook);
		}

		// 히트 이펙트를 생성한다.
		Hit_Effect(strColliderType, _pEnemyCharcter);

		// 배틀매니저로 양쪽 캐릭터의 시간을 멈춘다.
		CBattle_Manager::Get_Instance()->Set_DelayTime(_fDelayTime);

		// 궁극기 게이지를 채운다.
		m_tBattleData.Increase_Ultimate(4.f);

		// 히트도 들어오고 인풋도 성공했다면 FallTimeAcc를 항상 0.0으로 초기화시키자.
		m_dFallTimeAcc = 0.0;

		CCharacter::KNOCKBACK knockBack;
		ZeroMemory(&knockBack, sizeof(CCharacter::KNOCKBACK));
		// 때린 객체의 애니메이션이 가진 PushedForce를 받아옵니다.
		knockBack.fPushedForce = _fPushForce;//pEnemyAnimator->Get_PushedForce();
		// 때린 객체의 애니메이션이 가진 Height를 받아옵니다.
		knockBack.fHeight = _fHeight;//pEnemyAnimator->Get_Height();
	// 피격 애니메이션의 이벤트 적용 시간을 받아옵니다.
		if (nullptr != m_pAnimator->Get_NextAnimationClip())
			knockBack.dPlayTime = m_pAnimator->Get_NextAnimationClip()->Get_EventPlayTime();
		// 떄린 객체 -> 맞은 객체로 향하는 방향벡터
		knockBack.vAttackDir = XMVector3Normalize(m_pTransformCom->Get_Pos() - pEnemyTransform->Get_Pos());
		// y값을 제외시킨다.
		knockBack.vAttackDir.y = 0;

		// 넉백 정보를 넣어준다.
		m_tKnockBackInfo = knockBack;
	}

}

_bool CCharacter::Check_Enemy_State(CCharacter* _pEnemyCharcter)
{
	if (nullptr == _pEnemyCharcter)
		return false;

	_string strEnemyType = _pEnemyCharcter->Get_Animator()->Get_AnimationType();

	// Down 상태이거나 Roll 상태 , Dead 상태일때는 피격에 맞지 않는다.
	if ("DOWN" == strEnemyType || "DIE" == strEnemyType || "ROLL" == strEnemyType || "PASSIVE" == strEnemyType)
		return false;

	return true;
}

void CCharacter::Success_AnimAttack(CCharacter* _pOther)
{
	_string strColliderType = m_pAnimator->Get_CurrentAnimationClip()->Get_ColliderType();

	CBattle_Manager* pInstance = GET_INSTANCE(CBattle_Manager);
	// 밀치기 공격이었는지 판단 후 , 리턴한다.
	if ("PUSH" == m_pAnimator->Get_CurrentAnimationClip()->Get_ColliderType())
	{
		_pOther->Hit_Pushed(XMVector3Normalize(_pOther->Get_Transform()->Get_Pos() - m_pTransformCom->Get_Pos()));
		RELEASE_INSTANCE(CBattle_Manager);
		return;
	}

	// 대쉬 상태였다면 상대방도 대쉬였는지 체크하고 대쉬끼리 부딫히는 애니메이션을 재생시킨다.
	// 대쉬였다면 성공했다는 메세지를 넘겨준다.
	if ("DASH" == m_strAnimType || "AIR_DASH" == m_strAnimType)
		Success_Dash(_pOther, pInstance);
	else
		// 궁극기 게이지를 채운다.
		m_tBattleData.Increase_Ultimate(2.5f);

	_pOther->Hit_ByEnemy(this, pInstance);

	// 공격 성공타입을 검사하여 성공 함수를 호출한다.
	Check_Attack_Type(strColliderType);

	_vector vHitInformation = m_pAnimator->Get_ColliderDir();

	_float fDamage = XMVectorGetX(vHitInformation);
	_float fDelay = XMVectorGetY(vHitInformation);
	_float fShakeTime = XMVectorGetZ(vHitInformation);
	_float fShakePower = XMVectorGetW(vHitInformation);

	// 쉐이크 타임이 0이 아니라면 카메라의 쉐이크를 준다.
	if (0.f < fShakeTime)
	{
		CCamera_Manager* pCamManager = GET_INSTANCE(CCamera_Manager);

		if (nullptr != pCamManager)
		{
			CCamera* pCamera = pCamManager->Get_ActiveCamera();
			if (nullptr != pCamera)
			{
				static_cast<CCamera_Free*>(pCamera)->Start_CameraShake(fShakeTime, fShakePower);
			}
		}
		RELEASE_INSTANCE(CCamera_Manager);
	}
	RELEASE_INSTANCE(CBattle_Manager);
}

void CCharacter::Hit_ByEnemy(CCharacter* _pEnemyCharcter, CBattle_Manager* _pBattleManager)
{
	// 대쉬 반사 도중이면 다른 애니메이션을 재생시키지 않는다.
	if (false == m_bIsSuccessDash && m_strCurrentTrigger == "SUCCESS_DASH")
		return;

	// 혹시라도 체크가 안된 경우를 대비하여 현재 나의 타입이 대쉬였다면 SUCCESS_DASH로 만들어준다.
	if ("DASH" == m_strAnimType)
	{
		Input_Trigger("SUCCESS_DASH");
		m_bIsSuccessDash = false;

		_float3 vDir = XMVector3Normalize(m_pTransformCom->Get_Pos() - _pEnemyCharcter->Get_Transform()->Get_Pos());
		vDir.y = 0.f;

		m_vDashReflectDir = vDir;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		pGameInstance->Play_SoundW(TEXT("Hit_S_Sound01"), 0.3f);

		RELEASE_INSTANCE(CGameInstance);

		return;
	}

	// 맞았다는것을 체크해준다.
	m_bIsHit = true;

	// 맞을때마다 FallTimeAcc를 0으로 초기화해준다.
	m_dFallTimeAcc = 0.0;

	// 때린 대상의 애니메이터를 얻어온다.
	CAnimator* pEnemyAnimator = _pEnemyCharcter->Get_Animator();
	CTransform* pEnemyTransform = _pEnemyCharcter->Get_Transform();

	// Collider의 Dir값이 갖고있는 정보를 가져온다. ( x = 데미지 , y = 멈추는 시간, z = 아직 안씀( 추후에 추가 예정 ) )
	_vector vHitInformation = pEnemyAnimator->Get_ColliderDir();

	_float fDamage = XMVectorGetX(vHitInformation);
	_float fDelay = XMVectorGetY(vHitInformation);
	_float fShakeTime = XMVectorGetZ(vHitInformation);
	_float fShakePower = XMVectorGetW(vHitInformation);

	_vector vDirToAttackCharcter = pEnemyTransform->Get_Pos() - m_pTransformCom->Get_Pos();
	// 내적
	_vector vDot = XMVector3Dot(XMVector3Normalize(m_pTransformCom->Get_Look()), XMVector3Normalize(vDirToAttackCharcter));
	// Degree값으로 구한다.
	_float fDegree = XMConvertToDegrees(acosf(XMVectorGetX(vDot)));

	// 인풋이 성공했는지 확인하는 불 변수
	_bool bIsInput = false;

	// 콜라이더의 타입
	_string strColliderType = "";
	_string strSuccessTrigger = "";

	if (nullptr != pEnemyAnimator->Get_CurrentAnimationClip())
		strColliderType = pEnemyAnimator->Get_CurrentAnimationClip()->Get_ColliderType();

	// 현재 맞는 객체가 슈퍼아머 상태인지 판단합니다.
	if (true == Hit_SuperArmor())
	{
		// 데미지가 0보다 큰 경우에만 콤보 타이머를 초기화한다.
		if (fDamage > 0)
		{
			// 공격한 객체의 히트 카운트를 늘린다.
			_pEnemyCharcter->m_tBattleData.iHitCount += 1;
			// 콤보 타이머를 0으로 초기화한다.
			_pEnemyCharcter->m_tBattleData.tComboTimer.Reset_Timer();
		}

		// 인풋에 성공했는데 Upper나 Strike등의 공중에 뜨는 애니메이션이 아니었다면 DEAD 트리거를 넣는다.
		if (true == m_tBattleData.Is_Die())
		{
			if (true == m_pAnimator->Input_Trigger("DEAD"))
				return;
		}
		// 히트 이펙트를 생성한다.
		Hit_Effect(strColliderType, _pEnemyCharcter);

		// 궁극기 게이지를 채운다.
		m_tBattleData.Increase_Ultimate(4.f);

		// 실행 중이던 애니메이션을 그대로 진행시킨다.
		return;
	}

	// 애니메이션 타입이 가드라면 공격을 받지 않는다.
	if ("GUARD" == m_strAnimType)
	{
		// 미는거리가 10보다 길거나 같다면 미는 동작에 S를 붙여준다.
		if (10 <= pEnemyAnimator->Get_PushedForce())
		{
			strColliderType = "GUARD_STRONG";
			bIsInput = m_pAnimator->Input_Trigger(strColliderType);
		}
		else
		{
			strColliderType = "GUARD_NORMAL";
			bIsInput = m_pAnimator->Input_Trigger(strColliderType);
		}
	}

	// 공격의 방향으로 어느 애니메이션을 재생시킬지 판단한다.
	// 등뒤에서 공격 받았다면 뒤에서 맞는 애니메이션을 재생시킨다.
	if ("HIT_UPPER" == strColliderType || "HIT_DOWN" == strColliderType)
	{
		bIsInput = m_pAnimator->Input_Trigger(strColliderType);
	}
	// Strike에 맞으면 STRIKE FRONT로 무조건 떨어진다. => 맞은 대상은 때린 객체를 바라보게 됨.
	else if ("STRIKE" == strColliderType)
	{
		bIsInput = m_pAnimator->Input_Trigger(strColliderType);
	}
	else if ("SPIN" == strColliderType)
	{
		bIsInput = m_pAnimator->Input_Trigger(strColliderType);
	}
	else if ("BLOW" == strColliderType)
	{
		// 뒤쪽에있다면 뒤쪽에서 맞는 애니메이션 실행
		if (90.f <= fDegree)
			bIsInput = m_pAnimator->Input_Trigger("BLOW_BACK");
		// 앞쪽에서 맞으면 받아온 타입에 맞는 애니메이션 실행
		else
			bIsInput = m_pAnimator->Input_Trigger(strColliderType);
	}

	// 아직 UPPER나 DOWN의 상태처리가 되지 않은 경우
	if (false == bIsInput)
	{
		// 맞은 객체가 하늘에 떠있는 경우에는 공중에서 맞는 애니메이션을 실행시킨다.
		if (true == m_bIsAir)
		{
			// 하늘에 떠있다면 받아온 타입에 AIR를 붙인다.
			strColliderType += "_AIR";

			// 뒤쪽에있다면 뒤쪽에서 맞는 애니메이션 실행
			if (90.f <= fDegree)
			{
				strColliderType = "HIT_BACK_AIR";
				bIsInput = m_pAnimator->Input_Trigger(strColliderType);
			}
			// 앞쪽에서 맞으면 받아온 타입에 맞는 애니메이션 실행
			else
				bIsInput = m_pAnimator->Input_Trigger(strColliderType);
		}
		// 맞은 객체가 하늘에 떠있지 않다면 땅에서 맞는 애니메이션 실행
		else
		{
			// 뒤쪽에서 맞았는지 판단해준다.
			if (90.f <= fDegree)
			{
				if (2 <= _pEnemyCharcter->m_tBattleData.iHitCount + 1)
					strColliderType = "HIT_FRONT";
				else
					strColliderType = "HIT_BACK";
			}
			// 미는거리가 10보다 길거나 같다면 미는 동작에 S를 붙여준다.
			if (10 <= pEnemyAnimator->Get_PushedForce())
			{
				// 뒤쪽에서 맞았는지 판단해준다.
				if (90.f <= fDegree)
				{
					if (2 <= _pEnemyCharcter->m_tBattleData.iHitCount + 1)
						strColliderType = "HIT_FRONT";
					else
						strColliderType = "HIT_BACK";
				}
				else
					strColliderType = "HIT_FRONT";

				strColliderType += "_S";
			}

			bIsInput = m_pAnimator->Input_Trigger(strColliderType);
		}
	}

	// 트리거의 인풋에 성공했다면 다음 애니메이션 ( 피격 ) 이 가진 넉백 정보를 가져온다.
	if (true == bIsInput)
	{
		// 데미지가 0보다 큰 경우거나 가드 상태가 아닐 때만 콤보 타이머를 초기화한다.
		if (fDamage > 0.f && "GUARD" != m_strAnimType)
		{
			// 공격한 객체의 히트 카운트를 늘린다.
			_pEnemyCharcter->m_tBattleData.iHitCount += 1;
			// 콤보 타이머를 0으로 초기화한다.
			_pEnemyCharcter->m_tBattleData.tComboTimer.Reset_Timer();
		}

		// 가드 상태였다면 대미지를 받지 않는다.
		if ("GUARD" == m_strAnimType)
		{

		}
		else
		{
			// 데미지를 준다.
			m_tBattleData.Decrease_HP(fDamage);

			// 인풋에 성공했는데 Upper나 Strike등의 공중에 뜨는 애니메이션이 아니었다면 DEAD 트리거를 넣는다.
			if ("HIT_UPPER" != strColliderType && "BLOW" != strColliderType && "STRIKE" != strColliderType &&
				"SPIN" != strColliderType && "HIT_DOWN" != strColliderType)
			{
				if (true == m_tBattleData.Is_Die())
				{
					CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

					// 막타 사운드를 재생한다.
					pGameInstance->Play_SoundW(TEXT("Makta"));

					RELEASE_INSTANCE(CGameInstance);

					if (true == m_pAnimator->Input_Trigger("DEAD"))
						return;
				}
			}
		}

		// 공격한 객체의 히트 카운트가 2가 넘거나 같다면 맞은 객체의 Look을 때린 객체로 보게만든다.
		if (2 <= _pEnemyCharcter->m_tBattleData.iHitCount || "STRIKE" == strColliderType || "HIT_UPPER" == strColliderType || "HIT_DOWN" == strColliderType
			|| "SPIN" == strColliderType || "GUARD_NORMAL" == strColliderType || "GUARD_STRONG" == strColliderType)
		{
			// 자신으로부터 때린 객체로 향하는 Look 벡터를 만든다.
			_vector vLook = pEnemyTransform->Get_Pos() - m_pTransformCom->Get_Pos();
			vLook = XMVectorSetY(vLook, 0.f);

			// 상대방을 바라본다.
			m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + vLook);

			_pEnemyCharcter->m_tBattleData.bIsOverHit = true;
		}

		// 히트 이펙트를 생성한다.
		Hit_Effect(strColliderType, _pEnemyCharcter);

		if (fDamage > 0)
		{
			// 히트 타입을 구분하여 해야하는 행동들을 설정한다.
			Check_Hit_Type(strColliderType, _pEnemyCharcter);
		}

		// 배틀매니저로 양쪽 캐릭터의 시간을 멈춘다.
		_pBattleManager->Set_DelayTime(fDelay);

		// 궁극기 게이지를 채운다.
		m_tBattleData.Increase_Ultimate(4.f);

		// 히트도 들어오고 인풋도 성공했다면 FallTimeAcc를 항상 0.0으로 초기화시키자.
		m_dFallTimeAcc = 0.0;

		CCharacter::KNOCKBACK knockBack;
		ZeroMemory(&knockBack, sizeof(CCharacter::KNOCKBACK));
		// 때린 객체의 애니메이션이 가진 PushedForce를 받아옵니다.
		knockBack.fPushedForce = pEnemyAnimator->Get_PushedForce();
		// 때린 객체의 애니메이션이 가진 Height를 받아옵니다.
		knockBack.fHeight = pEnemyAnimator->Get_Height();
		// 피격 애니메이션의 이벤트 적용 시간을 받아옵니다.
		if (nullptr != m_pAnimator->Get_NextAnimationClip())
			knockBack.dPlayTime = m_pAnimator->Get_NextAnimationClip()->Get_EventPlayTime();
		// 떄린 객체 -> 맞은 객체로 향하는 방향벡터
		knockBack.vAttackDir = XMVector3Normalize(m_pTransformCom->Get_Pos() - pEnemyTransform->Get_Pos());
		// y값을 제외시킨다.
		knockBack.vAttackDir.y = 0;

		// 넉백 정보를 넣어준다.
		m_tKnockBackInfo = knockBack;
	}
}

void CCharacter::Hit_Effect(_string strColliderType, CCharacter* _pEnemyCharcter)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if ("DASH" == _pEnemyCharcter->m_strAnimType || "AIR_DASH" == _pEnemyCharcter->m_strAnimType)
	{
		_matrix PlayerMatrix = m_pTransformCom->Get_WorldMatrix();
		PlayerMatrix.r[3] += XMVectorSet(0.f, 7.f, 0.f, 0.f);
		pGameInstance->SetEffectStateCall(L"Shock3", PlayerMatrix);
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (CHAR_TYPE::AKAZA == _pEnemyCharcter->m_eCharType)
	{
		_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		//WorldMatrix.r[3] += XMVectorSet(0.f, 0.f, 0.f, 0.f);
		pGameInstance->SetEffectStateCall(L"Aka_Hit1_1", WorldMatrix);
		pGameInstance->SetEffectStateCall(L"Aka_Hit1_2", WorldMatrix);
	}
	else
	{

		BONECOLLIDERS EnemyBoneCollider = _pEnemyCharcter->Get_BoneCollider();
		if (EnemyBoneCollider.size() < 2)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		// 만약 1번 공격 콜라이더와 나의 Hit콜라이더가 안부딧히면 2번 공격 콜라이더 사용
		CBonesCollider* pAttackCollider = EnemyBoneCollider[1];
		CCollider* pMyHitCollider = (CCollider*)Get_Component(COM_COLLIDER_HIT);
		if (nullptr != pMyHitCollider)
		{
			if (false == pMyHitCollider->Is_Collision(pAttackCollider->Get_Collider()) && EnemyBoneCollider.size() >= 3)
			{
				pAttackCollider = EnemyBoneCollider[2];
			}
		}

		_float3 vAttackDir = pAttackCollider->Get_Direction();

		_float fSworldAngleXY = CMethod::Compute_Radian_XY_Plane(XMVectorSet(0.f, 1.f, 0.f, 0.f), vAttackDir);

		_matrix PlayerMatrix = m_pTransformCom->Get_WorldMatrix();
		PlayerMatrix.r[3] += XMVectorSet(0.f, 7.f, 0.f, 0.f);
		_matrix RotationMatrix = XMMatrixRotationZ(fSworldAngleXY);

		_matrix WorldMatrix = RotationMatrix * PlayerMatrix;

		if (("HIT_FRONT" == strColliderType) || ("HIT_FRONT_AIR" == strColliderType) || ("HIT_BACK" == strColliderType) ||
			("HIT_BACK_AIR" == strColliderType) || ("HIT_RIGHT" == strColliderType) || ("HIT_RIGHT_AIR" == strColliderType)
			|| ("HIT_LEFT" == strColliderType) || ("HIT_LEFT_AIR" == strColliderType))
		{
			_uint randomDmg = rand() % 3;
			{
				if (0 == randomDmg)
					pGameInstance->SetEffectStateCall(L"SlashDmg1_1", WorldMatrix);
				else if (1 == randomDmg)
					pGameInstance->SetEffectStateCall(L"SlashDmg1_2", WorldMatrix);
				else if (2 == randomDmg)
					pGameInstance->SetEffectStateCall(L"SlashDmg1_3", WorldMatrix);
			}
		}
		else if (("HIT_FRONT_S" == strColliderType) || ("HIT_BACK_S" == strColliderType) || ("HIT_S" == strColliderType)
			|| ("HIT_UPPER" == strColliderType) || ("STRIKE" == strColliderType) || ("HIT_DOWN" == strColliderType)
			|| ("BLOW" == strColliderType))
		{
			pGameInstance->SetEffectStateCall(L"SlashDmg2_1", WorldMatrix);
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CCharacter::Hit_Pushed(_vector _vDir)
{
	// 밀치기 공격에 맞았다는 것을 체크한다.
	m_bIsHitPush = true;
	m_dPushTimeAcc = 0.0;

	CCharacter::KNOCKBACK knockBack;
	ZeroMemory(&knockBack, sizeof(CCharacter::KNOCKBACK));
	// 때린 객체의 애니메이션이 가진 PushedForce를 받아옵니다.
	knockBack.fPushedForce = 20.f;
	// 때린 객체의 애니메이션이 가진 Height를 받아옵니다.
	knockBack.fHeight = 0.f;
	// 밀릴 시간을 넣는다.
	knockBack.dPlayTime = 0.1;
	// 떄린 객체 -> 맞은 객체로 향하는 방향벡터
	knockBack.vAttackDir = _vDir;
	// y값을 제외시킨다.
	knockBack.vAttackDir.y = 0;

	// 넉백 정보를 넣어준다.
	m_tKnockBackInfo = knockBack;
}

_bool CCharacter::Hit_SuperArmor()
{
	// 슈퍼아머가 적용되는 경우에 맞아도 경직을 받지 않고 데미지만 받는다.
	if ("SKILL_1" == m_strAnimType || "SKILL_1_AIR" == m_strAnimType || "SKILL_2" == m_strAnimType || "SKILL_2_AIR" == m_strAnimType
		|| "SKILL_3" == m_strAnimType || "SKILL_3_AIR" == m_strAnimType || "CHARGE_ATTACK" == m_strAnimType)
		return true;

	return false;
}

HRESULT CCharacter::SetUp_Animator(const _tchar* _pName, CModel* _pModel)
{
	if (nullptr == _pModel)
		return E_FAIL;

	CAnimator::ANIMATORDESC tDesc;
	tDesc.pModel = _pModel;
	tDesc.strFilePath = _pName;
	tDesc.pTransform = m_pTransformCom;

	if (FAILED(Add_Component(COM_ANIMATOR, LEVEL_STATIC,
		PROTO_COMP_ANIMATOR, (CComponent**)&m_pAnimator, &tDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCharacter::SetUp_ConstantTable()
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	CBattle_Manager* pBattleManager = GET_INSTANCE(CBattle_Manager);
	CCutScene_Manager* pManager = GET_INSTANCE(CCutScene_Manager);
	{
		m_bObjectMotionBlur = m_bMotionBlur | pManager->Is_PlayCutScene() /*| pBattleManager->Is_MotionBlurOn()*/;
	}
	RELEASE_INSTANCE(CCutScene_Manager);
	RELEASE_INSTANCE(CBattle_Manager);
	SETUP_CONSTANCE_VALUE(m_pShaderCom, "g_ObjectMotionBlur", m_bObjectMotionBlur);

	return S_OK;
}

HRESULT CCharacter::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components()))
		return E_FAIL;

	CCollider::COLLIDERDESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vScale = _float3{ 1.f, 1.f, 1.f };
	ColliderDesc.vTranslation = _float3{ 0.f, 0.f, 0.f };
	if (FAILED(__super::Add_Component(COM_COLLIDER_ANIMATTACK, LEVEL_STATIC, PROTO_COMP_OBBCOLLIDER, (CComponent**)&m_pAnimAttackCollider, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC,
		PROTO_COMP_SHADER_VTXANIMMODEL, (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	return S_OK;
}

void CCharacter::Jump(const _double& _dTimeDelta, const _string& _strTrigger)
{
	_vector vMovePos = Compute_MovePos(_dTimeDelta, m_vBehaviorDir.Get_Vector(), m_fMoveSpeed, 18.f, 0.4f, 0.5f);

	// 점프 위치가 네비 Y값보다 작으면 네비 Y로 고정
	_vector vPlayerPos = m_pTransformCom->Get_Pos() + vMovePos;
	_float fCurCellYPos = m_pNavigationCom->Compute_Height(vPlayerPos);
	if (fCurCellYPos >= XMVectorGetY(vPlayerPos))
	{
		m_dFallTimeAcc = 0.0;
		vPlayerPos = XMVectorSetY(vPlayerPos, fCurCellYPos);
		m_pAnimator->Input_Trigger(_strTrigger);
		m_bIsAir = false;
	}
	else
		m_bIsAir = true;

	// Y는 내가 설정한 Y값, XZ는 네비에 따라 제한
	m_pTransformCom->Move_Pos_NotNavigateY(vPlayerPos);
}

void CCharacter::Charge(const _double& _dTimeDelta, const _string& _strTrigger)
{
}

_vector CCharacter::Compute_MovePos(const _double& _dTimeDelta, _vector vMoveDir, _float fMoveSpeed, _float fMaxHeight, _float fMaxHeightRatio, _float fPlayTime)
{
	// 애니메이터 컴포넌트가 nullptr이 아니라면 중력을 적용하고 있는지를 알아오고, 중력이 적용되고 있을때만 FallTimeAcc를 늘려준다.
	_bool   bIsGravity = false;
	if (nullptr != m_pAnimator)
		bIsGravity = m_pAnimator->Is_Gravity();

	// 중력이 적용중이 아니라면 FallTimeAcc를 0으로 만든다.
	if (false == bIsGravity)
		m_dFallTimeAcc = 0.0;

	// 이동속도에 따른 Y를 제외한 이동값을 구한다.
	_vector vMovePos = XMVector3Normalize(vMoveDir) * fMoveSpeed * _float(_dTimeDelta);
	vMovePos = XMVectorSetY(vMovePos, 0.f);

	_float fGravity = 0.f;
	_float fHeight = 0.f;

	// 총 재생시간 * 최대 높이 도달하는 비율값이 0일때의 중력적용값
	if (fMaxHeightRatio * fPlayTime == 0.f)
	{
		fGravity = 450.f;
		fHeight = 0.f;
	}
	else
	{
		fGravity = 2 * fMaxHeight / pow(fMaxHeightRatio * fPlayTime, 2.f);
		fHeight = fGravity * fPlayTime * fMaxHeightRatio;
	}
	// FallTimeAcc값을 갖고 프레임당 Y값을 생성한다.
	_float fY = (fHeight - fGravity * (_float)m_dFallTimeAcc) * _float(_dTimeDelta);
	vMovePos = XMVectorSetY(vMovePos, fY);

	// 중력이 적용중일때만 FallTimeAcc를 늘린다.
	if (true == bIsGravity)
		m_dFallTimeAcc += _dTimeDelta;

	return vMovePos;
}

_vector CCharacter::Compute_MovePos_ByDist(const _double& _dTimeDelta, _vector vMoveDir, _float fDist, _float fMaxHeight, _float fMaxHeightRatio, _float fPlayTime)
{
	// Animator 컴포넌트가 없다면 비어있는 vector를 return한다.
	if (nullptr == m_pAnimator)
		return XMVectorZero();

	// 애니메이터 컴포넌트가 nullptr이 아니라면 중력을 적용하고 있는지를 알아오고, 중력이 적용되고 있을때만 FallTimeAcc를 늘려준다.
	_bool   bIsGravity = false;
	bIsGravity = m_pAnimator->Is_Gravity();

	// 중력이 적용중이 아니라면 FallTimeAcc를 0으로 만든다.
	if (false == bIsGravity)
		m_dFallTimeAcc = 0.0;

	// 최대 이동할 거리를 이용하여 총 플레이타임과의 계산을 통해 프레임당 이동거리를 계산한다.
	if (0.f == fPlayTime)
		return XMVectorZero();

	// 거리에 따른 프레임당 이동거리
	_vector vMovePos = XMVector3Normalize(vMoveDir) * (fDist / fPlayTime) * _float(_dTimeDelta);
	vMovePos = XMVectorSetY(vMovePos, 0.f);

	_float fGravity = 0.f;
	_float fHeight = 0.f;

	// 총 재생시간 * 최대 높이 도달하는 비율값이 0일때의 중력적용값
	if (fMaxHeightRatio * fPlayTime * fMaxHeight == 0.f)
	{
		fGravity = 400.f;
		fHeight = 0.f;
	}
	else
	{
		fGravity = 2 * fMaxHeight / pow(fMaxHeightRatio * fPlayTime, 2.f);
		fHeight = fGravity * fPlayTime * fMaxHeightRatio;
	}
	// FallTimeAcc값을 갖고 프레임당 Y값을 생성한다.
	_float fY = (fHeight - fGravity * (_float)m_dFallTimeAcc) * _float(_dTimeDelta);
	vMovePos = XMVectorSetY(vMovePos, fY);

	// 중력이 적용중일때만 FallTimeAcc를 늘린다.
	if (true == bIsGravity)
		m_dFallTimeAcc += _dTimeDelta;

	return vMovePos;
}

_vector CCharacter::Strike(const _double& _dTimeDelta, _vector vMoveDir, _float fDist, _float fMaxHeight, _float fMaxHeightRatio, _float fPlayTime)
{
	// Animator 컴포넌트가 없다면 비어있는 vector를 return한다.
	if (nullptr == m_pAnimator)
		return XMVectorZero();

	// 애니메이터 컴포넌트가 nullptr이 아니라면 중력을 적용하고 있는지를 알아오고, 중력이 적용되고 있을때만 FallTimeAcc를 늘려준다.
	_bool   bIsGravity = false;
	bIsGravity = m_pAnimator->Is_Gravity();

	// 중력이 적용중이 아니라면 FallTimeAcc를 0으로 만든다.
	if (false == bIsGravity)
		m_dFallTimeAcc = 0.0;

	// 최대 이동할 거리를 이용하여 총 플레이타임과의 계산을 통해 프레임당 이동거리를 계산한다.
	if (0.f == fPlayTime)
		return XMVectorZero();

	// 거리에 따른 프레임당 이동거리
	_vector vMovePos = XMVector3Normalize(vMoveDir) * (fDist / fPlayTime) * _float(_dTimeDelta);
	vMovePos = XMVectorSetY(vMovePos, 0.f);

	_float fGravity = 0.f;
	_float fHeight = 0.f;

	// 총 재생시간 * 최대 높이 도달하는 비율값이 0일때의 중력적용값
	if (fMaxHeightRatio * fPlayTime * fMaxHeight == 0.f)
	{
		fGravity = 350.f;
		fHeight = 0.f;
	}
	else
	{
		fGravity = 2 * fMaxHeight / pow(fMaxHeightRatio * fPlayTime, 2.f);
		fHeight = fGravity * fPlayTime * fMaxHeightRatio;
	}
	// FallTimeAcc값을 갖고 프레임당 Y값을 생성한다.
	_float fY = (fHeight - fGravity * (_float)m_dFallTimeAcc) * _float(_dTimeDelta);
	vMovePos = XMVectorSetY(vMovePos, fY);

	// 중력이 적용중일때만 FallTimeAcc를 늘린다.
	if (true == bIsGravity)
		m_dFallTimeAcc += _dTimeDelta;

	return vMovePos;
}

void CCharacter::Bound(const _double& _dTimeDelta)
{
}

/// <summary>
/// 직선으로 아래로 떨어지는 경우 사용한다.
/// </summary>
/// <param name="_dTimeDelta"></param>
void CCharacter::Fall(const _double& _dTimeDelta, const _string& _strTrigger)
{
	// Fall 도중에 맞으면 FallTimeAcc를 초기화한다.
	if (true == m_bIsHit && false == m_tBattleData.Is_Die())
		m_dFallTimeAcc = 0.0;

	_vector vMovePos = Compute_MovePos(_dTimeDelta, XMVectorZero(), 0.f);
	// 점프 위치가 네비 Y값보다 작으면 네비 Y로 고정
	_vector vPlayerPos = m_pTransformCom->Get_Pos() + vMovePos;
	_float fCurCellYPos = m_pNavigationCom->Compute_Height(vPlayerPos);
	if (fCurCellYPos >= XMVectorGetY(vPlayerPos))
	{
		m_dFallTimeAcc = 0.0;
		vPlayerPos = XMVectorSetY(vPlayerPos, fCurCellYPos);
		Input_Trigger(_strTrigger);
		m_bIsAir = false;
	}
	else
		m_bIsAir = true;

	// Y는 내가 설정한 Y값, XZ는 네비에 따라 제한
	m_pTransformCom->Move_Pos_NotNavigateY(vPlayerPos);
}

void CCharacter::Restrict_Nav(const _double& _dTimeDelta)
{
	// 밀치기 도중이면 적용하지 않는다.
	if (true == m_bIsHitPush)
		return;

	// Root본 애니메이션의 Root값을 받아서 이동합니다. ( 네비게이션에 막히면 이동하지 않습니다. )
	if (m_pAnimator->Is_Play())
	{
		if (true == m_pAnimator->Is_RootBone())
		{
			_matrix vRootBoneMatrix = m_pAnimator->Get_RootBoneMatrix();

			_vector vRootBonePos = CMethod::XMMatrixGetVector(vRootBoneMatrix, MATROW_POS);

			// 이동할 수 있는곳이라면 ( 네비게이션 메쉬 안이라면 ) 현재 위치를 RootBonePos로 지정합니다.
			//if (true == m_pNavigationCom->Is_Move(vRootBonePos))
			m_pTransformCom->Move_Pos_NotNavigateY(vRootBonePos);
		}
		// 애니메이션에서 넣어준 이동값이 있다면 이동한다.
		if (true == m_pAnimator->Is_BoneTransform() && true == m_pAnimator->Is_EventPlay())
		{
			_vector BoneTransformPos = m_pAnimator->Get_BoneTransformPos();

			_bool bIsIn = false;

			// 지상 공격이나 지상에서의 스텝 도중에는 네비게이션의 Y를 태운다.
			if ("ATTACK" == m_strAnimType || "STEP" == m_strAnimType)
				m_pTransformCom->Move_Pos(m_pTransformCom->Get_Pos() + BoneTransformPos);
			else if ("ATTACK_DOWN" == m_strAnimType)
			{
				_float fNaviHeight = m_pNavigationCom->Compute_Height(m_pTransformCom->Get_Pos());
				_vector vPos = m_pTransformCom->Get_Pos() + BoneTransformPos;

				if (XMVectorGetY(vPos) < fNaviHeight)
					vPos = XMVectorSetY(vPos, fNaviHeight);

				m_pTransformCom->Move_Pos_NotNavigateY(vPos);
			}
			// 그 외의 경우에는 네비게이션 Y를 태우지 않는다.
			else
				m_pTransformCom->Move_Pos_NotNavigateY(m_pTransformCom->Get_Pos() + BoneTransformPos);
		}
	}
}

void CCharacter::Skill_1(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
}

void CCharacter::Skill_2(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
}

void CCharacter::Skill_3(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
}

void CCharacter::Move_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
}

void CCharacter::Attack_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
}

void CCharacter::Attack_Upper_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
}

void CCharacter::Attack_Down_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
}

void CCharacter::Attack_Awake_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
}

void CCharacter::Ultimate_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
}

void CCharacter::Charge_Attack_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
}

void CCharacter::Step_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
}

void CCharacter::Dash_Ground_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
}

void CCharacter::Dash_Air_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
}

void CCharacter::Jump_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
}

void CCharacter::Passive_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
}

void CCharacter::Roll_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
}

// 지상에서 Hit된 경우 ( 높이가 없음 )
void CCharacter::Hit_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	// 콜라이더의 방향값을 받아옵니다.
	_vector vColliderDir = m_pAnimator->Get_ColliderDir();
	// 피격 효과에서 넘어온 PushedForce; 
	_float fDistance = m_tKnockBackInfo.fPushedForce;
	// 피격 효과에서 넘어온 MaxHeight
	_float fMaxHeight = m_tKnockBackInfo.fHeight;
	// 맞는 애니메이션 수행 시간
	_double dEventPlayTime = m_tKnockBackInfo.dPlayTime;
	// 밀려날 방향벡터
	_vector vAttackDir = m_tKnockBackInfo.vAttackDir.Get_Vector();

	// 계산해서 나온 프레임당 이동거리
	_vector vMovePos = Compute_MovePos_ByDist(_dTimeDelta, vAttackDir, fDistance, 0.f, 0.f, (_float)dEventPlayTime);

	_vector vPlayerPos = m_pTransformCom->Get_Pos() + vMovePos;

	// 네비게이션의 Y를 적용한다.
	m_pTransformCom->Move_Pos(vPlayerPos);
	m_dFallTimeAcc = 0.f;
}
void CCharacter::Air_Hit_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	// 콜라이더의 방향값을 받아옵니다.
	_vector vColliderDir = m_pAnimator->Get_ColliderDir();
	// 피격 효과에서 넘어온 PushedForce; 
	_float fDistance = m_tKnockBackInfo.fPushedForce;
	// 피격 효과에서 넘어온 MaxHeight
	_float fMaxHeight = m_tKnockBackInfo.fHeight;
	// 맞는 애니메이션 수행 시간
	_double dEventPlayTime = m_tKnockBackInfo.dPlayTime;
	// 밀려날 방향벡터
	_vector vAttackDir = m_tKnockBackInfo.vAttackDir.Get_Vector();

	// 계산해서 나온 프레임당 이동거리 => 미는값이 있다면 위로 조금 띄우고 없다면 그냥 있기

	// 공중에서 맞을 경우에는 기본적으로 조금씩 띄워진다.
	_float fHeight = 1.f;

	_vector vMovePos = Compute_MovePos_ByDist(_dTimeDelta, vAttackDir, fDistance, fHeight, 0.3f, 0.4f);
	// 플레이어의 위치
	_vector vPlayerPos = m_pTransformCom->Get_Pos() + vMovePos;
	_float fCurCellYPos = m_pNavigationCom->Compute_Height(vPlayerPos);
	if (fCurCellYPos >= XMVectorGetY(vPlayerPos))
	{
		m_dFallTimeAcc = 0.0;
		vPlayerPos = XMVectorSetY(vPlayerPos, fCurCellYPos);
		m_pAnimator->Input_Trigger("FALL_END");
		m_bIsAir = false;
	}
	else
		m_bIsAir = true;

	// Y는 내가 설정한 Y값, XZ는 네비에 따라 제한
	m_pTransformCom->Move_Pos_NotNavigateY(vPlayerPos);
}

void CCharacter::Down_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	// 만약 죽은 상태가 아니라면 DOWN_END를 트리거로 넣는다.
	if (false == m_tBattleData.Is_Die())
	{
		if (nullptr != m_pAnimator)
			Input_Trigger("DOWN_END");
	}

	// 네비 Y값에 내 Y를 맞춘다.
	m_pTransformCom->Move_Pos(m_pTransformCom->Get_Pos());
}

void CCharacter::Air_Down_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	// 콜라이더의 방향값을 받아옵니다.
	_vector vColliderDir = m_pAnimator->Get_ColliderDir();
	// 피격 효과에서 넘어온 PushedForce; 
	_float fDistance = m_tKnockBackInfo.fPushedForce;
	// 피격 효과에서 넘어온 MaxHeight
	_float fMaxHeight = m_tKnockBackInfo.fHeight;
	// 맞는 애니메이션 수행 시간
	_double dEventPlayTime = m_tKnockBackInfo.dPlayTime;
	// 밀려날 방향벡터
	_vector vAttackDir = m_tKnockBackInfo.vAttackDir.Get_Vector();

	// 애니메이션이 최대높이에 도달하는 비율
	_float fMaxHeightRatio = 0.7f;

	// 계산해서 나온 프레임당 이동거리
	_vector vMovePos = Compute_MovePos_ByDist(_dTimeDelta, vAttackDir, fDistance, fMaxHeight, fMaxHeightRatio, (_float)dEventPlayTime);
	// 플레이어의 위치
	_vector vPlayerPos = m_pTransformCom->Get_Pos() + vMovePos;
	_float fCurCellYPos = m_pNavigationCom->Compute_Height(vPlayerPos);
	if (fCurCellYPos >= XMVectorGetY(vPlayerPos))
	{
		m_dFallTimeAcc = 0.0;
		vPlayerPos = XMVectorSetY(vPlayerPos, fCurCellYPos);
		m_pAnimator->Input_Trigger("FALL_END");
		m_bIsAir = false;
	}
	else
		m_bIsAir = true;

	// Y는 내가 설정한 Y값, XZ는 네비에 따라 제한
	m_pTransformCom->Move_Pos_NotNavigateY(vPlayerPos);
}
void CCharacter::Strike_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	// 콜라이더의 방향값을 받아옵니다.
	_vector vColliderDir = m_pAnimator->Get_ColliderDir();
	// 피격 효과에서 넘어온 PushedForce; 
	_float fDistance = m_tKnockBackInfo.fPushedForce;
	// 피격 효과에서 넘어온 MaxHeight
	_float fMaxHeight = m_tKnockBackInfo.fHeight;
	// 맞는 애니메이션 수행 시간
	_double dEventPlayTime = m_tKnockBackInfo.dPlayTime;
	// 밀려날 방향벡터
	_vector vAttackDir = m_tKnockBackInfo.vAttackDir.Get_Vector();

	// 바운드되는 높이 ( 고정 )
	_float fBoundHeight = 5.f;
	// 스트라이크시 밀리는 거리 ( 고정 )
	_float fStrikeDistance = 0.f;
	// 애니메이션이 최대높이에 도달하는 비율
	_float fMaxHeightRatio = 0.7f;

	// 계산해서 나온 프레임당 이동거리
	_vector vMovePos = Compute_MovePos_ByDist(_dTimeDelta, vAttackDir, fStrikeDistance, 0.2f, 0.05f, (_float)dEventPlayTime);
	// 플레이어의 위치
	_vector vPlayerPos = m_pTransformCom->Get_Pos() + vMovePos;
	_float fCurCellYPos = m_pNavigationCom->Compute_Height(vPlayerPos);
	if (fCurCellYPos >= XMVectorGetY(vPlayerPos))
	{
		m_dFallTimeAcc = 0.0;
		vPlayerPos = XMVectorSetY(vPlayerPos, fCurCellYPos);
		if (true == m_pAnimator->Input_Trigger("BOUND"))
		{
			// AIR_DOWN으로 넘어가도 똑같이 떨어지게 하기 위해 Bound의 값을 m_tKnockBackInfo에 넣는다.
			CCharacter::KNOCKBACK knockBack;
			ZeroMemory(&knockBack, sizeof(CCharacter::KNOCKBACK));
			// 때린 객체의 애니메이션이 가진 PushedForce를 받아옵니다.
			knockBack.fPushedForce = 0.f;
			// 때린 객체의 애니메이션이 가진 Height를 받아옵니다.
			knockBack.fHeight = 15.f;
			// 피격 애니메이션의 이벤트 적용 시간을 받아옵니다.
			if (nullptr != m_pAnimator->Get_NextAnimationClip())
				knockBack.dPlayTime = m_pAnimator->Get_NextAnimationClip()->Get_EventPlayTime();
			// 떄린 객체 -> 맞은 객체로 향하는 방향벡터
			knockBack.vAttackDir = XMVectorZero();
			// 넉백 정보를 넣어준다.
			m_tKnockBackInfo = knockBack;
		}
		m_bIsAir = false;
	}
	else
		m_bIsAir = true;

	// Y는 내가 설정한 Y값, XZ는 네비에 따라 제한
	m_pTransformCom->Move_Pos_NotNavigateY(vPlayerPos);
}

void CCharacter::Bound_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	// 맞는 애니메이션 수행 시간
	_double dEventPlayTime = m_tKnockBackInfo.dPlayTime;
	// 밀려날 방향벡터
	_vector vAttackDir = m_tKnockBackInfo.vAttackDir.Get_Vector();
	// 바운드되는 높이 ( 고정 )
	_float fBoundHeight = m_tKnockBackInfo.fHeight;
	// 바운드시 밀리는 거리 ( 고정 )
	_float fBoundDistance = m_tKnockBackInfo.fPushedForce;
	// 애니메이션이 최대높이에 도달하는 비율
	_float fMaxHeightRatio = 0.7f;

	// 계산해서 나온 프레임당 이동거리
	_vector vMovePos = Compute_MovePos_ByDist(_dTimeDelta, vAttackDir, fBoundDistance, fBoundHeight, fMaxHeightRatio, (_float)dEventPlayTime);
	// 플레이어의 위치
	_vector vPlayerPos = m_pTransformCom->Get_Pos() + vMovePos;
	_float fCurCellYPos = m_pNavigationCom->Compute_Height(vPlayerPos);
	if (fCurCellYPos >= XMVectorGetY(vPlayerPos))
	{
		m_dFallTimeAcc = 0.0;
		vPlayerPos = XMVectorSetY(vPlayerPos, fCurCellYPos);
		m_pAnimator->Input_Trigger("FALL_END");
		m_bIsAir = false;
	}
	else
		m_bIsAir = true;

	// Y는 내가 설정한 Y값, XZ는 네비에 따라 제한
	m_pTransformCom->Move_Pos_NotNavigateY(vPlayerPos);
}

void CCharacter::Spin_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	// 콜라이더의 방향값을 받아옵니다.
	_vector vColliderDir = m_pAnimator->Get_ColliderDir();
	// 피격 효과에서 넘어온 PushedForce; 
	_float fDistance = 40.f;//m_tKnockBackInfo.fPushedForce;
	// 피격 효과에서 넘어온 MaxHeight
	_float fMaxHeight = 15.f;//m_tKnockBackInfo.fHeight;
	// 맞는 애니메이션 수행 시간
	_double dEventPlayTime = 0.7f;//m_tKnockBackInfo.dPlayTime;
	// 밀려날 방향벡터
	_vector vAttackDir = m_tKnockBackInfo.vAttackDir.Get_Vector();

	// 애니메이션이 최대높이에 도달하는 비율
	_float fMaxHeightRatio = 0.4f;

	// 계산해서 나온 프레임당 이동거리
	_vector vMovePos = Compute_MovePos_ByDist(_dTimeDelta, vAttackDir, fDistance, fMaxHeight, fMaxHeightRatio, (_float)0.5f);
	// 플레이어의 위치
	_vector vPlayerPos = m_pTransformCom->Get_Pos() + vMovePos;
	_float fCurCellYPos = m_pNavigationCom->Compute_Height(vPlayerPos);
	if (fCurCellYPos >= XMVectorGetY(vPlayerPos))
	{
		m_dFallTimeAcc = 0.0;
		vPlayerPos = XMVectorSetY(vPlayerPos, fCurCellYPos);
		m_pAnimator->Input_Trigger("FALL_END");
		m_bIsAir = false;
	}
	else
		m_bIsAir = true;

	// Y는 내가 설정한 Y값, XZ는 네비에 따라 제한
	m_pTransformCom->Move_Pos_NotNavigateY(vPlayerPos);
}

void CCharacter::Blow_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	// 콜라이더의 방향값을 받아옵니다.
	_vector vColliderDir = m_pAnimator->Get_ColliderDir();
	// 피격 효과에서 넘어온 PushedForce; 
	_float fDistance = m_tKnockBackInfo.fPushedForce;
	// 피격 효과에서 넘어온 MaxHeight
	_float fMaxHeight = m_tKnockBackInfo.fHeight;
	// 맞는 애니메이션 수행 시간
	_double dEventPlayTime = m_tKnockBackInfo.dPlayTime;
	// 밀려날 방향벡터
	_vector vAttackDir = m_tKnockBackInfo.vAttackDir.Get_Vector();

	// 애니메이션이 최대높이에 도달하는 비율
	_float fMaxHeightRatio = 0.7f;

	// 계산해서 나온 프레임당 이동거리
	_vector vMovePos = Compute_MovePos_ByDist(_dTimeDelta, vAttackDir, fDistance, fMaxHeight, fMaxHeightRatio, (_float)0.6f);
	// 플레이어의 위치
	_vector vPlayerPos = m_pTransformCom->Get_Pos() + vMovePos;
	_float fCurCellYPos = m_pNavigationCom->Compute_Height(vPlayerPos);
	if (fCurCellYPos >= XMVectorGetY(vPlayerPos))
	{
		m_dFallTimeAcc = 0.0;
		vPlayerPos = XMVectorSetY(vPlayerPos, fCurCellYPos);
		m_pAnimator->Input_Trigger("FALL_END");
		m_bIsAir = false;
	}
	else
		m_bIsAir = true;

	// Y는 내가 설정한 Y값, XZ는 네비에 따라 제한
	m_pTransformCom->Move_Pos_NotNavigateY(vPlayerPos);
}

void CCharacter::Hit_PushedState(const _double& _dTimeDelta)
{
	// 밀치기 공격에 맞았을 경우 하던 행동 그대로 하며 밀린다.
	if (true == m_bIsHitPush)
	{
		_double MaxPushTime = 0.1;

		if (MaxPushTime < m_dPushTimeAcc)
		{
			m_bIsHitPush = false;
			m_dPushTimeAcc = 0.0;
		}
		else
		{
			// 콜라이더의 방향값을 받아옵니다.
			_vector vColliderDir = m_pAnimator->Get_ColliderDir();
			// 피격 효과에서 넘어온 PushedForce; 
			_float fDistance = m_tKnockBackInfo.fPushedForce;
			// 피격 효과에서 넘어온 MaxHeight
			_float fMaxHeight = m_tKnockBackInfo.fHeight;
			// 맞는 애니메이션 수행 시간
			_double dEventPlayTime = m_tKnockBackInfo.dPlayTime;
			// 밀려날 방향벡터
			_vector vAttackDir = m_tKnockBackInfo.vAttackDir.Get_Vector();

			// 계산해서 나온 프레임당 이동거리
			_vector vMovePos = Compute_MovePos_ByDist(_dTimeDelta, vAttackDir, fDistance, 0.f, 0.f, (_float)dEventPlayTime);
			vMovePos = XMVectorSetY(vMovePos, 0.f);

			_vector vPlayerPos = m_pTransformCom->Get_Pos() + vMovePos;

			// 네비게이션의 Y를 적용한다.
			m_pTransformCom->Move_Pos_NotNavigateY(vPlayerPos);

			m_dPushTimeAcc += _dTimeDelta;
		}
	}
}

void CCharacter::Dash_Reflect_State(const _double& _dTimeDelta)
{
	_double MaxPushTime = 0.5f;

	if (MaxPushTime < m_dPushTimeAcc)
	{
		m_bIsSuccessDash = false;
		m_dPushTimeAcc = 0.0;
		m_vDashReflectDir = _float4(0.f, 0.f, 0.f, 0.f);
		m_fDashReflectDistance = 30.f;
	}
	else
	{
		// 계산해서 나온 프레임당 이동거리
		_vector vMovePos = Compute_MovePos_ByDist(_dTimeDelta, XMLoadFloat4(&m_vDashReflectDir), m_fDashReflectDistance, 0.f, 0.f, (_float)MaxPushTime);
		vMovePos = XMVectorSetY(vMovePos, 0.f);

		_vector vPlayerPos = m_pTransformCom->Get_Pos() + vMovePos;

		// 네비게이션의 Y를 적용한다.
		m_pTransformCom->Move_Pos_NotNavigateY(vPlayerPos);

		m_dPushTimeAcc += _dTimeDelta;
		m_fDashReflectDistance -= _float(_dTimeDelta * 30.f);
	}
}

void CCharacter::Dead_State(const _double& _dTimeDelta)
{
	// 프레임당 이동값을 계산한다.
	_vector vMovePos = Compute_MovePos(_dTimeDelta, m_vBehaviorDir.Get_Vector(), 0.f);

	// 점프 위치가 네비 Y값보다 작으면 네비 Y로 고정
	_vector vPlayerPos = m_pTransformCom->Get_Pos() + vMovePos;
	_float fCurCellYPos = m_pNavigationCom->Compute_Height(vPlayerPos);
	if (fCurCellYPos >= XMVectorGetY(vPlayerPos))
	{
		m_dFallTimeAcc = 0.0;
		vPlayerPos = XMVectorSetY(vPlayerPos, fCurCellYPos);
		m_bIsAir = false;
	}
	else
		m_bIsAir = true;

	// Y는 내가 설정한 Y값, XZ는 네비에 따라 제한
	m_pTransformCom->Move_Pos_NotNavigateY(vPlayerPos);
}

void CCharacter::Check_Attack_Type(_string _strHitType)
{
	if ("HIT_UPPER" == _strHitType)
	{
		Success_Attack_Upper();
	}
	else if ("HIT_DOWN" == _strHitType)
	{
		Success_Attack_Down();
	}
	else if ("SKILL_1" == _strHitType)
	{
		Success_SKILL_1();
	}
	else if ("SKILL_2" == _strHitType)
	{
		Success_SKILL_2();
	}
	else if ("SKILL_3" == _strHitType)
	{
		Success_SKILL_3();
	}
	else if ("THROW" == _strHitType)
	{
		Success_Throw();
	}
	else if ("HIT_AWAKE" == _strHitType)
	{
		Success_Attack_Awake();
	}
	else if ("ULTIMATE" == _strHitType)
	{
		Success_Ultimate();
	}
}

void CCharacter::Check_Hit_Type(_string _strHitType, CCharacter* _pEnemyCharcter)
{
	// 사운드 설정
	{
		if ("STRIKE" == _strHitType || "HIT_UPPER" == _strHitType || "HIT_DOWN" == _strHitType ||
			"SPIN" == _strHitType || "HIT_FRONT_S" == _strHitType || "HIT_BACK_S" == _strHitType || "BLOW" == _strHitType)
		{
			if (CHAR_TYPE::AKAZA == _pEnemyCharcter->m_eCharType)
			{
				Play_RandomSound(TEXT("HIT_S_FIST_SOUND"), 0.2f);
				Play_RandomSound_ByCondition(TEXT("HIT_S_VOICE"), 0.3f);
			}
			else
			{
				Play_RandomSound(TEXT("HIT_S_SOUND"), 0.2f);
				Play_RandomSound_ByCondition(TEXT("HIT_S_VOICE"), 0.3f);
			}
		}
		else if ("GUARD_NORMAL" == _strHitType || "GUARD_STRONG" == _strHitType)
		{
			Play_RandomSound(TEXT("HIT_GUARD"), 0.2f);
		}
		else
		{
			if (CHAR_TYPE::AKAZA == _pEnemyCharcter->m_eCharType)
			{
				Play_RandomSound(TEXT("HIT_FIST_SOUND"), 0.2f);
				Play_RandomSound_ByCondition(TEXT("HIT_VOICE"), 0.3f);
			}
			else
			{
				Play_RandomSound(TEXT("HIT_SOUND"), 0.2f);
				Play_RandomSound_ByCondition(TEXT("HIT_VOICE"), 0.3f);
			}
		}
	}
}

void CCharacter::Success_Attack_Upper()
{
}

void CCharacter::Success_Attack_Down()
{
}

void CCharacter::Success_SKILL_1()
{
}

void CCharacter::Success_SKILL_2()
{
}

void CCharacter::Success_SKILL_3()
{
}

void CCharacter::Success_Throw()
{
}

void CCharacter::Success_Attack_Awake()
{
}

void CCharacter::Success_Ultimate()
{
}


void CCharacter::Coll_Init()
{
	__super::Coll_Init();

	for (auto& iter : m_BonesCollider)
		iter->Get_Collider()->Init_Collision();

	if (nullptr != m_pAttackColliderCom)
		m_pAttackColliderCom->Init_Collision();

	if (nullptr != m_pAnimAttackCollider)
		m_pAnimAttackCollider->Init_Collision();
}

void CCharacter::Coll_Tick(_matrix _WorldMatrix)
{
	__super::Coll_Tick(_WorldMatrix);

	for (auto& iter : m_BonesCollider)
		iter->MultiplyCollider(XMMatrixScaling(0.1f, 0.1f, 0.1f) * _WorldMatrix);

	if (nullptr != m_pAttackColliderCom)
		m_pAttackColliderCom->Tick(_WorldMatrix);

	if (nullptr != m_pAnimAttackCollider)
		m_pAnimAttackCollider->Tick(_WorldMatrix);
}
#ifdef _DEBUG
void CCharacter::Coll_Render()
{
	__super::Coll_Render();

	// 충돌 콜라이더 렌더
	//for (auto& iter : m_BonesCollider)
	//   iter->Get_Collider()->Render_Debug();

	// 공격범위 콜라이더 렌더
	if (nullptr != m_pAttackColliderCom)
		m_pAttackColliderCom->Render_Debug();

	//공격 콜라이더 렌더
	if (nullptr != m_pAnimAttackCollider)
		m_pAnimAttackCollider->Render_Debug();
}

void CCharacter::Coll_Renderer()
{
	__super::Coll_Renderer();

	for (auto& iter : m_BonesCollider)
		m_pRendererCom->Add_Components(CRenderer::DEBUG_PLAYER, iter->Get_Collider());

	if (nullptr != m_pAttackColliderCom)
		m_pRendererCom->Add_Components(CRenderer::DEBUG_PLAYER, m_pAttackColliderCom);

	if (nullptr != m_pAttackColliderCom)
		m_pRendererCom->Add_Components(CRenderer::DEBUG_ETC, m_pAnimAttackCollider);

}
#endif
void CCharacter::Load_BonesData()
{
	if (nullptr == m_pModelCom)
		return;

	FILEGROUP* pFileGroup = m_pModelCom->Get_FileGroup();
	FILEDATA* pFileData = m_pModelCom->Get_FileData();

	if (nullptr != pFileGroup && nullptr != pFileData)
	{
		if (nullptr == m_pModelCom)
			return;

		FILEGROUP* pColliderGroup = pFileGroup->Find_Group(L"BonesCollider");
		if (nullptr == pColliderGroup)
		{
			pColliderGroup = pFileGroup->Find_Group(L"FBX_Converter");
			if (nullptr == pColliderGroup)
				return;

			pColliderGroup = pColliderGroup->Find_Group(L"BonesCollider");
			if (nullptr == pColliderGroup)
				return;
		}

		for (auto& iter : pColliderGroup->vecFileDataList)
		{
			CBonesCollider* pBonesCollider = CBonesCollider::Create();
			pBonesCollider->Load(m_pModelCom, &iter, LEVEL_STATIC);
			m_BonesCollider.push_back(pBonesCollider);
		}
	}
}

void CCharacter::Input_Trigger(const _string& _strCurrentTrigger)
{
	if (nullptr == m_pAnimator)
		return;

	m_strPrevTrigger = m_strCurrentTrigger;
	m_pAnimator->Input_Trigger(_strCurrentTrigger);
	m_strCurrentTrigger = _strCurrentTrigger;
}

void CCharacter::Success_Dash(CCharacter* _pOther, CBattle_Manager* _pBattleManager)
{
	if (nullptr == _pOther || nullptr == _pBattleManager)
		return;

	// 상대방 또한 대쉬라면 서로 튕기게 된다.
	if ("DASH" == _pOther->m_strAnimType || "AIR_DASH" == _pOther->m_strAnimType)
	{
		Input_Trigger("SUCCESS_DASH");
		m_bIsSuccessDash = false;

		_float3 vDir = XMVector3Normalize(m_pTransformCom->Get_Pos() - _pOther->Get_Transform()->Get_Pos());
		vDir.y = 0.f;

		m_vDashReflectDir = vDir;

		Play_RandomSound(TEXT("HIT_SOUND"), 0.3f);
	}
	// 상대방이 IDLE이었거나 AIR_DOWN 상태였다면 불값을 TRUE로 해주고 게임 시간을 멈춘다.
	else
	{
		Play_RandomSound(TEXT("HIT_DASH"), 0.1f);
		// 대쉬 성공을 인풋한다.
		Input_Trigger("SUCCESS_DASH");
		// 대쉬를 성공했다는것을 체크해준다.
		m_bIsSuccessDash = true;
	}
}

HRESULT CCharacter::Initialize_Sound()
{
	// HIT_SOUND
	{
		Add_Sound(TEXT("HIT_SOUND"), TEXT("Hit_Sound01"));
		Add_Sound(TEXT("HIT_SOUND"), TEXT("Hit_Sound02"));
	}
	// HIT_S_SOUND
	{
		Add_Sound(TEXT("HIT_S_SOUND"), TEXT("Hit_S_Sound01"));
		Add_Sound(TEXT("HIT_S_SOUND"), TEXT("Hit_S_Sound02"));
	}
	// HIT_FIST_SOUND
	{
		Add_Sound(TEXT("HIT_FIST_SOUND"), TEXT("Fist_Hit_01"));
		Add_Sound(TEXT("HIT_FIST_SOUND"), TEXT("Fist_Hit_02"));
		Add_Sound(TEXT("HIT_FIST_SOUND"), TEXT("Fist_Hit_03"));
	}
	// HIT_S_FIST_SOUND
	{
		Add_Sound(TEXT("HIT_S_FIST_SOUND"), TEXT("Fist_S_Hit_01"));
		Add_Sound(TEXT("HIT_S_FIST_SOUND"), TEXT("Fist_S_Hit_02"));
		Add_Sound(TEXT("HIT_S_FIST_SOUND"), TEXT("Fist_S_Hit_03"));
	}
	// HIT_GUARD
	{
		Add_Sound(TEXT("HIT_GUARD"), TEXT("Guard_Hit"));
	}
	// HIT_DASH
	{
		Add_Sound(TEXT("HIT_DASH"), TEXT("DashHit01"));
		Add_Sound(TEXT("HIT_DASH"), TEXT("DashHit02"));
	}

	return S_OK;
}

HRESULT CCharacter::Add_Sound(const _tchar* _szKey, const _tchar* _szFileName)
{
	// 존재하지 않는다면 추가하고 존재한다면 존재하는 맵에 파일이름을 넣는다.
	if (nullptr == Find_SoundList(_szKey))
	{
		vector<const _tchar*> SoundList;
		SoundList.push_back(_szFileName);
		m_SoundList.emplace(_szKey, SoundList);
	}
	else
		Find_SoundList(_szKey)->push_back(_szFileName);

	return S_OK;
}

vector<const _tchar*>* CCharacter::Find_SoundList(const _tchar* _szKey)
{
	auto iter = find_if(m_SoundList.begin(), m_SoundList.end(), CTag_Finder(_szKey));

	if (iter == m_SoundList.end())
		return nullptr;

	return &(iter->second);
}

HRESULT CCharacter::Play_RandomSound(const _tchar* _szKey, _float fVolume)
{
	vector<const _tchar*>* SoundList = Find_SoundList(_szKey);

	// 등록된 사운드를 찾아서 랜덤으로 재생시킨다/
	if (nullptr == SoundList)
		return E_FAIL;
	else
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		vector<const _tchar*> RandomSoundList;

		RandomSoundList = *SoundList;

		if (0 != SoundList->size())
		{
			_uint iRandomIndex = (_uint)pGameInstance->Get_RandomCount() % SoundList->size();
			pGameInstance->Play_SoundW(RandomSoundList[iRandomIndex], fVolume);
		}
		RELEASE_INSTANCE(CGameInstance);
	}
	return S_OK;
}

HRESULT CCharacter::Play_RandomSound_ByCondition(const _tchar* _szKey, _float fVolume)
{
	vector<const _tchar*>* SoundList = Find_SoundList(_szKey);

	// 등록된 사운드를 찾아서 랜덤으로 재생시킨다/
	if (nullptr == SoundList)
		return E_FAIL;
	else
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		vector<const _tchar*> RandomSoundList;

		RandomSoundList = *SoundList;

		if (0 != SoundList->size())
		{
			_uint iRandomIndex = (_uint)pGameInstance->Get_RandomCount() % SoundList->size();
			pGameInstance->Play_Sound_ByCondition(RandomSoundList[iRandomIndex], fVolume);
		}
		RELEASE_INSTANCE(CGameInstance);
	}
	return S_OK;
}

HRESULT CCharacter::Play_Sound(const _tchar* _szKey, const _tchar* _szFileName, _float fVolume)
{
	vector<const _tchar*>* SoundList = Find_SoundList(_szKey);

	// 등록된 사운드를 찾아서 랜덤으로 재생시킨다/
	if (nullptr == SoundList)
		return E_FAIL;
	else
	{
		for (auto& pSound : *SoundList)
		{
			if (!lstrcmp(pSound, _szFileName))
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

				pGameInstance->Play_SoundW(_szFileName, fVolume);

				RELEASE_INSTANCE(CGameInstance);

				return S_OK;
			}
		}
	}
	return E_FAIL;
}

CGameObject* CCharacter::Clone(void* pArg)
{
	return nullptr;
}

void CCharacter::Free()
{
	for (auto& iter : m_BonesCollider)
		Safe_Release(iter);

	for (auto& iter : m_CutSceneModulMap)
		Safe_Release(iter.second);

	m_BonesCollider.clear();

	Safe_Release(m_pController);
	Safe_Release(m_pAnimator);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pAnimAttackCollider);
	Safe_Release(m_pAttackColliderCom);

	__super::Free();
}


void CCharacter::tagBattleData::Decrease_HP(const _float& _fDamage)
{
	_float fDamage = fCurHp - _fDamage;
	if (fDamage <= 0)
	{
		fDamage = 0.f;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		// 막타 사운드를 재생한다.
		pGameInstance->Play_SoundW(TEXT("Makta"));

		RELEASE_INSTANCE(CGameInstance);
	}
	fCurHp = fDamage;
}

void CCharacter::tagBattleData::Init()
{
	// 승리 카운트를 제외하고 초기화해준다.
	fMaxHp = 100.f;
	//fCurHp = fMaxHp;
	fAttackPower = 0.f;
	fDefensiveValue = 0.f;
	fMaxChargeValue = 100.f;
	//fChargeValue = fMaxChargeValue;
	fMaxUltimateValue = 100.f;
	fUltimateValue = 0.f;
	fAwakeValue = 100.f;
	iUltimateMaxCount = 3;
	iUltimateCount = 0;
	iHitCount = 0;
	bIsAwake = false;
	tChargeTimer = CUSTIMER(2.f);
	tAttackTimer = CUSTIMER(20.f);
	tComboTimer = CUSTIMER(1.0f);

	//iWinCount = 0;
}

void CCharacter::tagBattleData::Decrease_Charge(const _float& fValue)
{
	// 개방중일떄는 소모하지 않는다.
	if (true == bIsAwake)
		fChargeValue -= 0.f;
	else
		fChargeValue -= fValue;

	tChargeTimer.Reset_Timer();
}

void CCharacter::tagBattleData::Increase_Ultimate(const _float& fValue)
{
	if (true == bIsAwake)
		return;

	if (!Is_CheckMaxUltimateCount())
		fUltimateValue += fValue;

	if (fUltimateValue >= fMaxUltimateValue)
		Increase_UltimateCount();

}

void CCharacter::tagBattleData::Increase_UltimateCount()
{
	if (iUltimateCount + 1 <= iUltimateMaxCount)
	{
		iUltimateCount++;
		fUltimateValue = fUltimateValue - fMaxChargeValue;
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		pGameInstance->Play_SoundW(TEXT("UltimateValue_Up"), 0.2f);

		RELEASE_INSTANCE(CGameInstance);
	}

	if (iUltimateCount == iUltimateMaxCount)
		fUltimateValue = 0.f;
}

void CCharacter::tagBattleData::Decrease_AwakeValue(const _float& fValue)
{
	if (fAwakeValue >= 0.f)
		fAwakeValue -= fValue;
	else
	{
		bIsAwake = false;
		fAwakeValue = 100.f;
	}
}

void CCharacter::tagBattleData::Active_AwakeMode()
{
	// 이미 개방상태가 아닌 경우에만 사용할 수 있다.
	if (true == bIsAwake)
		return;

	// 개방 게이지가 1개 이상일때 가능하다.
	if (iUltimateCount > 0)
	{
		iUltimateCount--;
		bIsAwake = true;
	}
}
