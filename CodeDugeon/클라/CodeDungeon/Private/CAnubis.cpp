#include "ClientDefines.h"
#include "CAnubis.h"
#include "UGameInstance.h"
#include "URenderer.h"
#include "CMainCamera.h"
#include "UTransform.h"
#include "UAnimModel.h"
#include "URegion.h"
#include "UNavigation.h"
#include "CAnubis.h"
#include "CAnubisAnimController.h"
#include "UCell.h"
#include "UMethod.h"
#include "UPlayer.h"
#include "UCollider.h"
#include "UParticle.h"
#include "UParticleSystem.h"
#include "CModelObjects.h"
#include "UAnimation.h"
#include "CAnubisStaff.h"
#include "UMat.h"
#include "UGuard.h"

CAnubis::CAnubis(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType)
	: CMob(_spDevice, _wstrLayer, _eCloneType), m_AnubisType{},
	m_PathFindingState{},
	m_AstarPath{},
	m_isPathFinding{ false },
	m_currentPathIndex{ 0 },
	m_spAnubisStaff{ nullptr },
	m_f3OriginPos{ },
	m_f3OriginDirection{},
	m_spMagicCircle{ nullptr },
	m_spMagicSphere{nullptr},
	m_bisShield{false},
	m_spFireCircle{nullptr},
	m_spFireCircle1{nullptr},
	m_dShieldTimer{},
	m_fShieldRadius{},
	m_fMagicCircleRadius{},
	m_bisFireAttack{false}
{
}

CAnubis::CAnubis(const CAnubis& _rhs)
	: CMob(_rhs), m_AnubisType{},
	m_PathFindingState{},
	m_AstarPath{},
	m_isPathFinding{ false },
	m_currentPathIndex{ 0 },
	m_spAnubisStaff{nullptr},
	m_f3OriginPos{},
	m_f3OriginDirection{},
	m_spMagicCircle{ nullptr },
	m_spMagicSphere{ nullptr },
	m_bisShield{ false },
	m_spFireCircle{ nullptr },
	m_spFireCircle1{ nullptr },
	m_dShieldTimer{},
	m_fShieldRadius{},
	m_fMagicCircleRadius{},
	m_bisFireAttack{ false }
{
}

void CAnubis::Free()
{
}

HRESULT CAnubis::NativeConstruct()
{
	return __super::NativeConstruct();
}

