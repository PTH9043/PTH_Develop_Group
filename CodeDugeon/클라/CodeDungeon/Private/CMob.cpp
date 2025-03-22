#include "ClientDefines.h"
#include "UGameInstance.h"
#include "UPlayer.h"
#include "URenderer.h"
#include "CMainCamera.h"
#include "UTransform.h"
#include "UAnimModel.h"
#include "URegion.h"
#include "UNavigation.h"
#include "CMummyAnimController.h"
#include "UCell.h"
#include "UMethod.h"
#include "CMob.h"
#include "UCollider.h"
#include "UProcessedData.h"
#include "UAnimation.h"

CMob::CMob(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType)
	: UCharacter(_spDevice, _wstrLayer, _eCloneType),
	m_fDistancefromNearestPlayer{ 0 },
	m_bFoundTarget{ false },
	m_spTargetPlayer{ nullptr },
	m_f3TargetPos{},
	m_dtimeAccumulator{ 0 },
	m_delapsedTime{ 0 },
	m_fActivationRange{ 0 },
	m_fDeactivationRange{0},
	m_isSendDataToBehavior{false},
	m_isFirstFoundState{ false }
{
}

CMob::CMob(const CMob& _rhs)
	: UCharacter(_rhs), 
	m_fDistancefromNearestPlayer{ 0 },
	m_bFoundTarget{ false },
	m_spTargetPlayer{ nullptr },
	m_f3TargetPos{},
	m_dtimeAccumulator{ 0 },
	m_delapsedTime{ 0 },
	m_fActivationRange{ 0 },
	m_fDeactivationRange{ 0 },
	m_isSendDataToBehavior{ false },
	m_isFirstFoundState{ false }
{
}

void CMob::Free()
{
}

HRESULT CMob::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT CMob::NativeConstructClone(const VOIDDATAS& _Datas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_Datas), E_FAIL);	

#ifdef _ENABLE_PROTOBUFF
	MOBSERVERDATA MobServerData = UMethod::ConvertTemplate_Index<MOBSERVERDATA>(_Datas, MOBORDER);
	GetTransform()->SetNewWorldMtx(MobServerData.mWorldMatrix);
	SetNetworkID(MobServerData.iMobID);
	GetTransform()->SetScale({ 0.7f, 0.7f, 0.7f });
	GetAnimModel()->SetAnimation(MobServerData.iStartAnimIndex);
	GetCurrentNavi()->FindCell(GetTransform()->GetPos());
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	spGameInstance->AddCollisionPawnList(ThisShared<UPawn>());
	if (spGameInstance->GetNetworkOwnerID() == 61)
	{
		m_isSendDataToBehavior = true;
	}
#else
	GetTransform()->SetScale({ 0.7f, 0.7f, 0.7f });
	SetTargetPlayer(nullptr);
	m_isSendDataToBehavior = true;
#endif
	return S_OK;
}

void CMob::TickActive(const _double& _dTimeDelta)
{
	__super::TickActive(_dTimeDelta);

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	_float3 CurrentMobPos = GetTransform()->GetPos();

	if (nullptr == m_spTargetPlayer)
	{
		m_spTargetPlayer = spGameInstance->FindPlayerToDistance(CurrentMobPos);
	}

	if (m_spTargetPlayer)
	{
		_float3 CurrentPlayerPos = m_spTargetPlayer->GetTransform()->GetPos();
		CalculateDistanceBetweenPlayers(CurrentPlayerPos, CurrentMobPos);
		SearchForPlayers();
	}
}

void CMob::LateTickActive(const _double& _dTimeDelta)
{
	GetRenderer()->AddRenderGroup(RENDERID::RI_NONALPHA_LAST, GetShader(), ThisShared<UPawn>());
	if (true == IsSendDataToBehavior())
	{
		_float3 vPosition{ GetTransform()->GetPos() };
		SHPTR<UCell> newCell{};

		if (false == GetCurrentNavi()->IsMove(vPosition, REF_OUT newCell))
		{
			_float3 closestPoint = GetCurrentNavi()->ClampPositionToCell(vPosition);
			GetTransform()->SetPos(_float3(closestPoint.x, vPosition.y, closestPoint.z));
			vPosition = GetTransform()->GetPos();
		}

		_float newHeight = GetCurrentNavi()->ComputeHeight(vPosition);
		GetTransform()->SetPos(_float3(vPosition.x, newHeight, vPosition.z));

		__super::LateTickActive(_dTimeDelta);
	}
}

void CMob::NetworkTickActive(const _double& _dTimeDelta)
{
	_int CurAnimState = GetAnimationController()->GetAnimState();
	if (true == IsSendDataToBehavior() && CurAnimState != UAnimationController::ANIM_DEATH)
	{
		if (true == GetFoundTargetState() || true == m_isFirstFoundState)
		{
#ifdef _ENABLE_PROTOBUFF
			SendMobStateData();
#endif
			m_isFirstFoundState = true;
		}
	}
}

