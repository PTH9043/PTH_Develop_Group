#include "ClientDefines.h"
#include "CWarriorPlayer.h"
#include "CUserWarriorAnimController.h"
#include "UGameInstance.h"
#include "URenderer.h"
#include "CMainCamera.h"
#include "UTransform.h"
#include "UAnimModel.h"
#include "URegion.h"
#include "UNavigation.h"
#include "UCell.h"
#include "UParticle.h"
#include "UParticleSystem.h"
#include "CSword.h"
#include "UCollider.h"
#include "UTrail.h"
#include "UBlood.h"
#include "UVIBufferTrail.h"
#include "CModelObjects.h"
#include "UProcessedData.h"
#include "UMethod.h"
#include "UAnimation.h"
#include "UDust.h"
#include "UGuard.h"
#include "CShurikenThrowing.h"
#include "CAnubis.h"

CWarriorPlayer::CWarriorPlayer(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType)
	: UPlayer(_spDevice, _wstrLayer, _eCloneType), 
	m_spSword{ nullptr }, 
	m_bisCollisionWithObj{ false }, 
	isAttack{ false }, 
	m_stParticleType{}, 
	m_stParticleParam{},
	m_spFootPrintParticle{nullptr},
	m_spTrail{nullptr},
	m_spBlood{nullptr},
	m_bisKicked{ false },
	m_dKickedElapsed{ 0 },
	m_bisRise{ false },
	m_bCanInteractChest{ false },
	m_bCanInteractGuard{ false }
{
}

CWarriorPlayer::CWarriorPlayer(const CWarriorPlayer& _rhs) : 
	UPlayer(_rhs), 
	m_spSword{ nullptr },
	m_bisCollisionWithObj{ false },
	isAttack{ false },
	m_stParticleType{},
	m_stParticleParam{},
	m_spFootPrintParticle{ nullptr },
	m_spTrail{ nullptr },
	m_spBlood{ nullptr },
	m_bisKicked{ false },
	m_dKickedElapsed{ 0 },
	m_bisRise{ false },
	m_bCanInteractChest{ false },
	m_bCanInteractGuard{ false }
{
}

void CWarriorPlayer::Free()
{
}