void CAnubis::CreateParticles()
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	{
		UParticle::PARTICLEDESC tDesc;
		tDesc.wstrParticleComputeShader = PROTO_RES_COMPUTEBLOODEFFECT2DSHADER;
		tDesc.wstrParticleShader = PROTO_RES_PARTICLEBLOOD2DSHADER;


		tDesc.ParticleParam.stGlobalParticleInfo.fAccTime = 0.f;
		//tDesc.ParticleParam.stGlobalParticleInfo.fDeltaTime = 2.f;
		tDesc.ParticleParam.stGlobalParticleInfo.fEndScaleParticle = 3.5f;
		tDesc.ParticleParam.stGlobalParticleInfo.fStartScaleParticle = 0.5f;
		tDesc.ParticleParam.stGlobalParticleInfo.fMaxLifeTime = 1.;
		tDesc.ParticleParam.stGlobalParticleInfo.fMinLifeTime = 0.3f;
		tDesc.ParticleParam.stGlobalParticleInfo.fMaxSpeed = 7;
		tDesc.ParticleParam.stGlobalParticleInfo.fMinSpeed = 5;
		tDesc.ParticleParam.stGlobalParticleInfo.iMaxCount = 400;
		tDesc.ParticleParam.stGlobalParticleInfo.fParticleThickness = 1.f;
		tDesc.ParticleParam.stGlobalParticleInfo.fParticleDirection = _float3(0.f, 0.f, 0.1f);
		tDesc.ParticleParam.stGlobalParticleInfo.fParticlePosition = _float3(0.f, 0.f, 0.f);
		tDesc.ParticleParam.stGlobalParticleInfo.fParticleKind = PARTICLE_BLOOD;
		m_spBloodParticle = std::static_pointer_cast<UParticle>(spGameInstance->CloneActorAdd(PROTO_ACTOR_PARTICLE, { &tDesc }));
	}
	{
		m_stParticleParam = m_spBloodParticle->GetParticleSystem()->GetParticleParam();
		m_stParticleType = m_spBloodParticle->GetParticleSystem()->GetParticleTypeParam();
		m_stParticleType->fParticleType = PARTICLE_TYPE_DEFAULT;
		m_stParticleType->fParticleLifeTimeType = PARTICLE_LIFETIME_TYPE_DEFAULT;
		m_spBloodParticle->SetBloodTexture(0, L"blood"); // y값 증가 x 원
		m_spBloodParticle->SetBloodTexture(1, L"blood1"); // y값 증가 x 원
		m_spBloodParticle->SetBloodTexture(2, L"blood2"); // y값 증가 x 원
		m_spBloodParticle->SetBloodTexture(3, L"blood3"); // y값 증가 x 원
		m_spBloodParticle->SetBloodTexture(4, L"blood4"); // y값 증가 x 원
		m_spBloodParticle->SetBloodTexture(5, L"blood5"); // y값 증가 x 원
		m_spBloodParticle->SetBloodTexture(6, L"blood6"); // y값 증가 x 원
		m_spBloodParticle->SetBloodTexture(7, L"blood7"); // y값 증가 x 원
		m_spBloodParticle->SetParticleType(PARTICLE_BLOOD);
		*m_spBloodParticle->GetParticleSystem()->GetCreateInterval() = 0.85f;
		*m_spBloodParticle->GetParticleSystem()->GetAddParticleAmount() = 5;
	}
	{
		UParticle::PARTICLEDESC tDesc;
		tDesc.wstrParticleComputeShader = PROTO_RES_COMPUTESLASH2DSHADER;
		tDesc.wstrParticleShader = PROTO_RES_PARTICLESLASH2DSHADER;


		tDesc.ParticleParam.stGlobalParticleInfo.fAccTime = 0.f;
		//tDesc.ParticleParam.stGlobalParticleInfo.fDeltaTime = 2.f;
		tDesc.ParticleParam.stGlobalParticleInfo.fEndScaleParticle = 1.75f;
		tDesc.ParticleParam.stGlobalParticleInfo.fStartScaleParticle = 0.5f;
		tDesc.ParticleParam.stGlobalParticleInfo.fMaxLifeTime = 0.30;
		tDesc.ParticleParam.stGlobalParticleInfo.fMinLifeTime = 0.15f;
		tDesc.ParticleParam.stGlobalParticleInfo.fMaxSpeed = 7;
		tDesc.ParticleParam.stGlobalParticleInfo.fMinSpeed = 5;
		tDesc.ParticleParam.stGlobalParticleInfo.iMaxCount = 3;
		tDesc.ParticleParam.stGlobalParticleInfo.fParticleThickness = 1.f;
		tDesc.ParticleParam.stGlobalParticleInfo.fParticleDirection = _float3(0.f, 0.f, 0.1f);
		tDesc.ParticleParam.stGlobalParticleInfo.fParticlePosition = _float3(0.f, 0.f, 0.f);
		tDesc.ParticleParam.stGlobalParticleInfo.fParticleKind = PARTICLE_SLASH;
		m_spSlashParticle = std::static_pointer_cast<UParticle>(spGameInstance->CloneActorAdd(PROTO_ACTOR_PARTICLE, { &tDesc }));
	}
	{
		m_spSlashParticle->GetParticleSystem()->GetParticleTypeParam()->fParticleType = PARTICLE_TYPE_DEFAULT;
		m_spSlashParticle->GetParticleSystem()->GetParticleTypeParam()->fParticleLifeTimeType = PARTICLE_LIFETIME_TYPE_DEFAULT;
		m_spSlashParticle->SetTexture(L"Slash2");

		m_spSlashParticle->SetParticleType(PARTICLE_SLASH);
		*m_spSlashParticle->GetParticleSystem()->GetCreateInterval() = 0.35f;
		*m_spSlashParticle->GetParticleSystem()->GetAddParticleAmount() = 1;
	}
	{

		UParticle::PARTICLEDESC tDesc;
		tDesc.wstrParticleComputeShader = PROTO_RES_COMPUTEATTACKEFFECT2DSHADER;
		tDesc.wstrParticleShader = PROTO_RES_PARTICLEATTACK2DSHADER;


		tDesc.ParticleParam.stGlobalParticleInfo.fAccTime = 0.f;
		//tDesc.ParticleParam.stGlobalParticleInfo.fDeltaTime = 2.f;
		tDesc.ParticleParam.stGlobalParticleInfo.fEndScaleParticle = 0.45f;  //0.45f;  //0.85 scale 끌때
		tDesc.ParticleParam.stGlobalParticleInfo.fStartScaleParticle = 0.05f;  //0.05f;  //1.0 
		tDesc.ParticleParam.stGlobalParticleInfo.fMaxLifeTime = 1.5f;
		tDesc.ParticleParam.stGlobalParticleInfo.fMinLifeTime = 0.1f;
		tDesc.ParticleParam.stGlobalParticleInfo.fMaxSpeed = 2.88f;
		tDesc.ParticleParam.stGlobalParticleInfo.fMinSpeed = 1.88f;
		tDesc.ParticleParam.stGlobalParticleInfo.iMaxCount = 512;
		tDesc.ParticleParam.stGlobalParticleInfo.fParticleThickness = 1.f;
		tDesc.ParticleParam.stGlobalParticleInfo.fParticleDirection = _float3(0.f, 0.f, 0.f);
		tDesc.ParticleParam.stGlobalParticleInfo.fParticlePosition = _float3(0.f, 0.f, 0.f);
		tDesc.ParticleParam.stGlobalParticleInfo.fParticleKind = PARTICLE_ATTACK;
		m_spAttackParticle = std::static_pointer_cast<UParticle>(spGameInstance->CloneActorAdd(PROTO_ACTOR_PARTICLE, { &tDesc }));
	}
	{
		m_spAttackParticle->GetParticleSystem()->GetParticleTypeParam()->fParticleType = PARTICLE_TYPE_DEFAULT;
		m_spAttackParticle->GetParticleSystem()->GetParticleTypeParam()->fParticleLifeTimeType = PARTICLE_LIFETIME_TYPE_DEFAULT;
		m_spAttackParticle->SetParticleType(PARTICLE_ATTACK);
		*m_spAttackParticle->GetParticleSystem()->GetAddParticleAmount() = 7;
		*m_spAttackParticle->GetParticleSystem()->GetCreateInterval() = 0.5f;
		m_spAttackParticle->SetTexture(L"FireSpark3");
	}
	{

		UParticle::PARTICLEDESC tDesc;
		tDesc.wstrParticleComputeShader = PROTO_RES_COMPUTEATTACKEFFECT2DSHADER;
		tDesc.wstrParticleShader = PROTO_RES_PARTICLEATTACKTWO2DSHADER;


		tDesc.ParticleParam.stGlobalParticleInfo.fAccTime = 0.f;
		tDesc.ParticleParam.stGlobalParticleInfo.fDeltaTime = 2.f;
		tDesc.ParticleParam.stGlobalParticleInfo.fEndScaleParticle = 1.0f;///0.4;//	 0.8f
		tDesc.ParticleParam.stGlobalParticleInfo.fStartScaleParticle = 1.2f;///0.2;//	 1.0f
		tDesc.ParticleParam.stGlobalParticleInfo.fMaxLifeTime = 1.5f;
		tDesc.ParticleParam.stGlobalParticleInfo.fMinLifeTime = 0.1f;
		tDesc.ParticleParam.stGlobalParticleInfo.fMaxSpeed = 7.25f;//3.25f
		tDesc.ParticleParam.stGlobalParticleInfo.fMinSpeed = 1.88f;
		tDesc.ParticleParam.stGlobalParticleInfo.iMaxCount = 512;
		tDesc.ParticleParam.stGlobalParticleInfo.fParticleThickness = 1.f;
		tDesc.ParticleParam.stGlobalParticleInfo.fParticleDirection = _float3(0.f, 0.f, 0.f);
		tDesc.ParticleParam.stGlobalParticleInfo.fParticlePosition = _float3(0.f, 0.f, 0.f);
		tDesc.ParticleParam.stGlobalParticleInfo.fParticleKind = PARTICLE_ATTACK;
		m_spAttackParticleTwo = std::static_pointer_cast<UParticle>(spGameInstance->CloneActorAdd(PROTO_ACTOR_PARTICLE, { &tDesc }));
	}
	{
		m_spAttackParticleTwo->GetParticleSystem()->GetParticleTypeParam()->fParticleType = PARTICLE_TYPE_DEFAULT;
		m_spAttackParticleTwo->GetParticleSystem()->GetParticleTypeParam()->fParticleLifeTimeType = PARTICLE_LIFETIME_TYPE_DEFAULT;
		m_spAttackParticleTwo->SetParticleType(PARTICLE_ATTACK);
		*m_spAttackParticleTwo->GetParticleSystem()->GetAddParticleAmount() = 3;
		*m_spAttackParticleTwo->GetParticleSystem()->GetCreateInterval() = 0.5f;
		m_spAttackParticleTwo->SetTexture(L"FireSpark3");
	} 
	{
		UParticle::PARTICLEDESC tDesc;
		tDesc.wstrParticleComputeShader = PROTO_RES_COMPUTEFOOTPRINT2DSHADER;
		tDesc.wstrParticleShader = PROTO_RES_PARTICLEFOOTPRINT2DSHADER;


		tDesc.ParticleParam.stGlobalParticleInfo.fAccTime = 0.f;
		//tDesc.ParticleParam.stGlobalParticleInfo.fDeltaTime = 2.f;
		tDesc.ParticleParam.stGlobalParticleInfo.fEndScaleParticle = 7.5f;// 6.5f
		tDesc.ParticleParam.stGlobalParticleInfo.fStartScaleParticle = 4.5f; //3.1f;
		tDesc.ParticleParam.stGlobalParticleInfo.fMaxLifeTime = 2.0f;
		tDesc.ParticleParam.stGlobalParticleInfo.fMinLifeTime = 0.1f;
		tDesc.ParticleParam.stGlobalParticleInfo.fMaxSpeed = 1.f;
		tDesc.ParticleParam.stGlobalParticleInfo.fMinSpeed = 1.f;
		tDesc.ParticleParam.stGlobalParticleInfo.iMaxCount = 50;
		tDesc.ParticleParam.stGlobalParticleInfo.fParticleThickness = 1.f;
		tDesc.ParticleParam.stGlobalParticleInfo.fParticleDirection = -GetTransform()->GetLook();
		tDesc.ParticleParam.stGlobalParticleInfo.fParticlePosition = GetTransform()->GetPos();
		tDesc.ParticleParam.stGlobalParticleInfo.fParticleKind = PARTICLE_FOOTPRINT;
		m_spFootPrintParticle = std::static_pointer_cast<UParticle>(spGameInstance->CloneActorAdd(PROTO_ACTOR_PARTICLE, { &tDesc }));

		m_stParticleParam = m_spFootPrintParticle->GetParticleSystem()->GetParticleParam();
		m_stParticleType = m_spFootPrintParticle->GetParticleSystem()->GetParticleTypeParam();
		m_stParticleType->fParticleType = PARTICLE_TYPE_AUTO;
		m_stParticleType->fParticleLifeTimeType = PARTICLE_LIFETIME_TYPE_DEFAULT;
		m_spFootPrintParticle->SetTexture(L"Sand");// DUST 


		*m_spFootPrintParticle->GetParticleSystem()->GetCreateInterval() = 0.8f;
		*m_spFootPrintParticle->GetParticleSystem()->GetAddParticleAmount() = 2;
		m_spFootPrintParticle->SetParticleType(PARTICLE_FOOTPRINT);
		m_spFootPrintParticle->SetActive(false);
	}
}