HRESULT CMob::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	return __super::RenderActive(_spCommand, _spTableDescriptor);
}

HRESULT CMob::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	return __super::RenderShadowActive(_spCommand, _spTableDescriptor);
}
HRESULT CMob::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	return __super::RenderOutlineActive(_spCommand, _spTableDescriptor,_pass);
}

void CMob::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
	if (true == IsDamaged())
	{
		SetDamaged(false);
	}
}

void CMob::ReceiveNetworkProcessData(const UProcessedData& _ProcessData)
{
#ifdef _ENABLE_PROTOBUFF

	switch (_ProcessData.GetDataType())
	{
	case TAG_SC_MONSTERSTATE:
	{
		if (true == IsSendDataToBehavior())
			return;

		MOBSTATE scMonsterState;
		scMonsterState.ParseFromArray(_ProcessData.GetData(), _ProcessData.GetDataSize());
		GetAnimationController()->ReceiveNetworkProcessData(&scMonsterState);
		GetTransform()->SetPos({ scMonsterState.posx(), scMonsterState.posy(), scMonsterState.posz() });
		GetTransform()->RotateFix({ scMonsterState.rotatex(), scMonsterState.rotatey(), scMonsterState.rotatez() });
		SetFoundTargetState(scMonsterState.foundon());
	}
	break;
	case TAG_SC_DAMAGED:
	{
		SetDamaged(true);
	}
	break;
	}
#endif
}

#ifdef _ENABLE_PROTOBUFF
void CMob::SendMobStateData()
{
	CSHPTRREF<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

	_float3 vCharacterPos = GetTransform()->GetPos();
	_float3 vCharRotate = GetTransform()->GetRotationValue();

	_int state = GetAnimationController()->GetAnimState();
	_int AnimIndex = GetAnimModel()->GetCurrentAnimIndex();
	_double Animtime = GetAnimModel()->GetCurrentAnimation()->GetTimeAcc();

	VECTOR3 vMove;
	VECTOR3 vRotate;
	{
		PROTOFUNC::MakeVector3(OUT & vMove, vCharacterPos.x, vCharacterPos.y, vCharacterPos.z);
		PROTOFUNC::MakeVector3(OUT & vRotate, vCharRotate.x, vCharRotate.y, vCharRotate.z);
	}
	MOBSTATE charMove;
	PROTOFUNC::MakeMobState(OUT & charMove, GetNetworkID(), vMove, vRotate,
		state, AnimIndex, IsDamaged(), GetFoundTargetState(), Animtime);
	spGameInstance->InsertSendProcessPacketInQuery(std::move(UProcessedData(charMove, TAG_CS_MONSTERSTATE)));
}

void CMob::SendCollisionData()
{
	RETURN_CHECK(false == IsDamaged(), ;);

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	COLLISIONDATA csCollision;
	VECTOR3 Pos;
	_llong NetworkID = GetNetworkID();
	{
		PROTOFUNC::MakeCollisionData(&csCollision, NetworkID, spGameInstance->GetNetworkOwnerID());
	}
	spGameInstance->InsertSendProcessPacketInQuery(UProcessedData(NetworkID, csCollision, TAG_CS_MONSTERCOLIISION));
}
#endif

void CMob::SearchForPlayers()
{
	if (m_fDistancefromNearestPlayer < m_fActivationRange)
		m_bFoundTarget = true;
	else if(m_fDistancefromNearestPlayer >= m_fDeactivationRange)
		m_bFoundTarget = false;
}

void CMob::CalculateDistanceBetweenPlayers(const _float3& _CurrentPlayerPos, const _float3& _CurrentMobPos)
{
	m_fDistancefromNearestPlayer = _float3::Distance(_CurrentMobPos, _CurrentPlayerPos);
}

void CMob::SetMobPlacement(_int _CellIndex)
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

	SHPTR<UNavigation> spNavigation = GetCurrentNavi();
	SHPTR<UCell> spCell = spNavigation->FindCell(_CellIndex);

	GetTransform()->SetPos(spCell->GetCenterPos());
}

void CMob::MoveAlongPath(const VECTOR<_float3>& path, size_t& currentPathIndex, const _double& _dTimeDelta)
{
    if (currentPathIndex >= path.size())
        return;

    _float3 currentPosition = GetTransform()->GetPos();
	_float3 targetPosition = (path)[currentPathIndex];

    _float3 direction = targetPosition - currentPosition;
    _float distance = direction.Length();
    direction.Normalize();

    if (distance < 1.0f)
    {
		currentPosition = targetPosition;
        currentPathIndex++;
    }
	m_f3TargetPos = targetPosition;
}