HRESULT CWarriorPlayer::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT CWarriorPlayer::NativeConstructClone(const VOIDDATAS& _Datas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_Datas), E_FAIL);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

	SHPTR<UNavigation> spNavigation = GetCurrentNavi();
	//int cellIndex = 0;
	//int cellIndex = 349;

	int cellIndex = 0;

	SHPTR<UCell> spCell = spNavigation->FindCell(cellIndex);
	GetTransform()->SetPos(spCell->GetCenterPos());
	SetSpawnPoint(spCell);
	SetSpawnPoint(spCell->GetCenterPos());

	if (false == IsNetworkConnected())
	{
		SetSpawnPointCamera(static_pointer_cast<CMainCamera>(GetFollowCamera())->GetCurrentNavi()->FindCell(GetTransform()->GetPos()));

		SHPTR<CMainCamera> spMainCamera = std::static_pointer_cast<CMainCamera>(GetFollowCamera());
		if (nullptr != spMainCamera)
		{
			spMainCamera->SetMoveState(false);
		}
	}

	GetAnimModel()->SetAnimation(L"idle01");
	GetTransform()->SetScale({ 0.5f, 0.5f, 0.5f });
	SetMovingSpeed(50.f);
	SetRunningSpeed(100.f);
	SetRunState(false);

	UCollider::COLLIDERDESC tDesc;
	tDesc.vTranslation = _float3(0.f, 0.f, 0.f);
	tDesc.vScale = _float3(0, 0, 0);
	SHPTR<UCollider> Collider = static_pointer_cast<UCollider>(spGameInstance->CloneComp(PROTO_COMP_OBBCOLLIDER, { &tDesc }));
	_wstring mainColliderTag = L"Main";
	AddColliderInContainer(mainColliderTag, Collider);

	Collider = static_pointer_cast<UCollider>(spGameInstance->CloneComp(PROTO_COMP_OBBCOLLIDER, { &tDesc }));
	_wstring attackColliderTag = L"ForAttack";
	AddColliderInContainer(attackColliderTag, Collider);

	{
		UParticle::PARTICLEDESC tDesc;
		tDesc.wstrParticleComputeShader = PROTO_RES_COMPUTEFOOTPRINT2DSHADER;
		tDesc.wstrParticleShader = PROTO_RES_PARTICLEFOOTPRINT2DSHADER;


		tDesc.ParticleParam.stGlobalParticleInfo.fAccTime = 0.f;
		//tDesc.ParticleParam.stGlobalParticleInfo.fDeltaTime = 2.f;
		tDesc.ParticleParam.stGlobalParticleInfo.fEndScaleParticle =7.5f;// 6.5f
		tDesc.ParticleParam.stGlobalParticleInfo.fStartScaleParticle =4.5f; //3.1f;
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
		*m_spFootPrintParticle->GetParticleSystem()->GetAddParticleAmount() = 4;
		m_spFootPrintParticle->SetParticleType(PARTICLE_FOOTPRINT);
		m_spFootPrintParticle->SetActive(false);
	}
	//Heal Effect
	
	{	
		UParticle::PARTICLEDESC tDesc;
		tDesc.wstrParticleComputeShader = PROTO_RES_COMPUTEHEAL2DSHADER;
		tDesc.wstrParticleShader = PROTO_RES_PARTICLEHEAL2DSHADER;

		
		tDesc.ParticleParam.stGlobalParticleInfo.fAccTime = 0.f;
		//tDesc.ParticleParam.stGlobalParticleInfo.fDeltaTime = 2.f;
		tDesc.ParticleParam.stGlobalParticleInfo.fEndScaleParticle = 0.9f;
		tDesc.ParticleParam.stGlobalParticleInfo.fStartScaleParticle =0.05f;
		tDesc.ParticleParam.stGlobalParticleInfo.fMaxLifeTime = 2.0f;
		tDesc.ParticleParam.stGlobalParticleInfo.fMinLifeTime = 0.1f;
		tDesc.ParticleParam.stGlobalParticleInfo.fMaxSpeed = 1.99f;
		tDesc.ParticleParam.stGlobalParticleInfo.fMinSpeed = 1.99f;
		tDesc.ParticleParam.stGlobalParticleInfo.iMaxCount =512;
		tDesc.ParticleParam.stGlobalParticleInfo.fParticleThickness = 1.f;
		tDesc.ParticleParam.stGlobalParticleInfo.fParticleDirection = _float3(0.f, 0.f, 0.f);
		tDesc.ParticleParam.stGlobalParticleInfo.fParticlePosition = _float3(0.f, 0.f, 0.f);
		tDesc.ParticleParam.stGlobalParticleInfo.fParticleKind = PARTICLE_HEAL;
		m_spHealParticle = std::static_pointer_cast<UParticle>(spGameInstance->CloneActorAdd(PROTO_ACTOR_PARTICLE, { &tDesc }));		
		m_spHealParticle->GetParticleSystem()->GetParticleTypeParam()->fParticleType = PARTICLE_TYPE_DEFAULT;
		m_spHealParticle->GetParticleSystem()->GetParticleTypeParam()->fParticleLifeTimeType = PARTICLE_LIFETIME_TYPE_DEFAULT; 
		m_spHealParticle->SetParticleType(PARTICLE_HEAL);
		*m_spHealParticle->GetParticleSystem()->GetAddParticleAmount() = 1;
		*m_spHealParticle->GetParticleSystem()->GetCreateInterval() = 1.1f;
		m_spHealParticle->SetTexture(L"Twinkle");
	}
	{
		SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
		CSword::EQDESC Desc(std::static_pointer_cast<UModel>(spGameInstance->CloneResource(PROTO_RES_LONGSWORDMODEL)), ThisShared<UCharacter>(), L"..\\..\\Resource\\Model\\Item\\Equip\\Sword\\Convert\\EquipDesc\\sword_FBX.bin");
		m_spSword = std::static_pointer_cast<CSword>(spGameInstance->CloneActorAdd(PROTO_ACTOR_LONGSWORD, {&Desc}));
	}
	{
		UVIBufferTrail::TrailVertexCnt tDesc;
		tDesc.iMaxVertexCount = 100;
		m_spTrail = std::static_pointer_cast<UTrail>(spGameInstance->CloneActorAdd(PROTO_ACTOR_TRAIL, { &tDesc }));
		m_spTrail->SetActive(true);
		m_spTrail->SetColorTexture(L"elec");
		m_spTrail->SetTrailShapeTexture(L"Noise_Bee");
		m_spTrail->SetTrailNoiseTexture(L"GlowDiffuse");	
	}
	{
		m_spBlood = std::static_pointer_cast<UBlood>(spGameInstance->CloneActorAdd(PROTO_ACTOR_BLOOD));
	}
	/*{
		m_spDust = std::static_pointer_cast<UDust>(spGameInstance->CloneActorAdd(PROTO_ACTOR_DUST));
	}*/

	for (auto& Colliders : GetColliderContainer())
	{
		Colliders.second->SetScale(_float3(4, 10, 4));
		Colliders.second->SetTranslate(_float3(0, 10, 0));
	}
	SetOutline(true);
	SetifPlayer(true);//depth만 기록하고 outline안그리도록 다른 물체  outline depth판정위해
	//SetIfOutlineScale(false); 만약 플레이어 그릴거면 SetifPlayer->false, SetIfOutlineScale->true
	SetHealth(2500);
	SetMaxHealth(2500);
	SetAnimModelRim(true);
	SetAttack(100);
	return S_OK;
}

