#include "ClientDefines.h"
#include "CMimicAnimController.h"
#include "CMimic.h"
#include "UGameInstance.h"
#include "UAnimModel.h"
#include "UCharacter.h"
#include "UAnimation.h"
#include "UPlayer.h"
#include "UTransform.h"

CMimicAnimController::CMimicAnimController(CSHPTRREF<UDevice> _spDevice)
    : CMonsterAnimController(_spDevice),
    m_bAttackMode{ false },
    m_bTauntMode{ false },
    m_dlastHitTime{ 0 },
    m_dlastAttackTime{ 0 },
    m_bstartlastHitTime{ false },
    m_blastAttackWasFirst{ false },
    m_dIdleTimer{0},
    m_bFoundPlayerFirsttime{false},
    m_didleRandomValueChoosingTimer{0},
    m_iRandomValue{ 0 },
    m_dRecvAnimDuration{ 0 }
{
}

CMimicAnimController::CMimicAnimController(const CMimicAnimController& _rhs)
    : CMonsterAnimController(_rhs),
    m_bAttackMode{ false },
    m_bTauntMode{ false },
    m_dlastHitTime{ 0 },
    m_dlastAttackTime{ 0 },
    m_bstartlastHitTime{ false },
    m_blastAttackWasFirst{ false },
    m_dIdleTimer{0},
    m_bFoundPlayerFirsttime{ false },
    m_didleRandomValueChoosingTimer{ 0 },
    m_iRandomValue{ 0 },
    m_dRecvAnimDuration{ 0 }
{
}

void CMimicAnimController::Free()
{
}

HRESULT CMimicAnimController::NativeConstruct()
{
    return __super::NativeConstruct();
}

HRESULT CMimicAnimController::NativeConstructClone(const VOIDDATAS& _tDatas)
{
    RETURN_CHECK_FAILED(__super::NativeConstructClone(_tDatas), E_FAIL);

    m_wpMimicMob = std::dynamic_pointer_cast<CMimic>(GetOwnerCharacter());

    return S_OK;
}

void CMimicAnimController::Tick(const _double& _dTimeDelta)
{
    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis_patrol(0, 3);
    std::uniform_int_distribution<> dis_attack(0, 1);


    ClearTrigger();
    SetAnimState(-1);
    SHPTR<CMimic> spMimic = m_wpMimicMob.lock();
    SHPTR<UAnimModel> spAnimModel = spMimic->GetAnimModel();

    const _wstring& CurAnimName = spAnimModel->GetCurrentAnimation()->GetAnimName();

    _float DistanceFromPlayer = spMimic->GetDistanceFromPlayer();
    _bool FoundPlayer = spMimic->GetFoundTargetState();
    _bool Hit = false;

#ifndef _ENABLE_PROTOBUFF
    if (spMimic->GetPrevHealth() > spMimic->GetHealth())
        Hit = true;
#else
    if (true == spMimic->IsDamaged())
    {
        Hit = true;
        spMimic->SetDamaged(false);
    }
#endif
    _float AttackRange = 5.0f;

    if (spMimic->GetTargetPlayer()->GetDeathState())
    {
        spMimic->SetFoundTargetState(false);
    }

    // Handle found player state
    if (FoundPlayer && !spMimic->GetOpeningState())
    {
        if (spGameInstance->GetDIKeyDown(DIK_F)) {
            spMimic->SetOpeningState(true);
            spAnimModel->SetAnimation(L"Enter");
        }
    }
    if ((!FoundPlayer && spMimic->GetOpeningState()))
    {
        // Handle idle mode with 1/3 probability and 3-second duration
        m_bAttackMode = false;
        m_bTauntMode = false;
        //Idle Timer
        if (CurAnimName == L"Idle")
        {
            m_dIdleTimer += _dTimeDelta;
            if (m_dIdleTimer >= 2.0)
            {
                m_dIdleTimer = 0.0;
            }

        }
        else
            m_dIdleTimer = 0;

        if (m_dIdleTimer == 0)
        {
            m_didleRandomValueChoosingTimer += _dTimeDelta;
            if (m_didleRandomValueChoosingTimer > 2)
            {
                m_iRandomValue = dis_patrol(gen);
            }

            if (m_iRandomValue == 0)
            {
                UpdateState(spAnimModel, ANIM_IDLE, L"IDLE");
            }
            else if (m_iRandomValue != 0)
            {
                UpdateState(spAnimModel, ANIM_MOVE, L"WALK");
            }
        }
    }

    if (FoundPlayer && spMimic->GetOpeningState())
    {
        m_bAttackMode = true;
    }

    // Handle hit state
    if (Hit)
    {
        spAnimModel->SetAnimation(L"Hit");
        spAnimModel->UpdateAttackData(false, spAnimModel->GetAttackCollider());
        spMimic->SetPrevHealth(spMimic->GetHealth());
    }

    // Handle attack mode state
    if (m_bAttackMode && !Hit)
    {
        m_dlastAttackTime += _dTimeDelta;
        if (m_dlastAttackTime > 3.0)
        {
            m_blastAttackWasFirst = !m_blastAttackWasFirst;
            m_dlastAttackTime = 0;
        }

        if (DistanceFromPlayer > AttackRange)
        {
            UpdateState(spAnimModel, ANIM_MOVE, L"WALK");
        }
        else
        {
            UpdateState(spAnimModel, ANIM_ATTACK, L"ATTACK");
        }
    }

    // Check for death
    if (spMimic->GetHealth() <= 0)
    {
        spMimic->SetDeathState(true);
    }

    // Handle death state
    if (spMimic->GetDeathState())
    {
        spAnimModel->UpdateAttackData(false, spAnimModel->GetAttackCollider());
        UpdateState(spAnimModel, ANIM_DEATH, L"DEATH");
    }

    // Tick event
    spAnimModel->TickEvent(spMimic.get(), GetTrigger(), _dTimeDelta);
}