HRESULT CAnubis::NativeConstructClone(const VOIDDATAS& _Datas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_Datas), E_FAIL);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

	CreateParticles();
	{
		CAnubisStaff::EQDESC Desc1(std::static_pointer_cast<UModel>(spGameInstance->CloneResource(PROTO_RES_ANUBISSTAFFMODEL)), ThisShared<UCharacter>(), L"..\\..\\Resource\\Model\\Item\\Equip\\AnubisHook\\Convert\\EquipDesc\\Anubis_Staff_FBX.bin");
		m_spAnubisStaff = std::static_pointer_cast<CAnubisStaff>(spGameInstance->CloneActorAdd(PROTO_ACTOR_ANUBISSTAFF, { &Desc1 }));

	}

	UCollider::COLLIDERDESC tDesc;
	tDesc.vTranslation = _float3(1, 8, 1);
	tDesc.vScale = _float3(0, 10, 0);
	SHPTR<UCollider> Collider = static_pointer_cast<UCollider>(spGameInstance->CloneComp(PROTO_COMP_OBBCOLLIDER, { &tDesc }));
	_wstring mainColliderTag = L"Main";

	AddColliderInContainer(mainColliderTag, Collider);
	UCollider::COLLIDERDESC tDesc2;
	tDesc2.vTranslation = _float3(0.f, 5.f, 0.f);
	tDesc2.vScale = _float3(8, 8, 8);
	SHPTR<UCollider> Collider2 = static_pointer_cast<UCollider>(spGameInstance->CloneComp(PROTO_COMP_SPHERECOLLIDER, { &tDesc2 }));
	_wstring mainColliderTag2 = L"MagicSphere";

	AddColliderInContainer(mainColliderTag2, Collider2);
	UCollider::COLLIDERDESC tDesc3;
	tDesc3.vTranslation = _float3(0.f, 0.5f, 0.f);
	tDesc3.vScale = _float3(1, 1, 1);
	SHPTR<UCollider> Collider3 = static_pointer_cast<UCollider>(spGameInstance->CloneComp(PROTO_COMP_OBBCOLLIDER, { &tDesc3 }));
	_wstring mainColliderTag3 = L"MagicCircle";
	AddColliderInContainer(mainColliderTag3, Collider3);

	m_spMagicCircle = std::static_pointer_cast<UMat>(spGameInstance->CloneActorAdd(PROTO_ACTOR_MAT));
	m_spMagicCircle->SetColorTexture(L"Magic5");
	m_fMagicCircleRadius = 20;
	m_spMagicCircle ->GetTransform()->SetScale(_float3(1, 1, 1));

	m_spFireCircle = std::static_pointer_cast<UMat>(spGameInstance->CloneActorAdd(PROTO_ACTOR_MAT));
	m_spFireCircle->SetColorTexture(L"firecircle3");
	m_spFireCircle->GetTransform()->SetScale(_float3(1, 1, 1));
	m_spFireCircle->SetActive(false);

	m_spFireCircle1 = std::static_pointer_cast<UMat>(spGameInstance->CloneActorAdd(PROTO_ACTOR_MAT));
	m_spFireCircle1->SetColorTexture(L"FireCircle");
	m_spFireCircle1->GetTransform()->SetScale(_float3(1, 1, 1));
	m_spFireCircle1->SetActive(false);

	UGuard::GUARDDESC guardDesc;
	guardDesc.GuardType = UGuard::TYPE_SPHERE;
	m_spMagicSphere = std::static_pointer_cast<UGuard>(spGameInstance->CloneActorAdd(PROTO_ACTOR_GUARD, { &guardDesc }));
	m_spMagicSphere->SetActive(true);
	m_spMagicSphere->SetColorTexture(L"purple");
	m_fShieldRadius = 8;
	m_spMagicSphere->GetTransform()->SetScale(_float3(m_fShieldRadius, m_fShieldRadius, m_fShieldRadius));

	//spGameInstance->AddCollisionPawnList(m_spMagicSphere);

	SetHealth(1000);
	SetMaxHealth(1000);
	SetActivationRange(70);
	SetDeactivationRange(80);
	SetOutlineByAbility(true);
	SetOutlineColor(_float3(1, 1, 0));
	SetOutline(true);
	return S_OK;
}