void CWarriorPlayer::ReceiveNetworkProcessData(const UProcessedData& _ProcessData)
{
//	if (_ProcessData.GetDataID() != GetNetworkID())
//		return;

#ifdef _ENABLE_PROTOBUFF

	switch (_ProcessData.GetDataType())
	{
	case TAG_SC_PLAYERSTATE:
	{
		CHARSTATE PlayerState;
		PlayerState.ParseFromArray(_ProcessData.GetData(), _ProcessData.GetDataSize());
		GetAnimationController()->ReceiveNetworkProcessData(&PlayerState);
		GetTransform()->SetPos(_float3{ PlayerState.posx(), PlayerState.posy(), PlayerState.posz() });
		GetTransform()->RotateFix(_float3{ PlayerState.rotatex(), PlayerState.rotatey(), PlayerState.rotatez() });
	}
	break;
	case TAG_SC_SELFPLAYERMOVE:
	{
		SC_SEEPLAYERMOVE selfPlayerMove;
		selfPlayerMove.ParseFromArray(_ProcessData.GetData(), _ProcessData.GetDataSize());
		// SelfPlayer 
		GetTransform()->SetPos(_float3{ selfPlayerMove.posx(), selfPlayerMove.posy(), selfPlayerMove.posz() });
	}
	break;
	}
#endif
}

void CWarriorPlayer::TickActive(const _double& _dTimeDelta)
{
	__super::TickActive(_dTimeDelta);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	
	JumpState(_dTimeDelta);
	GetAnimationController()->Tick(_dTimeDelta);

	//게임끝 트리거
	if (GetCurrentNavi()->GetCurIndex() == 1141)
		m_bisGameEnd = true;

	_int AnimState = GetAnimationController()->GetAnimState();
	const _wstring& CurAnimName = GetAnimModel()->GetCurrentAnimation()->GetAnimName();

	SHPTR<UCollider> ps = GetAnimModel()->BringAttackCollider(UCollider::TYPE_OBB);
	if (ps) {
		SHPTR<DirectX::BoundingOrientedBox> OBB = ps->GetOBB();

		_float3 plusPoint = ps->GetHeightAdjustedPointFromCenter(OBB, false);
		_float3 minusPoint = ps->GetHeightAdjustedPointFromCenter(OBB, true);
		_float4x4 AnimTransform = ps->GetTransformMatrix();
		m_spTrail->SetRenderingTrail(isAttack);
		m_spTrail->AddTrail(plusPoint, minusPoint);
	}
	
	m_spFootPrintParticle->SetActive(true);

	if (GetAnimationController()->GetAnimState() == CUserWarriorAnimController::ANIM_MOVE)  {//|| AnimState == CWarriorAnimController::ANIM_ATTACK|| AnimState == CWarriorAnimController::ANIM_COMBO
		*m_spFootPrintParticle->GetParticleSystem()->GetAddParticleAmount() =4;
		*m_spFootPrintParticle->GetParticleSystem()->GetCreateInterval() = 0.355f;
		_float3 pos = GetTransform()->GetPos() + GetTransform()->GetRight();
		pos.y += 1.0;
		_float3 Look = GetTransform()->GetLook();
		_float3 Right = 1.2 * GetTransform()->GetRight();
		//pos -= 3 * Look;
		m_spFootPrintParticle->SetPosition(pos);
		m_spFootPrintParticle->SetDirection(Right);
		SetAnimModelRimColor(_float3(204 / 204.f, 255 / 204.f, 0));
	}
	else {	
		*m_spFootPrintParticle->GetParticleSystem()->GetAddParticleAmount() = 0;
		*m_spFootPrintParticle->GetParticleSystem()->GetCreateInterval() = 0.8f;
		SetAnimModelRimColor(_float3(0,0, 0));
	}

	if (GetAnimationController()->GetAnimState() == CUserWarriorAnimController::ANIM_HIT) {
		if (false == IsNetworkConnected())
		{
			m_spBlood->SetActive(true);
			m_spBlood->SetTimer(1.75f);
		}
		m_fInteractionTimeElapsed = 0;
		m_bSetRimOn = true;
	}
	else {
	//	SetAnimModelRimColor(_float3(0, 0, 0));
	}

	if (m_bSetRimOn) {
		SetAnimModelRimColor(_float3(1, 0, 0));
		m_bSetRimTimeElapsed += _dTimeDelta;
		if (m_bSetRimTimeElapsed > 1.f) {
			m_bSetRimOn = false;
		}
	}
	else {
		m_bSetRimTimeElapsed = 0.f;
		//SetAnimModelRimColor(_float3(0, 0, 0));
	}
	if (false == IsNetworkConnected())
	{
		if (m_spBlood->CheckTimeOver()) {
			m_spBlood->SetActive(false);
		}
	}
	
	if (IfOpenChestForHeal) {//2.1초 지속
		HealTrigger = true;
		IncreaseHealth(10); //이렇게 하면 119정도 오름
		SetAnimModelRimColor(_float3(0, 1, 0));
		m_spHealParticle->SetActive(true);
		_float3 pos = GetTransform()->GetPos();
		pos.y += 2;
		
		m_spHealParticle->SetPosition(pos);
		m_spHealParticle->GetParticleSystem()->GetParticleParam()->stGlobalParticleInfo.fAccTime = 0.f;		
	}

	if (HealTrigger)
	{
		HealTimer += _dTimeDelta;
		if (HealTimer > 1.75f) {
			//SetAnimModelRimColor(_float3(1, 0, 0));
			m_spHealParticle->SetActive(false);
			HealTimer = 0;
			HealTrigger = false;
			IfOpenChestForHeal = false;
		}
	}

	// Rotation 
	{
		if (m_bStartedGame&& !m_bisGameEnd) {

			if (spGameInstance->GetDIKeyDown(DIK_F9))
			{
				m_isMouseEnable = !m_isMouseEnable;
				POINT ptCursorPos;
				ShowCursor(TRUE);
				spGameInstance->SetGamePause(m_isMouseEnable);
			}
			else if(false == m_isMouseEnable)
			{
				POINT ptCursorPos;
				ShowCursor(FALSE);
				SetCursorPos(1000, 400);
			}
		}
		if (m_bisGameEnd) {
			spGameInstance->PauseGame();
		}
	}

	if(CurAnimName != L"roll_back" && CurAnimName != L"roll_front"&& CurAnimName != L"roll_right"&& CurAnimName != L"roll_left"&&
		CurAnimName != L"jumpZ0" &&  CurAnimName != L"rise01"&& CurAnimName != L"rise02")
	{
		for (auto& Colliders : GetColliderContainer())
		{
			if(Colliders.first == L"ForAttack")
			{
				Colliders.second->SetScale(_float3(3, 10, 3));
				Colliders.second->SetTranslate(_float3(0, 10, 0));
				Colliders.second->SetTransform(GetTransform()->GetWorldMatrix());
			}
			else
			{
				Colliders.second->SetScale(_float3(4, 10, 4));
				Colliders.second->SetTranslate(_float3(0, 10, 0));
				Colliders.second->SetTransform(GetTransform()->GetWorldMatrix());
			}
		}
	}
	else
	{
		for (auto& Colliders : GetColliderContainer())
		{
			if (Colliders.first == L"ForAttack")
			{
				Colliders.second->SetScale(_float3(0, 0, 0));
				Colliders.second->SetTranslate(_float3(0, 10, 0));
				Colliders.second->SetTransform(GetTransform()->GetWorldMatrix());
			}
			else
			{
				Colliders.second->SetScale(_float3(4, 10, 4));
				Colliders.second->SetTranslate(_float3(0, 10, 0));
				Colliders.second->SetTransform(GetTransform()->GetWorldMatrix());
			}
		}
	}
	{
		
		if (m_fSaveCheckpointCount == 0) {
			m_spTrail->SetColorTexture(L"elec");
			m_spTrail->SetTrailShapeTexture(L"Noise_Bee");
			m_spTrail->SetTrailNoiseTexture(L"GlowDiffuse");			
		}
		else if (m_fSaveCheckpointCount == 1) {
			m_spTrail->SetColorTexture(L"FireRed2");
			m_spTrail->SetTrailShapeTexture(L"T_Sword_Slash_11");
			m_spTrail->SetTrailNoiseTexture(L"elec");			
		}
		else if (m_fSaveCheckpointCount == 2) {
			m_spTrail->SetColorTexture(L"Cloudy");
			m_spTrail->SetTrailShapeTexture(L"Noise_Bee");
			m_spTrail->SetTrailNoiseTexture(L"GlowDiffuse");			
		}
		else if (m_fSaveCheckpointCount == 3) {
			m_spTrail->SetColorTexture(L"Pink");
			m_spTrail->SetTrailShapeTexture(L"Noise_Thunder");
			m_spTrail->SetTrailNoiseTexture(L"GlowDiffuse");			
		}
		else {
			m_fSaveCheckpointCount = 0;
		}
	}

}

