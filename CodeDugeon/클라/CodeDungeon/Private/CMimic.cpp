#include "ClientDefines.h"
#include "CMimic.h"
#include "UGameInstance.h"
#include "URenderer.h"
#include "CMainCamera.h"
#include "UTransform.h"
#include "UAnimModel.h"
#include "URegion.h"
#include "UNavigation.h"
#include "CMimicAnimController.h"
#include "UCell.h"
#include "UMethod.h"
#include "UPlayer.h"
#include "UCollider.h"
#include "UParticle.h"
#include "UParticleSystem.h"
#include "CModelObjects.h"
#include "UAnimation.h"
#include "UProcessedData.h"

CMimic::CMimic(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType)
	: CMob(_spDevice, _wstrLayer, _eCloneType), 
	m_PathFindingState{},
	m_AstarPath{},
	m_isPathFinding{ false },
	m_currentPathIndex{ 0 },
	m_bisOpen{false}
{
}

CMimic::CMimic(const CMimic& _rhs)
	: CMob(_rhs), 
	m_PathFindingState{},
	m_AstarPath{},
	m_isPathFinding{ false },
	m_currentPathIndex{ 0 },
	m_bisOpen{ false }
{
}

void CMimic::Free()
{
}

HRESULT CMimic::NativeConstruct()
{
	return __super::NativeConstruct();
}

void CMimic::CreateParticles()
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
		tDesc.ParticleParam.stGlobalParticleInfo.iMaxCount = 10;
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
		m_spBloodParticle->SetBloodTexture(0, L"blood"); 
		m_spBloodParticle->SetBloodTexture(1, L"blood1");
		m_spBloodParticle->SetBloodTexture(2, L"blood2");
		m_spBloodParticle->SetBloodTexture(3, L"blood3");
		m_spBloodParticle->SetBloodTexture(4, L"blood4");
		m_spBloodParticle->SetBloodTexture(5, L"blood5");
		m_spBloodParticle->SetBloodTexture(6, L"blood6");
		m_spBloodParticle->SetBloodTexture(7, L"blood7");
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
		m_spSlashParticle->SetTexture(L"Slash3");

		m_spSlashParticle->SetParticleType(PARTICLE_SLASH);
		*m_spSlashParticle->GetParticleSystem()->GetCreateInterval() = 0.35f;
		*m_spSlashParticle->GetParticleSystem()->GetAddParticleAmount() = 1;
	}


	{

		UParticle::PARTICLEDESC tDesc;
		tDesc.wstrParticleComputeShader = PROTO_RES_COMPUTEATTACKEFFECT2DSHADER;
		tDesc.wstrParticleShader = PROTO_RES_PARTICLEATTACK2DSHADER;


		tDesc.ParticleParam.stGlobalParticleInfo.fAccTime = 0.f;
		tDesc.ParticleParam.stGlobalParticleInfo.fDeltaTime = 2.f;
		tDesc.ParticleParam.stGlobalParticleInfo.fEndScaleParticle = 0.4;///0.4;//	 0.8f
		tDesc.ParticleParam.stGlobalParticleInfo.fStartScaleParticle = 0.2;///0.2;//	 1.0f
		tDesc.ParticleParam.stGlobalParticleInfo.fMaxLifeTime = 1.5f;
		tDesc.ParticleParam.stGlobalParticleInfo.fMinLifeTime = 0.1f;
		tDesc.ParticleParam.stGlobalParticleInfo.fMaxSpeed = 1.88;//3.25f
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
		*m_spAttackParticle->GetParticleSystem()->GetAddParticleAmount() = 4;
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
}