void CAnubis::TickActive(const _double& _dTimeDelta)
{
	__super::TickActive(_dTimeDelta);
	GetAnimationController()->Tick(_dTimeDelta);
	_float3 pos = GetTransform()->GetPos();

	pos.y += 5;
	m_spBloodParticle->SetPosition(pos);
	m_spSlashParticle->SetPosition(pos);
	m_spAttackParticle->SetPosition(pos);
	m_spAttackParticleTwo->SetPosition(pos);
	_int CurAnimState = GetAnimationController()->GetAnimState();
	if (true == IsSendDataToBehavior())
	{
		_float3 CurrentMobPos = GetTransform()->GetPos();
		_float3 CurrentPlayerPos = GetTargetPlayer()->GetTransform()->GetPos();
		SHPTR<UCell> CurrentMobCell = GetCurrentNavi()->GetCurCell();
		SHPTR<UCell> CurrentPlayerCell = GetTargetPlayer()->GetCurrentNavi()->GetCurCell();

		const _wstring& CurAnimName = GetAnimModel()->GetCurrentAnimation()->GetAnimName();
		_float walkingSpeed = 15;

		if (CurAnimState == UAnimationController::ANIM_MOVE)
		{
			AddTimeAccumulator(_dTimeDelta);

			// A* for moving towards player when player is found
			if (GetFoundTargetState() && !GetTargetPlayer()->GetDeathState())
			{
				m_spFootPrintParticle->SetActive(true);
				{
					*m_spFootPrintParticle->GetParticleSystem()->GetAddParticleAmount() = 4;
					*m_spFootPrintParticle->GetParticleSystem()->GetCreateInterval() = 0.355f;
					_float3 pos = GetTransform()->GetPos() + GetTransform()->GetRight();
					pos.y += 1.0;
					_float3 Look = GetTransform()->GetLook();
					_float3 Right = 1.2 * GetTransform()->GetRight();
					//pos -= 3 * Look;
					m_spFootPrintParticle->SetPosition(pos);
					m_spFootPrintParticle->SetDirection(Right);
				}
			//	SetOutline(true);
				if (GetTimeAccumulator() >= 0.5)
				{
					SHPTR<UNavigation> spNavigation = GetCurrentNavi();
					m_PathFindingState = (spNavigation->StartPathFinding(CurrentMobPos, CurrentPlayerPos, CurrentMobCell, CurrentPlayerCell));
					m_isPathFinding = true;
					SetTimeAccumulator(0.0);
				}
				if (m_isPathFinding)
				{
					SHPTR<UNavigation> spNavigation = GetCurrentNavi();
					if (spNavigation->StepPathFinding(m_PathFindingState))
					{
						m_isPathFinding = false;
						if (m_PathFindingState.pathFound)
						{
							m_AstarPath = (spNavigation->OptimizePath(m_PathFindingState.path, CurrentMobPos, CurrentPlayerPos));
							m_currentPathIndex = 0; // index initialized when path is optimized
						}
					}
				}
				if (!m_AstarPath.empty())
				{
					MoveAlongPath(m_AstarPath, m_currentPathIndex, _dTimeDelta);
					_float3 direction = CurrentMobPos - GetTargetPos();
					GetTransform()->SetDirectionFixedUp(-direction, _dTimeDelta, 5);
				}
				if (CurAnimName != L"Cast")
					GetTransform()->TranslateDir(GetTransform()->GetLook(), _dTimeDelta, walkingSpeed);
			}
			else
			{
				//SetOutline(false);
				SHPTR<UNavigation> spNavigation = GetCurrentNavi();
				SHPTR<UCell> originCell = spNavigation->FindCell(m_f3OriginPos);
				if (GetTimeAccumulator() >= 0.5)
				{
					m_PathFindingState = (spNavigation->StartPathFinding(CurrentMobPos, originCell->GetCenterPos(), CurrentMobCell, originCell));
					m_isPathFinding = true;
					SetTimeAccumulator(0.0);
				}
				if (m_isPathFinding)
				{
					if (spNavigation->StepPathFinding(m_PathFindingState))
					{
						m_isPathFinding = false;
						if (m_PathFindingState.pathFound)
						{
							m_AstarPath = (spNavigation->OptimizePath(m_PathFindingState.path, CurrentMobPos, originCell->GetCenterPos()));
							m_currentPathIndex = 0; // index initialized when path is optimized
						}
					}
				}
				if (!m_AstarPath.empty())
				{
					MoveAlongPath(m_AstarPath, m_currentPathIndex, _dTimeDelta);
					_float3 direction = CurrentMobPos - GetTargetPos();
					GetTransform()->SetDirectionFixedUp(-direction, _dTimeDelta, 5);
				}
				GetTransform()->TranslateDir(GetTransform()->GetLook(), _dTimeDelta, walkingSpeed);
			}
		}
		else if (CurAnimState == UAnimationController::ANIM_ATTACK)
		{
			_float3 direction = CurrentMobPos - CurrentPlayerPos;
			GetTransform()->SetDirectionFixedUp(-direction, _dTimeDelta, 5);
		}
		else {
			m_spFootPrintParticle->SetActive(false);
			*m_spFootPrintParticle->GetParticleSystem()->GetAddParticleAmount() = 0;
			*m_spFootPrintParticle->GetParticleSystem()->GetCreateInterval() = 0.8f;
		}

		SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

		// death animation
		if (CurAnimState == UAnimationController::ANIM_DEATH)
		{
			if (GetElapsedTime() == 0)
			{
				spGameInstance->SoundPlayOnce(L"AnubisDeath");
			}

			_double DeathAnimSpeed = 20;
			SetElapsedTime(GetElapsedTime() + (_dTimeDelta * DeathAnimSpeed));
			_double DeathTimeArcOpenEnd = 50;
			if (GetElapsedTime() < DeathTimeArcOpenEnd) {
				GetAnimModel()->TickAnimToTimeAccChangeTransform(GetTransform(), _dTimeDelta, GetElapsedTime());
				GetAnimModel()->UpdateDissolveTImer(_dTimeDelta * 1.2f);
				if (!m_bDissolveSound) {
					spGameInstance->SoundPlayOnce(L"DissolveSound");
					m_bDissolveSound = true;
				}
			}
		}
		else if (CurAnimState == UAnimationController::ANIM_IDLE)
		{
		//	SetOutline(false);
			GetAnimModel()->TickAnimation(_dTimeDelta);
			GetTransform()->SetPos(GetTransform()->GetPos());
		}
		else
		{
			m_bDissolveSound = false;
			GetAnimModel()->TickAnimChangeTransform(GetTransform(), _dTimeDelta);
			SetElapsedTime(0.0);
		}
	}
	else
	{
		if (CurAnimState == UAnimationController::ANIM_MOVE)
		{
			if (GetFoundTargetState() && !GetTargetPlayer()->GetDeathState())
			{
				m_spFootPrintParticle->SetActive(true);
				{
					*m_spFootPrintParticle->GetParticleSystem()->GetAddParticleAmount() = 4;
					*m_spFootPrintParticle->GetParticleSystem()->GetCreateInterval() = 0.355f;
					_float3 pos = GetTransform()->GetPos() + GetTransform()->GetRight();
					pos.y += 1.0;
					_float3 Look = GetTransform()->GetLook();
					_float3 Right = 1.2 * GetTransform()->GetRight();
					//pos -= 3 * Look;
					m_spFootPrintParticle->SetPosition(pos);
					m_spFootPrintParticle->SetDirection(Right);
				}
			//	SetOutline(true);
			}
			else
			{
				//SetOutline(false);
			}
		}
		else if (CurAnimState == UAnimationController::ANIM_ATTACK){}
		else
		{
			m_spFootPrintParticle->SetActive(false);
			*m_spFootPrintParticle->GetParticleSystem()->GetAddParticleAmount() = 0;
			*m_spFootPrintParticle->GetParticleSystem()->GetCreateInterval() = 0.8f;
		}
		SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
		// death animation
		if (CurAnimState == UAnimationController::ANIM_DEATH)
		{
			_double DeathAnimSpeed = 20;
			SetElapsedTime(GetElapsedTime() + (_dTimeDelta * DeathAnimSpeed));
			_double DeathTimeArcOpenEnd = 50;
			if (GetElapsedTime() < DeathTimeArcOpenEnd) {
				GetAnimModel()->TickAnimToTimeAccChangeTransform(GetTransform(), _dTimeDelta, GetElapsedTime());
				GetAnimModel()->UpdateDissolveTImer(_dTimeDelta * 1.2f);
				if (!m_bDissolveSound) {
					spGameInstance->SoundPlayOnce(L"DissolveSound");
					m_bDissolveSound = true;
				}
			}
		}
		else if (CurAnimState == UAnimationController::ANIM_IDLE)
		{
			SetOutline(false);
			GetAnimModel()->TickAnimation(_dTimeDelta);
			GetTransform()->SetPos(GetTransform()->GetPos());
		}
		else
		{
			m_bDissolveSound = false;
			GetAnimModel()->TickAnimation(_dTimeDelta);
			SetElapsedTime(0.0);
		}
	}

	for (auto& iter : GetColliderContainer())
	{
		if (iter.first == L"Main")
			iter.second->SetTransform(GetTransform()->GetWorldMatrix());
		else if (iter.first == L"MagicSphere")
			iter.second->SetTransform(GetTransform()->GetPos(), GetTransform()->GetQuaternion());
		else
		{
			iter.second->SetTransform(GetTransform()->GetPos(), m_spFireCircle->GetTransform()->GetQuaternion());
			iter.second->SetScale(_float3(m_spFireCircle->GetTransform()->GetScale().x - 7, 0.3, m_spFireCircle->GetTransform()->GetScale().y - 7));
		}
	}
}