void CWarriorPlayer::LateTickActive(const _double& _dTimeDelta)
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	GetRenderer()->AddRenderGroup(RENDERID::RI_NONALPHA_LAST, GetShader(), ThisShared<UPawn>());
	RETURN_CHECK(true == IsNetworkConnected(), ;);

	__super::LateTickActive(_dTimeDelta);
	_float3 direction(0.0f, 0.0f, 0.0f);

	if (false == spGameInstance->IsFreeModeCameraEnable() &&  false == spGameInstance->IsGamePause())
	{
		FollowCameraMove(_float3{ 0.f, 20.f, -40.f }, _dTimeDelta);
		_float3 vCamPosition{ GetFollowCamera()->GetTransform()->GetPos() };
		SHPTR<UNavigation> CamNavi = static_pointer_cast<CMainCamera>(GetFollowCamera())->GetCurrentNavi();
		SHPTR<UCell> newCell{};
		if (_float3::Distance(GetFollowCamera()->GetTransform()->GetPos(), GetTransform()->GetPos()) < 30)
		{
			//카메라 네비 검사
			if (false == CamNavi->IsMove(vCamPosition, REF_OUT newCell))
			{
				_float3 closestPoint = CamNavi->ClampPositionToCell(vCamPosition);
				GetFollowCamera()->GetTransform()->SetPos(_float3(closestPoint.x, vCamPosition.y, closestPoint.z));
				vCamPosition = GetFollowCamera()->GetTransform()->GetPos();
			}
		}
		else
		{
			CamNavi->FindCell(GetTransform()->GetPos());
			GetFollowCamera()->GetTransform()->SetPos(GetTransform()->GetPos());
		}
	}
}

void CWarriorPlayer::NetworkTickActive(const _double& _dTimeDelta)
{
#ifdef _ENABLE_PROTOBUFF
	SendMoveData();
#endif
}

HRESULT CWarriorPlayer::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	return __super::RenderActive(_spCommand, _spTableDescriptor);
}

