#include "ClientDefines.h"
#include "CMob.h"
#include "UGameInstance.h"
#include "URenderer.h"
#include "CMainCamera.h"
#include "UTransform.h"
#include "UAnimModel.h"
#include "UMethod.h"
#include "CItemChest.h"
#include "CItemChestAnimController.h"
#include "UCollider.h"
#include "UPlayer.h"
#include "CWarriorPlayer.h"
#include "UParticle.h"
#include "UParticleSystem.h"

CItemChest::CItemChest(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType)
	: CMob(_spDevice, _wstrLayer, _eCloneType), m_bisOpen{false}, m_ChestType{}
{
}

CItemChest::CItemChest(const CItemChest& _rhs)
	: CMob(_rhs), m_bisOpen{false}, m_ChestType{}
{
}

void CItemChest::Free()
{
}

HRESULT CItemChest::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT CItemChest::NativeConstructClone(const VOIDDATAS& _Datas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_Datas), E_FAIL);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	SetPawnType(PAWNTYPE::PAWN_STATICOBJ);

	SetActivationRange(10);
	SetDeactivationRange(20);

	UCollider::COLLIDERDESC tDesc;
	tDesc.vTranslation = _float3(0.f, 0.f, 0.f);
	tDesc.vScale = _float3(1.f, 1.f, 1.f);
	SHPTR<UCollider> Collider = static_pointer_cast<UCollider>(spGameInstance->CloneComp(PROTO_COMP_OBBCOLLIDER, { &tDesc }));
	_wstring mainColliderTag = L"Main";
	AddColliderInContainer(mainColliderTag, Collider);
	GetAnimModel()->SetAnimation(L"Open");

	SetOutlineColor(_float3(0, 1, 0));
	for (auto& Containers : GetColliderContainer())
	{
		Containers.second->SetTranslate(GetAnimModel()->GetCenterPos());
		Containers.second->SetScaleToFitModel(GetAnimModel()->GetMinVertexPos(), GetAnimModel()->GetMaxVertexPos());
	}
	{
		UParticle::PARTICLEDESC tDesc;
		tDesc.wstrParticleComputeShader = PROTO_RES_COMPUTEOPENCHEST2DSHADER;
		tDesc.wstrParticleShader = PROTO_RES_PARTICLEOPENCHEST2DSHADER;


		tDesc.ParticleParam.stGlobalParticleInfo.fAccTime = 0.f;
		tDesc.ParticleParam.stGlobalParticleInfo.fDeltaTime = 2.f;
		tDesc.ParticleParam.stGlobalParticleInfo.fEndScaleParticle = 1.3f;
		tDesc.ParticleParam.stGlobalParticleInfo.fStartScaleParticle = 0.05f;
		tDesc.ParticleParam.stGlobalParticleInfo.fMaxLifeTime = 1.5f;
		tDesc.ParticleParam.stGlobalParticleInfo.fMinLifeTime = 0.1f;
		tDesc.ParticleParam.stGlobalParticleInfo.fMaxSpeed = 2.1f;
		tDesc.ParticleParam.stGlobalParticleInfo.fMinSpeed = 2.1f;
		tDesc.ParticleParam.stGlobalParticleInfo.iMaxCount = 512;
		tDesc.ParticleParam.stGlobalParticleInfo.fParticleThickness = 1.f;
		tDesc.ParticleParam.stGlobalParticleInfo.fParticleDirection = _float3(0.f, 0.f, 0.1f);
		tDesc.ParticleParam.stGlobalParticleInfo.fParticlePosition = _float3(0.f, 0.f, 0.f);
		tDesc.ParticleParam.stGlobalParticleInfo.fParticleKind = PARTICLE_OPENCHEST;
		m_spOpenChestParticle = std::static_pointer_cast<UParticle>(spGameInstance->CloneActorAdd(PROTO_ACTOR_PARTICLE, { &tDesc }));
	}
	m_spOpenChestParticle->GetParticleSystem()->GetParticleTypeParam()->fParticleType = PARTICLE_TYPE_DEFAULT;
	m_spOpenChestParticle->GetParticleSystem()->GetParticleTypeParam()->fParticleLifeTimeType = PARTICLE_LIFETIME_TYPE_DEFAULT;
	m_spOpenChestParticle->SetParticleType(PARTICLE_OPENCHEST);
	*m_spOpenChestParticle->GetParticleSystem()->GetAddParticleAmount() = 2;
	*m_spOpenChestParticle->GetParticleSystem()->GetCreateInterval() = 1.1f;
	m_spOpenChestParticle->SetTexture(L"Twinkle");
	
	return S_OK;
}