void CAnubis::LateTickActive(const _double& _dTimeDelta)
{
	if (true == IsSendDataToBehavior())
	{
		_float newHeight = GetCurrentNavi()->ComputeHeight(GetTransform()->GetPos());
		GetTransform()->SetPos(_float3(GetTransform()->GetPos().x, newHeight, GetTransform()->GetPos().z));
	}

	__super::LateTickActive(_dTimeDelta);

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	_int CurAnimState = GetAnimationController()->GetAnimState();
	if (CurAnimState == UAnimationController::ANIM_DEATH)
	{
		if (GetElapsedTime() >= 100.0)
		{
			m_spBloodParticle->SetActive(false);
			m_spFootPrintParticle->SetActive(false);
			m_spSlashParticle->SetActive(false);
			m_spAttackParticle->SetActive(false);
			m_spAttackParticleTwo->SetActive(false);
			SetActive(false);
			spGameInstance->RemoveCollisionPawn(ThisShared<CMob>());
		}
	}

	m_spFireCircle1->GetTransform()->SetPos(_float3(GetTransform()->GetPos().x, GetTransform()->GetPos().y + 0.5, GetTransform()->GetPos().z));
	m_spFireCircle1->GetTransform()->RotateTurn(_float3(0, 1, 0), 500, _dTimeDelta);

	m_spFireCircle->GetTransform()->SetPos(_float3(GetTransform()->GetPos().x, GetTransform()->GetPos().y + 0.5, GetTransform()->GetPos().z));
	m_spFireCircle->GetTransform()->RotateTurn(_float3(0, 1, 0), 500, _dTimeDelta);


	if (m_bisShield)
	{
		if(m_dShieldTimer == 0)
		{
			spGameInstance->SoundPlayOnce(L"barrier_make");
		}

		m_dShieldTimer += _dTimeDelta;
		m_spMagicCircle->SetActive(true);
		m_spMagicSphere->SetActive(true);
		if (m_dShieldTimer <= 8)
		{
			m_spMagicSphere->GetTransform()->SetPos(_float3(GetTransform()->GetPos().x, GetTransform()->GetPos().y + 5, GetTransform()->GetPos().z));
			m_spMagicCircle->GetTransform()->SetPos(_float3(GetTransform()->GetPos().x, GetTransform()->GetPos().y + 0.5, GetTransform()->GetPos().z));
			m_spMagicCircle->GetTransform()->RotateTurn(_float3(0, 1, 0), 30, _dTimeDelta);
			if (1 + 20 * m_dShieldTimer < m_fMagicCircleRadius)
			{
				m_spMagicCircle->GetTransform()->SetScale(_float3(1 + 20 * m_dShieldTimer, 1 + 20 * m_dShieldTimer, 1));
			}
			if (1 + 10 * m_dShieldTimer < m_fShieldRadius)
			{
				m_spMagicSphere->GetTransform()->SetScale(_float3(1 + 10 * m_dShieldTimer, 1 + 10 * m_dShieldTimer, 1 + 10 * m_dShieldTimer));
			}
		}
		else
		{
			m_dShieldTimer = 0;
			m_bisShield = false;
			m_spMagicCircle->SetActive(false);
			m_spMagicSphere->SetActive(false);
			m_spMagicCircle->GetTransform()->SetScale(_float3(1, 1, 1));
			m_spMagicSphere->GetTransform()->SetScale(_float3(1, 1, 1));
		}
	}
}