HRESULT CWarriorPlayer::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	return __super::RenderShadowActive(_spCommand, _spTableDescriptor);
}

HRESULT CWarriorPlayer::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor,_bool _pass)
{
	return __super::RenderOutlineActive(_spCommand, _spTableDescriptor,_pass);
}

void CWarriorPlayer::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
	if (true == IsDamaged())
	{
		SetDamaged(false);
	}

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	PAWNTYPE ePawnType = _pEnemy->GetPawnType();
	const _wstring& CurAnimName = GetAnimModel()->GetCurrentAnimation()->GetAnimName();

	if (PAWNTYPE::PAWN_CHAR == ePawnType)
	{
		UCharacter* pCharacter = static_cast<UCharacter*>(_pEnemy.get());
		_float3 direction = _pEnemy->GetTransform()->GetPos() - GetTransform()->GetPos();
		
		const _wstring& EnemyCurAnimName = pCharacter->GetAnimModel()->GetCurrentAnimation()->GetAnimName();
		SHPTR<CUserWarriorAnimController> spController = static_pointer_cast<CUserWarriorAnimController>(GetAnimationController());
		direction.Normalize();

		_float3 forward = GetTransform()->GetLook();
		_float dotProduct = forward.Dot(direction);

		for (auto& iter : GetColliderContainer())
		{
			if (iter.first == L"ForAttack")
			{
				if (pCharacter->GetAnimModel()->IsCollisionAttackCollider(iter.second))
				{
					if (EnemyCurAnimName == L"attack4_kick" || EnemyCurAnimName == L"attack5_kick"
						|| EnemyCurAnimName == L"Attack 2" || EnemyCurAnimName == L"Attack 3" || EnemyCurAnimName == L"Jump Forward")
					{
						if (CurAnimName != L"rise01" && CurAnimName != L"rise02" && CurAnimName != L"dead03")
						{
							if (m_dKickedElapsed >= 50)
							{
								m_dKickedElapsed = 0;
								m_bisKicked = false;
							}
							else
							{
								GetTransform()->SetDirectionFixedUp(pCharacter->GetTransform()->GetLook());
								m_bisKicked = true;
							}
							if (!GetIsHItAlreadyState())
							{
								DecreaseHealth(200);
							}
							SetHitAlreadyState(true);
						}
					}
					else
					{
						if (CurAnimName != L"rise01" && CurAnimName != L"rise02" && CurAnimName != L"dead01" && !m_bisKicked && CurAnimName != L"dead03")
						{
							if (!GetIsHItAlreadyState())
							{
								DecreaseHealth(50);
							}
							SetHitAlreadyState(true);
						}
					}
				}
				else
				{
					SetHitAlreadyState(false);
				}

				for (auto& iter2 : pCharacter->GetColliderContainer())
				{
					
					//아누비스 공격 충돌
					if (iter2.first == L"MagicSphere")
					{
						CAnubis* pAnubis = static_cast<CAnubis*>(_pEnemy.get());
						if (pAnubis->GetShieldState())
						{
							if (iter.second->IsCollision(iter2.second))
							{
								if (CurAnimName != L"rise01" && CurAnimName != L"rise02" && CurAnimName != L"dead03")
								{
									if (m_dKickedElapsed >= 50)
									{
										m_dKickedElapsed = 0;
										m_bisKicked = false;
									}
									else
									{
										GetTransform()->SetDirectionFixedUp(-pAnubis->GetTransform()->GetLook());
										m_bisKicked = true;
									}
									if (!GetIsHItAlreadyState())
									{
										DecreaseHealth(50);
									}
									SetHitAlreadyState(true);
								}
							}
							else
								SetHitAlreadyState(false);
						}
					}
					//아누비스 공격 충돌
					else if (iter2.first == L"MagicCircle")
					{
						CAnubis* pAnubis = static_cast<CAnubis*>(_pEnemy.get());
						if (pAnubis->GetFireAttackState())
						{
							if (iter.second->IsCollision(iter2.second))
							{
								if (CurAnimName != L"jumpZ0" && CurAnimName != L"rise01" && CurAnimName != L"rise02" && CurAnimName != L"dead01")
								{
									if (!GetIsHItAlreadyState())
									{
										DecreaseHealth(10);
									}
									SetHitAlreadyState(true);
								}
							}
							else
								SetHitAlreadyState(false);
						}
					}
				}
			}
		}
	}
	else if (PAWNTYPE::PAWN_STATICOBJ == ePawnType)
	{
		CModelObjects* pModelObject = static_cast<CModelObjects*>(_pEnemy.get());
		for (auto& iter : GetColliderContainer())
		{
			if (iter.first == L"Main")
			{
				for (auto& iter2 : pModelObject->GetColliderContainer())
				{
					if (iter2.first == L"Main")
					{
						SetCollidedNormal(iter.second->GetCollisionNormal(iter2.second));

						if (GetCollidedNormal() != _float3::Zero) // 충돌이 발생한 경우
						{
							SetOBJCollisionState(true);
							// 속도 결정
							_float speed = spGameInstance->GetDIKeyPressing(DIK_LSHIFT) ? 60.0f : 20.0f;
							if (CurAnimName == L"roll_back" || CurAnimName == L"roll_front" || CurAnimName == L"roll_left" || CurAnimName == L"roll_right")
								GetTransform()->SetPos(GetPrevPos());
							else
								ApplySlidingMovement(GetCollidedNormal(), speed, _dTimeDelta);
						}
						else
						{
							SetOBJCollisionState(false);
						}
					}
					else if (iter2.first == L"ForInteractionStatue")
					{
						if (iter.second->IsCollision(iter2.second))
						{
							m_bCanInteractStatue = true;
							pModelObject->SetOutline(true);
							if (pModelObject->GetCheckPointToOtherColor()) {
								m_bDoneInteractStatue = true;
							}
							else {
								m_bDoneInteractStatue = false;
							}
							//철장 여는 용도
							if (spGameInstance->GetDIKeyPressing(DIK_F)) {
								if (!CheckPointSaveSound && !m_bDoneInteractStatue) {
									spGameInstance->SoundPlayOnce(L"CheckpointSaving");
									CheckPointSaveSound = true;
								}
								m_fInteractionTimeElapsed += _dTimeDelta;
								
							}
							else {
								if (CheckPointSaveSound) {
									CheckPointSaveSound = false;
									spGameInstance->StopSound(L"CheckpointSaving");
								}
								m_fInteractionTimeElapsed = 0;
								
							}
							if (m_fInteractionTimeElapsed > 4.f&&!pModelObject->GetCheckPointToOtherColor()) {
								spGameInstance->SoundPlayOnce(L"CheckPointSave");
								m_fSaveCheckpointCount++;
								m_bSaveCheckpointStatue = true;
								pModelObject->SetInteractionState(true);
								pModelObject->SetCheckPointToOtherColor(true);
								SetSpawnPoint(GetCurrentNavi()->GetCurCell());
								SetSpawnPointCamera(static_pointer_cast<CMainCamera>(GetFollowCamera())->GetCurrentNavi()->GetCurCell());
								SetSpawnPoint(GetTransform()->GetPos());
								//pModelObject->SetOutline(false);
								m_fInteractionTimeElapsed = 0;
							}
								
						}
						else
						{
							pModelObject->SetOutline(false);
							m_bCanInteractStatue = false;
						}
					}
					else if (iter2.first == L"ForInteractionBars")
					{
						if (iter.second->IsCollision(iter2.second))
						{
							m_bCanInteractBar = true;
							pModelObject->SetOutline(true);

							if (pModelObject->GetCheckPointToOtherColor()) {
								m_bDoneInteractBar = true;
							}
							else {
								m_bDoneInteractBar = false;
							}

							//철장 여는 용도
							if (spGameInstance->GetDIKeyPressing(DIK_F)) {
								if (m_fInteractionTimeElapsed < 4.f) {//BarLift
									if (!LiftBarsSound&& !m_bDoneInteractBar) {
										spGameInstance->SoundPlayOnce(L"BarLift");
										LiftBarsSound = true;
									}
									
									m_fInteractionTimeElapsed += _dTimeDelta;
								}
							}
							else {
								if (LiftBarsSound) {
									LiftBarsSound = false;
									spGameInstance->StopSound(L"BarLift");
								}
								m_fInteractionTimeElapsed = 0;
							}
							if (m_fInteractionTimeElapsed > 3.99f) {
								spGameInstance->SoundPlayOnce(L"BarLiftStart");
								spGameInstance->SoundPlayOnce(L"BarLift2");						
								pModelObject->SetInteractionState(true);
								pModelObject->SetCheckPointToOtherColor(true);
								m_fInteractionTimeElapsed = 0;
								//pModelObject->SetOutline(false);
							}

						}
						else
						{
							pModelObject->SetOutline(false);
							m_bCanInteractBar = false;
						}
					}
					else if (iter2.first == L"ForInteractionCoreMinotaur")
					{
						if (iter.second->IsCollision(iter2.second))
						{
							m_bCanInteractCoreMinotaur = true;
							pModelObject->SetOutline(true);
							if (pModelObject->GetCheckPointToOtherColor()) {
								m_bDoneInteractCoreMinotaur = true;
							}
							else {
								m_bDoneInteractCoreMinotaur = false;
							}

							if (spGameInstance->GetDIKeyPressing(DIK_F)) {
								if (!MinotaurCoreSound&&!m_bDoneInteractCoreMinotaur) {
									spGameInstance->SoundPlayOnce(L"MinoCore");
									MinotaurCoreSound = true;
								}
								m_fInteractionTimeElapsed += _dTimeDelta;

							}
							else {
								if (MinotaurCoreSound) {
									MinotaurCoreSound = false;
									spGameInstance->StopSound(L"MinoCore");
								}
								m_fInteractionTimeElapsed = 0;

							}

							if (m_fInteractionTimeElapsed > 5.f && !pModelObject->GetCheckPointToOtherColor()) {
								spGameInstance->SoundPlayOnce(L"CoreComplete");
								m_bDeactivatedCoreMinotaur = true;
								pModelObject->SetInteractionState(true);
								pModelObject->SetCheckPointToOtherColor(true);				
								m_fInteractionTimeElapsed = 0;
							}
						}
						else
						{
							pModelObject->SetOutline(false);
							m_bCanInteractCoreMinotaur = false;
						}
					}
					else if (iter2.first == L"ForInteractionCoreHarlequinn")
					{
						if (iter.second->IsCollision(iter2.second))
						{
							m_bCanInteractCoreHarlequinn = true;
							pModelObject->SetOutline(true);
							if (pModelObject->GetCheckPointToOtherColor()) {
								m_bDoneInteractCoreHarlequinn = true;
							}
							else {
								m_bDoneInteractCoreHarlequinn = false;
							}
							if (spGameInstance->GetDIKeyPressing(DIK_F)) {
								if (!HalequinCoreSound && !m_bDoneInteractCoreHarlequinn) {
									spGameInstance->SoundPlayOnce(L"HalequinCore");
									HalequinCoreSound = true;
								}
								m_fInteractionTimeElapsed += _dTimeDelta;

							}
							else {
								if (HalequinCoreSound) {
									HalequinCoreSound = false;
									spGameInstance->StopSound(L"HalequinCore");
								}
								m_fInteractionTimeElapsed = 0;

							}
							if (m_fInteractionTimeElapsed > 5.f && !pModelObject->GetCheckPointToOtherColor()) {
								spGameInstance->SoundPlayOnce(L"CoreComplete2");
								m_bDeactivatedCoreHarlequinn = true;
								pModelObject->SetInteractionState(true);
								pModelObject->SetCheckPointToOtherColor(true);
								m_fInteractionTimeElapsed = 0;
							}
						}
						else
						{
							pModelObject->SetOutline(false);
							m_bCanInteractCoreHarlequinn = false;
						}
					}
					else if (iter2.first == L"ForInteractionCoreAnubis")
					{
						if (iter.second->IsCollision(iter2.second))
						{
							m_bCanInteractCoreAnubis = true;
							pModelObject->SetOutline(true);
							if (pModelObject->GetCheckPointToOtherColor()) {
								m_bDoneInteractCoreAnubis = true;
							}
							else {
								m_bDoneInteractCoreAnubis = false;
							}
							if (spGameInstance->GetDIKeyPressing(DIK_F)) {
								if (!AnubisCoreSound && !m_bDoneInteractCoreAnubis) {
									spGameInstance->SoundPlayOnce(L"AnubisCore");
									AnubisCoreSound = true;
								}
								m_fInteractionTimeElapsed += _dTimeDelta;

							}
							else {
								if (AnubisCoreSound) {
									AnubisCoreSound = false;
									spGameInstance->StopSound(L"AnubisCore");
								}
								m_fInteractionTimeElapsed = 0;

							}
							if (m_fInteractionTimeElapsed > 5.f && !pModelObject->GetCheckPointToOtherColor()) {
								spGameInstance->SoundPlayOnce(L"CoreComplete3");
								m_bDeactivatedCoreAnubis = true;
								pModelObject->SetInteractionState(true);
								pModelObject->SetCheckPointToOtherColor(true);
								m_fInteractionTimeElapsed = 0;
							}
						}
						else
						{
							pModelObject->SetOutline(false);
							m_bCanInteractCoreAnubis = false;
						}
						}
				}
			}
		}
	}
	else if (PAWNTYPE::PAWN_PROJECTILE == ePawnType)
	{
		CShurikenThrowing* pModelObject = static_cast<CShurikenThrowing*>(_pEnemy.get());
		for (auto& iter : GetColliderContainer())
		{
			if (iter.first == L"ForAttack")
			{
				for (auto& iter2 : pModelObject->GetColliderContainer())
				{
					if(pModelObject->GetThrowingState())
					{
						if (iter.second->IsCollision(iter2.second))
						{
							if (CurAnimName != L"rise01" && CurAnimName != L"rise02" && CurAnimName != L"dead01" && !m_bisKicked && CurAnimName != L"dead03")
							{
								if (!GetIsHItAlreadyState())
								{
									DecreaseHealth(100);
								}
								SetHitAlreadyState(true);
							}
						}
						else
						{
							SetHitAlreadyState(false);
						}
					}

				}
			}
		}
	}
	else if (PAWNTYPE::PAWN_ECT == ePawnType) //가드 충돌
	{
		UGuard* pGuard = static_cast<UGuard*>(_pEnemy.get());
		for (auto& iter : GetColliderContainer())
		{
			if (iter.first == L"Main")
			{
				for (auto& iter2 : pGuard->GetColliderContainer())
				{
					

					if (iter2.first == L"Main")
					{
						SetCollidedNormal(iter.second->GetCollisionNormal(iter2.second));

						if (GetCollidedNormal() != _float3::Zero) // 충돌이 발생한 경우
						{
							if (!GuardCollideSound) {
								spGameInstance->SoundPlayOnce(L"GuardCollideSound");
								GuardCollideSound = true;
							}
							if (CurAnimName != L"rise01" && CurAnimName != L"rise02" && CurAnimName != L"dead03")
							{
								if (m_dKickedElapsed >= 50)
								{
									m_dKickedElapsed = 0;
									m_bisKicked = false;
								}
								else
								{
									GetTransform()->SetDirectionFixedUp(-pGuard->GetTransform()->GetLook());
									m_bisKicked = true;
								}
						
							}
							SetOBJCollisionState(true);
							// 속도 결정
							_float speed = spGameInstance->GetDIKeyPressing(DIK_LSHIFT) ? 60.0f : 20.0f;
							if (CurAnimName == L"roll_back" || CurAnimName == L"roll_front" || CurAnimName == L"roll_left" || CurAnimName == L"roll_right"|| CurAnimName== L"jumpZ0")
								GetTransform()->SetPos(GetPrevPos());
							else
								ApplySlidingMovement(GetCollidedNormal(), speed, _dTimeDelta);
						}
						else
						{
							GuardCollideSound = false;
							SetOBJCollisionState(false);
						}
					}
					else if (iter2.first == L"ForInteractionGuard")
					{
						if (iter.second->IsCollision(iter2.second))
						{
							m_bCanInteractGuard = true;
							
						}
						else {
							
							m_bCanInteractGuard = false;
						}
							
						/*if (iter.second->IsCollision(iter2.second))
						{
							m_bCanInteractGuard = true;
						}
						else
						{
							m_bCanInteractGuard = false;
						}*/
					}

				}
			}
		}
	}
