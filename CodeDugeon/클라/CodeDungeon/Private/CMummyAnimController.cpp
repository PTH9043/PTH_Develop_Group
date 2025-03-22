#include "ClientDefines.h"
#include "CMummyAnimController.h"
#include "CMummy.h"
#include "UGameInstance.h"
#include "UAnimModel.h"
#include "UCharacter.h"
#include "UAnimation.h"
#include "UTransform.h"
#include "UPlayer.h"
#include "USound.h"

CMummyAnimController::CMummyAnimController(CSHPTRREF<UDevice> _spDevice)
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
    m_dRecvAnimDuration{ 0 },
    m_bPlayAttackSound1{false},
    m_bPlayAttackSound2{false},
    m_bPlayHitSound{ false }
{
}

CMummyAnimController::CMummyAnimController(const CMummyAnimController& _rhs)
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
    m_dRecvAnimDuration{ 0 },
    m_bPlayAttackSound1{false},
    m_bPlayAttackSound2{false},
    m_bPlayHitSound{ false }
{
}

void CMummyAnimController::Free()
{
}

HRESULT CMummyAnimController::NativeConstruct()
{
    return __super::NativeConstruct();
}

HRESULT CMummyAnimController::NativeConstructClone(const VOIDDATAS& _tDatas)
{
    RETURN_CHECK_FAILED(__super::NativeConstructClone(_tDatas), E_FAIL);

    m_wpMummyMob = std::dynamic_pointer_cast<CMummy>(GetOwnerCharacter());

    return S_OK;
}

void CMummyAnimController::Tick(const _double& _dTimeDelta)
{
    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis_patrol(0, 3);
    std::uniform_int_distribution<> dis_attack(0, 1);
    std::uniform_int_distribution<> dis_hit(0, 2);


    ClearTrigger();
    SetAnimState(-1);
    SHPTR<CMummy> spMummy = m_wpMummyMob.lock();
    SHPTR<UAnimModel> spAnimModel = spMummy->GetAnimModel();

    const _wstring& CurAnimName = spAnimModel->GetCurrentAnimation()->GetAnimName();

    _float DistanceFromPlayer = spMummy->GetDistanceFromPlayer();
    _bool FoundPlayer = spMummy->GetFoundTargetState();
    _bool Hit = false;

#ifndef _ENABLE_PROTOBUFF
    if (spMummy->GetPrevHealth() > spMummy->GetHealth())
        Hit = true;
#else
    if (true == spMummy->IsDamaged())
    {
        Hit = true;
        spMummy->SetDamaged(false);
    }
#endif
    _float AttackRange = 10.0f;

    // Handle found player state
    if (FoundPlayer && !spMummy->GetTargetPlayer()->GetDeathState())
    {
        if(!m_bFoundPlayerFirsttime)
        {
            UpdateState(spAnimModel, ANIM_AWAKE, L"WAKEUP");
            if (spMummy->GetMummyType() == CMummy::TYPE_LYING)
                spAnimModel->SetAnimation(L"openLaying");
            else if (spMummy->GetMummyType() == CMummy::TYPE_STANDING)
                spAnimModel->SetAnimation(L"openStanding");
            m_bFoundPlayerFirsttime = true;
            m_dIdleTimer = 0;
        }
    }
     if ((!FoundPlayer && m_bFoundPlayerFirsttime) || spMummy->GetTargetPlayer()->GetDeathState())
    {
        spAnimModel->UpdateAttackData(false, spAnimModel->GetAttackCollider());
        // Handle idle mode with 1/3 probability and 3-second duration
        m_bAttackMode = false;
        m_bTauntMode = false;
        //Idle Timer
        if (CurAnimName == L"idle")
        {
            m_dIdleTimer += _dTimeDelta;
            if (m_dIdleTimer >= 1.0)
            {
                m_dIdleTimer = 0.0;
            }

        }
        else
            m_dIdleTimer = 0;

        if (m_dIdleTimer == 0)
        {
            m_didleRandomValueChoosingTimer += _dTimeDelta;
            if (m_didleRandomValueChoosingTimer > 3)
            {
                m_iRandomValue = dis_patrol(gen);
            }

            if (m_iRandomValue == 0)
            {
                UpdateState(spAnimModel, ANIM_IDLE, L"IDLE");
            }

            else if (m_iRandomValue != 0)
            {
                UpdateState(spAnimModel, ANIM_MOVE, L"WALKF");
            }
        }
    }
    if (FoundPlayer && m_bFoundPlayerFirsttime && !m_bAttackMode && !spMummy->GetTargetPlayer()->GetDeathState())
    {
        m_bTauntMode = true;
        m_dIdleTimer = 0;
    }

    // Handle taunt mode state
    if (CurAnimName == L"openLaying" || CurAnimName == L"openStanding")
    {
        m_bTauntMode = true;
    }

    if (m_bTauntMode)
    {
        UpdateState(spAnimModel, ANIM_TAUNT, L"TAUNT");
        if (CurAnimName == L"taunt")
        {
            USound* TauntSound1 = spGameInstance->BringSound(L"Taunt_VO2").get();
            TauntSound1->PlayWithInputChannel(&m_pTauntChannel);

            m_bAttackMode = true;
            m_bTauntMode = false;
        }
    }

    // Handle hit state
    if (Hit)
    {
        spAnimModel->SetAnimation(L"gotHit");

        m_irandomNumforhit = dis_hit(gen);
        if (m_irandomNumforhit == 0)
        {
            USound* HitSound1 = spGameInstance->BringSound(L"enemy_hit1").get();
            HitSound1->PlayWithInputChannel(&m_pHitChannel);
        }
        else if (m_irandomNumforhit == 1)
        {
            USound* HitSound2 = spGameInstance->BringSound(L"enemy_hit2").get();
            HitSound2->PlayWithInputChannel(&m_pHitChannel);
        }
        else if (m_irandomNumforhit == 2)
        {
            USound* HitSound3 = spGameInstance->BringSound(L"enemy_hit3").get();
            HitSound3->PlayWithInputChannel(&m_pHitChannel);
        }

        USound* HitSound = spGameInstance->BringSound(L"GotHit_VO_2").get();
        HitSound->PlayWithInputChannel(&m_pHitChannel);
        spAnimModel->UpdateAttackData(false, spAnimModel->GetAttackCollider());
        spMummy->SetPrevHealth(spMummy->GetHealth());
    }

    // Handle attack mode state
    if (m_bAttackMode && !Hit)
    {
        if (DistanceFromPlayer > AttackRange)
        {
            UpdateState(spAnimModel, ANIM_MOVE, L"WALKF");
        }
        else
        {         
            if (CurAnimName == L"attack1")
                m_blastAttackWasFirst = false;
            else if(CurAnimName == L"attack2")
                m_blastAttackWasFirst = true;
            if (m_blastAttackWasFirst)
                UpdateState(spAnimModel, ANIM_ATTACK, L"ATTACK01");           
            else
                UpdateState(spAnimModel, ANIM_ATTACK, L"ATTACK02");
        }
    }


    // Check for death
    if (spMummy->GetHealth() <= 0)
    {
        spMummy->SetDeathState(true);
    }
     
    // Handle death state
    if (spMummy->GetDeathState())
    {
        spAnimModel->UpdateAttackData(false, spAnimModel->GetAttackCollider());
        UpdateState(spAnimModel, ANIM_DEATH, L"DEAD");
    }

    // Tick event
    spAnimModel->TickEvent(spMummy.get(), GetTrigger(), _dTimeDelta);
}