HRESULT CAnubis::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	const _wstring& CurAnimName = GetAnimModel()->GetCurrentAnimation()->GetAnimName();

	__super::RenderActive(_spCommand, _spTableDescriptor);
	return S_OK;
}

HRESULT CAnubis::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	return __super::RenderShadowActive(_spCommand, _spTableDescriptor);
}

HRESULT CAnubis::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	return __super::RenderOutlineActive(_spCommand, _spTableDescriptor,_pass);
}

void CAnubis::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
	__super::Collision(_pEnemy, _dTimeDelta);

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	PAWNTYPE ePawnType = _pEnemy->GetPawnType();
	const _wstring& CurAnimName = GetAnimModel()->GetCurrentAnimation()->GetAnimName();

	_float3 direction = (_pEnemy->GetTransform()->GetPos() - GetTransform()->GetPos());
	direction.Normalize();

	auto handleCollisionWithPlayer = [&](UCharacter* pCharacter) {
		for (const auto& iter : GetColliderContainer())
		{
			if(iter.first == L"Main")
			{
				if(!m_bisShield)
				{
					if (pCharacter->GetAnimModel()->IsCollisionAttackCollider(iter.second))
					{
						if (CurAnimName != L"openLaying" && CurAnimName != L"openStanding" &&
							CurAnimName != L"taunt" && CurAnimName != L"death")
						{
							if (!GetIsHItAlreadyState())
							{
								m_spBloodParticle->SetActive(true);
								m_spSlashParticle->SetActive(true);
								m_spAttackParticle->SetActive(true);
								m_spAttackParticleTwo->SetActive(true);
								m_spBloodParticle->GetParticleSystem()->GetParticleParam()->stGlobalParticleInfo.fAccTime = 0.f;
								m_spSlashParticle->GetParticleSystem()->GetParticleParam()->stGlobalParticleInfo.fAccTime = 0.f;
								m_spAttackParticle->GetParticleSystem()->GetParticleParam()->stGlobalParticleInfo.fAccTime = 0.f;
								m_spAttackParticleTwo->GetParticleSystem()->GetParticleParam()->stGlobalParticleInfo.fAccTime = 0.f;
								// Decrease health on hit
								DecreaseHealth(pCharacter->GetAttack());
							}



							SetHitAlreadyState(true);
						}
					}
					else
					{
						SetHitAlreadyState(false);
					}
				}

				for (const auto& iter2 : pCharacter->GetColliderContainer())
				{
					if (iter2.first == L"Main")
					{
						if (iter.second->IsCollision(iter2.second))
						{
							SetCollisionState(true);
							GetTransform()->SetPos(GetTransform()->GetPos() - direction * 7 * _dTimeDelta);
						}
						else
						{
							SetCollisionState(false);
						}
					}
				}
			}
		}
		};

	auto handleCollisionWithCharacter = [&](UCharacter* pCharacter) {
		for (const auto& iter : GetColliderContainer())
		{
			for (const auto& iter2 : pCharacter->GetColliderContainer())
			{
				if (iter2.first == L"Main")
				{
					if (iter.second->IsCollision(iter2.second))
					{
						SetCollisionState(true);
						GetTransform()->SetPos(GetTransform()->GetPos() - direction * 7 * _dTimeDelta);
					}
					else
					{
						SetCollisionState(false);
					}
				}
			}
		}
		};

	auto handleCollisionWithStaticObject = [&](CModelObjects* pModelObject) {
		for (const auto& iter : GetColliderContainer())
		{
			for (const auto& iter2 : pModelObject->GetColliderContainer())
			{
				if (iter2.first == L"Main")
				{
					SetCollidedNormal(iter.second->GetCollisionNormal(iter2.second));
					if (GetCollidedNormal() != _float3::Zero)
					{
						_float speed = spGameInstance->GetDIKeyPressing(DIK_LSHIFT) ? 50.0f : 20.0f;
						ApplySlidingMovement(GetCollidedNormal(), speed, _dTimeDelta);
					}
				}
			}
		}
		};

	if (ePawnType == PAWNTYPE::PAWN_PLAYER)
	{
		UCharacter* pCharacter = static_cast<UCharacter*>(_pEnemy.get());
		handleCollisionWithPlayer(pCharacter);
	}
	else if (ePawnType == PAWNTYPE::PAWN_CHAR)
	{
		UCharacter* pCharacter = static_cast<UCharacter*>(_pEnemy.get());
		handleCollisionWithCharacter(pCharacter);
	}
	else if (ePawnType == PAWNTYPE::PAWN_STATICOBJ)
	{
		CModelObjects* pModelObject = static_cast<CModelObjects*>(_pEnemy.get());
		handleCollisionWithStaticObject(pModelObject);
	}

#ifdef _ENABLE_PROTOBUFF
	SendCollisionData();
#endif
}