#ifdef _ENABLE_PROTOBUFF
	if (true == IsDamaged())
	{
		SendCollisionData(_pEnemy.get());
	}
#endif
}

#ifdef _ENABLE_PROTOBUFF
void CWarriorPlayer::SendMoveData()
{
	RETURN_CHECK(true == IsNetworkConnected(), ;);
	CSHPTRREF<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

	_float3 vCharacterPos = GetTransform()->GetPos();
	_float3 vCharRotate = GetTransform()->GetRotationValue();

	_int state = GetAnimationController()->GetAnimState();
	_int AnimIndex = GetAnimModel()->GetCurrentAnimIndex();

	VECTOR3 vMove;
	VECTOR3 vRotate; 
	{
		PROTOFUNC::MakeVector3(OUT & vMove, vCharacterPos.x, vCharacterPos.y, vCharacterPos.z);
		PROTOFUNC::MakeVector3(OUT & vRotate, vCharRotate.x, vCharRotate.y, vCharRotate.z);
	}
	CHARSTATE charMove;
	PROTOFUNC::MakeCharState(OUT & charMove, spGameInstance->GetNetworkOwnerID(), vMove, vRotate, 
		state, AnimIndex, IsDamaged() == true ? 1 : 0);
	spGameInstance->InsertSendProcessPacketInQuery(std::move(UProcessedData(charMove, TAG_CS_PLAYERSTATE)));
}