void CItemChest::TickActive(const _double& _dTimeDelta)
{
	__super::TickActive(_dTimeDelta);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

	static _double elapsedTime = 0;
	_double ItemChestOpeningSpeed = 2;
	_double ItemChestTimeArcOpenEnd = 3;
	GetAnimationController()->Tick(_dTimeDelta);
	SetTargetPlayer(std::static_pointer_cast<UPlayer>(spGameInstance->GetCurrPlayer()));
	//상자 여는 트리거
	if (GetFoundTargetState())
	{
		SetOutline(true);
		SetIfOutlineScale(true);
		if (!m_bisOpen)
			static_pointer_cast<CWarriorPlayer>(GetTargetPlayer())->SetCanInteractChestState(true);
		if (spGameInstance->GetDIKeyDown(DIK_F) && !m_bisOpen) {
			if (m_ChestType == TYPE_CHEST) {
				static_pointer_cast<CWarriorPlayer>(GetTargetPlayer())->SetIfOpenChest(true);
				spGameInstance->SoundPlayOnce(L"ChestOpen");
			}
			SetOpeningState(true);
		}
	}
	else
	{
		SetOutline(false);

		static_pointer_cast<CWarriorPlayer>(GetTargetPlayer())->SetCanInteractChestState(false);
	}

	if (m_bisOpen)
	{
		if (m_ChestType == TYPE_CHEST)
		{
			if (!m_bisOpenSound) {
				spGameInstance->SoundPlayOnce(L"Heal");
				m_bisOpenSound = true;
			}

			ParticleActiveTime += _dTimeDelta;
			if (GetElapsedTime() < ItemChestTimeArcOpenEnd)
			{
				SetElapsedTime(GetElapsedTime() + _dTimeDelta * ItemChestOpeningSpeed);
				GetAnimModel()->TickAnimToTimeAccChangeTransform(GetTransform(), _dTimeDelta, GetElapsedTime());
			}
			m_spOpenChestParticle->SetActive(true);
			m_spOpenChestParticle->SetPosition(GetTransform()->GetPos());
			m_spOpenChestParticle->GetParticleSystem()->GetParticleParam()->stGlobalParticleInfo.fAccTime = 0.f;
		}
		else
		{
			spGameInstance->RemoveCollisionPawn(ThisShared<CMob>());
			SetActive(false);
		}
	}

	if (ParticleActiveTime > 5.f) {
		m_spOpenChestParticle->SetActive(false);
	}

	for (auto& Containers : GetColliderContainer())
	{
		Containers.second->SetTransform(GetTransform());
	}
}

void CItemChest::LateTickActive(const _double& _dTimeDelta)
{
	GetRenderer()->AddRenderGroup(RENDERID::RI_NONALPHA_LAST, GetShader(), ThisShared<UPawn>());
	
	AddOutlineRenderGroup(RI_DEPTHRECORD);
	//AddNorPosRenderGroup(RI_NORPOS_FORABILITY);
	AddNorPosRenderGroup(RI_NORPOS);


	
	/*for (auto& Colliders : GetColliderContainer())
		Colliders.second->AddRenderer(RENDERID::RI_NONALPHA_LAST);*/
}

HRESULT CItemChest::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	return __super::RenderActive(_spCommand, _spTableDescriptor);
}

HRESULT CItemChest::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	return S_OK;
}

void CItemChest::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}