HRESULT CMimic::NativeConstructClone(const VOIDDATAS& _Datas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_Datas), E_FAIL);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

	CreateParticles();


	UCollider::COLLIDERDESC tDesc;
	tDesc.vTranslation = _float3(0.f, 0.f, 0.f);
	tDesc.vScale = _float3(0, 0, 0);
	SHPTR<UCollider> Collider = static_pointer_cast<UCollider>(spGameInstance->CloneComp(PROTO_COMP_OBBCOLLIDER, { &tDesc }));
	_wstring mainColliderTag = L"Main";

	AddColliderInContainer(mainColliderTag, Collider);
	for (auto& Colliders : GetColliderContainer())
	{
		Colliders.second->SetScale(_float3(3, 7, 3));
		Colliders.second->SetTranslate(_float3(0, 3, 0));
	}

	SetHealth(200);
	SetMaxHealth(200);
	SetActivationRange(10);
	SetDeactivationRange(120);
	SetOutlineByAbility(true);
	SetOutline(true);

	/*GetTransform()->SetPos(spCell->GetCenterPos());*/
	return S_OK;
}

void CMimic::TickActive(const _double& _dTimeDelta)
{

	__super::TickActive(_dTimeDelta);
	GetAnimationController()->Tick(_dTimeDelta);
	_float3 pos = GetTransform()->GetPos();

	pos.y += 5;
	m_spBloodParticle->SetPosition(pos);
	m_spSlashParticle->SetPosition(pos);
	_int CurAnimState = GetAnimationController()->GetAnimState();
	if (true == IsSendDataToBehavior())
	{
		_float3 CurrentMobPos = GetTransform()->GetPos();
		_float3 CurrentPlayerPos = GetTargetPlayer()->GetTransform()->GetPos();
		SHPTR<UCell> CurrentMobCell = GetCurrentNavi()->GetCurCell();
		SHPTR<UCell> CurrentPlayerCell = GetTargetPlayer()->GetCurrentNavi()->GetCurCell();
		const _wstring& CurAnimName = GetAnimModel()->GetCurrentAnimation()->GetAnimName();

		if (CurAnimState == UAnimationController::ANIM_MOVE)
		{
			AddTimeAccumulator(_dTimeDelta);

			// A* for moving towards player when player is found
			if (GetFoundTargetState() && !GetTargetPlayer()->GetDeathState())
			{
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
					GetTransform()->SetDirectionFixedUp(direction, _dTimeDelta, 5);
				}
				if (CurAnimName != L"Hit")
					GetTransform()->TranslateDir(-GetTransform()->GetLook(), _dTimeDelta, 20);
			}
			else // patrolling when player is not found
			{
				SHPTR<UNavigation> spNavigation = GetCurrentNavi();
				SHPTR<UCell> spNeighborCell = spNavigation->ChooseRandomNeighborCell(3);
				if (GetTimeAccumulator() >= 5.0)
				{
					m_PathFindingState = (spNavigation->StartPathFinding(CurrentMobPos, spNeighborCell->GetCenterPos(), CurrentMobCell, spNeighborCell));
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
							m_AstarPath = (spNavigation->OptimizePath(m_PathFindingState.path, CurrentMobPos, spNeighborCell->GetCenterPos()));
							m_currentPathIndex = 0; // index initialized when path is optimized
						}
					}
				}
				if (!m_AstarPath.empty())
				{
					MoveAlongPath(m_AstarPath, m_currentPathIndex, _dTimeDelta);
					_float3 direction = CurrentMobPos - GetTargetPos();
					GetTransform()->SetDirectionFixedUp(direction, _dTimeDelta, 5);
				}
				GetTransform()->TranslateDir(-GetTransform()->GetLook(), _dTimeDelta, 20);
			}
		}
		else if (CurAnimState == UAnimationController::ANIM_ATTACK)
		{
			_float3 direction = CurrentMobPos - CurrentPlayerPos;
			GetTransform()->SetDirectionFixedUp(direction, _dTimeDelta, 5);
		}

		if (CurAnimName == L"Hit")
		{
			GetTransform()->TranslateDir(GetTransform()->GetLook(), _dTimeDelta, 10);
		}

		SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

		// death animation
		if (CurAnimState == UAnimationController::ANIM_DEATH)
		{
			_double DeathAnimSpeed = 20;
			if (GetElapsedTime() == 0)
			{
				spGameInstance->SoundPlayOnce(L"mimic_death");
			}
			SetElapsedTime(GetElapsedTime() + (_dTimeDelta * DeathAnimSpeed));
			_double DeathTimeArcOpenEnd = 50;
			if (GetElapsedTime() < DeathTimeArcOpenEnd) {
				GetAnimModel()->TickAnimToTimeAccChangeTransform(GetTransform(), _dTimeDelta, GetElapsedTime());
				GetAnimModel()->UpdateDissolveTImer(_dTimeDelta * 1.2f);
				if (!m_bDissolveSound) {
					spGameInstance->SoundPlayOnce(L"DissolveSound3");
					m_bDissolveSound = true;
				}
			}

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
					spGameInstance->SoundPlayOnce(L"DissolveSound3");
					m_bDissolveSound = true;
				}
			}
		}
		else
		{
			m_bDissolveSound = false;
			GetAnimModel()->TickAnimation(_dTimeDelta);
			SetElapsedTime(0.0);
		}
	}
	UpdateCollision();
}