void CWarriorPlayer::SendCollisionData(UPawn* _pPawn)
{
	if (true == IsDamaged())
	{
		SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
		COLLISIONDATA csCollision;
		_llong NetworkID = spGameInstance->GetNetworkOwnerID();
		{
			PROTOFUNC::MakeCollisionData(&csCollision, NetworkID, _pPawn->GetNetworkID());
		}
		spGameInstance->InsertSendProcessPacketInQuery(UProcessedData(NetworkID, csCollision, TAG_CS_PLAYERCOLLISION));
	}
}

#endif

void CWarriorPlayer::TranslateStateMoveAndRunF(CSHPTRREF<UGameInstance> _spGameInstance, const _double& _dTimeDelta, const _float _fSpeed)
{
	RETURN_CHECK(true == IsNetworkConnected(), ;);

	if (_spGameInstance->GetDIKeyPressing(DIK_W))
	{
		GetTransform()->MoveForward(_dTimeDelta, _fSpeed);
	}
}

_float CWarriorPlayer::GetUltAttackOneCoolTime()
{
	RETURN_CHECK(true == IsNetworkConnected(), 0.f);
	SHPTR<CUserWarriorAnimController> spController = static_pointer_cast<CUserWarriorAnimController>(GetAnimationController());
	return spController->GetUltAttackOneCoolTime();
}