void CMimic::LateTickActive(const _double& _dTimeDelta)
{
	__super::LateTickActive(_dTimeDelta);

	if (true == IsSendDataToBehavior())
	{
		_float newHeight = GetCurrentNavi()->ComputeHeight(GetTransform()->GetPos());
		GetTransform()->SetPos(_float3(GetTransform()->GetPos().x, newHeight, GetTransform()->GetPos().z));
	}

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	_int CurAnimState = GetAnimationController()->GetAnimState();
	if (CurAnimState == UAnimationController::ANIM_DEATH)
	{
		if (GetElapsedTime() >= 100.0)
		{
			m_spBloodParticle->SetActive(false);
			m_spSlashParticle->SetActive(false);
			m_spAttackParticle->SetActive(false);
			m_spAttackParticleTwo->SetActive(false);
			SetActive(false);
			SetOutline(false);
			spGameInstance->RemoveCollisionPawn(ThisShared<CMob>());
		}
	}
}

HRESULT CMimic::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	const _wstring& CurAnimName = GetAnimModel()->GetCurrentAnimation()->GetAnimName();

	if(m_bisOpen)
		__super::RenderActive(_spCommand, _spTableDescriptor);

	return S_OK;
}

HRESULT CMimic::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	return __super::RenderShadowActive(_spCommand, _spTableDescriptor);
}

HRESULT CMimic::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	return __super::RenderOutlineActive(_spCommand, _spTableDescriptor,_pass);
}

void CMimic::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	PAWNTYPE ePawnType = _pEnemy->GetPawnType();
	const _wstring& CurAnimName = GetAnimModel()->GetCurrentAnimation()->GetAnimName();

	_float3 direction = (_pEnemy->GetTransform()->GetPos() - GetTransform()->GetPos());
	direction.Normalize();
	_bool IsHit = true;
#ifdef _ENABLE_PROTOBUFF
	_bool isCollision = false;
	_int DamageEnable = 0;
#endif
	if(m_bisOpen)
	{
		auto handleCollisionWithPlayer = [&](UCharacter* pCharacter) {
			for (const auto& iter : GetColliderContainer())
			{
				if (pCharacter->GetAnimModel()->IsCollisionAttackCollider(iter.second))
				{
					if (CurAnimName != L"death")
					{
						if (!GetIsHItAlreadyState())
						{
							m_spBloodParticle->SetActive(true);
							m_spSlashParticle->SetActive(true);
							m_spBloodParticle->GetParticleSystem()->GetParticleParam()->stGlobalParticleInfo.fAccTime = 0.f;
							m_spSlashParticle->GetParticleSystem()->GetParticleParam()->stGlobalParticleInfo.fAccTime = 0.f;
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
		}

#ifdef _ENABLE_PROTOBUFF
	SendCollisionData();
#endif
}