_float CWarriorPlayer::GetUltAttackTwoCoolTime()
{
	RETURN_CHECK(true == IsNetworkConnected(), 0.f);
	SHPTR<CUserWarriorAnimController> spController = static_pointer_cast<CUserWarriorAnimController>(GetAnimationController());
	return spController->GetUltAttackTwoCoolTime();
}

_float CWarriorPlayer::GetShortAttackCoolTime()
{
	RETURN_CHECK(true == IsNetworkConnected(), 0.f);
	SHPTR<CUserWarriorAnimController> spController = static_pointer_cast<CUserWarriorAnimController>(GetAnimationController());
	return spController->GetShortAttackCoolTime();
}

_bool CWarriorPlayer::GetBlindEffectBool()
{
	RETURN_CHECK(true == IsNetworkConnected(), false);
	SHPTR<CUserWarriorAnimController> spController = static_pointer_cast<CUserWarriorAnimController>(GetAnimationController());
	return spController->GetBlindEffectBool();
}

_bool CWarriorPlayer::GetDieEffectBool()
{
	RETURN_CHECK(true == IsNetworkConnected(), false);
	SHPTR<CUserWarriorAnimController> spController = static_pointer_cast<CUserWarriorAnimController>(GetAnimationController());
	return spController->GetDieEffectBool();
}

