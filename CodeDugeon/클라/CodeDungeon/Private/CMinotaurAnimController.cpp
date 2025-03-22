#include "ClientDefines.h"
#include "CMinotaurAnimController.h"
#include "CMinotaur.h"
#include "UGameInstance.h"
#include "UAnimModel.h"
#include "UCharacter.h"
#include "UAnimation.h"
#include "UTransform.h"
#include "UPlayer.h"
#include "USound.h"
#include "CWarriorPlayer.h"

CMinotaurAnimController::CMinotaurAnimController(CSHPTRREF<UDevice> _spDevice)
    : CMonsterAnimController(_spDevice),
    m_bAttackMode{ false },
    m_dlastHitTime{ 0 },
    m_dlastAttackTime{ 0 },
    m_bstartlastHitTime{ false },
    m_blastAttackWasFirst{ false },
    m_dIdleTimer{0},
    m_bRushMode{false},
    m_dRushModeTimer{ 0 },
    m_dRushAttackTimer{0},
    m_didleRandomValueChoosingTimer{ 0 },
    m_iRandomValue{0}
{
}

CMinotaurAnimController::CMinotaurAnimController(const CMinotaurAnimController& _rhs)
    : CMonsterAnimController(_rhs),
    m_bAttackMode{ false },
    m_dlastHitTime{ 0 },
    m_dlastAttackTime{ 0 },
    m_bstartlastHitTime{ false },
    m_blastAttackWasFirst{ false },
    m_dIdleTimer{0},
    m_bRushMode{ false },
    m_dRushModeTimer{ 0 },
    m_dRushAttackTimer{ 0 },
    m_didleRandomValueChoosingTimer{ 0 },
    m_iRandomValue{ 0 }
{
}

void CMinotaurAnimController::Free()
{
}

HRESULT CMinotaurAnimController::NativeConstruct()
{
    return __super::NativeConstruct();
}

HRESULT CMinotaurAnimController::NativeConstructClone(const VOIDDATAS& _tDatas)
{
    RETURN_CHECK_FAILED(__super::NativeConstructClone(_tDatas), E_FAIL);

    m_wpMinotaurMob = std::dynamic_pointer_cast<CMinotaur>(GetOwnerCharacter());

    return S_OK;
}

void CMinotaurAnimController::Tick(const _double& _dTimeDelta)
{
    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis_patrol(0, 3);
    std::uniform_int_distribution<> dis_attack(0, 3);
    std::uniform_int_distribution<> dis_hit(0, 2);

    ClearTrigger();
    SetAnimState(-1);

    SHPTR<CMinotaur> spMinotaur = m_wpMinotaurMob.lock();
    SHPTR<UAnimModel> spAnimModel = spMinotaur->GetAnimModel();
    const _wstring& CurAnimName = spAnimModel->GetCurrentAnimation()->GetAnimName();
    const _wstring& PlayerAnimName = spMinotaur->GetTargetPlayer()->GetAnimModel()->GetCurrentAnimation()->GetAnimName();
    _float playerKickedTime = static_pointer_cast<CWarriorPlayer>(spMinotaur->GetTargetPlayer())->GetWarriorKickedTimeElapsed();
    _float DistanceFromPlayer = spMinotaur->GetDistanceFromPlayer();
  
    if (spMinotaur->GetTargetPlayer()->GetDeathState())
    {
        spMinotaur->SetFoundTargetState(false);
    }


    _bool FoundPlayer = spMinotaur->GetFoundTargetState();
    _bool Hit = false;

#ifndef _ENABLE_PROTOBUFF
    if (spMinotaur->GetPrevHealth() > spMinotaur->GetHealth())
        Hit = true;
#else
    if (true == spMinotaur->IsDamaged())
    {
        Hit = true;
        spMinotaur->SetDamaged(false);
    }
#endif
    _float AttackRange = 10.0f;

    if (!FoundPlayer || spMinotaur->GetTargetPlayer()->GetDeathState())
    {
        spAnimModel->UpdateAttackData(false, spAnimModel->GetAttackCollider());
        m_bAttackMode = false;
        UpdateState(spAnimModel, ANIM_MOVE, L"WALK");
    }

    if(FoundPlayer && !spMinotaur->GetTargetPlayer()->GetDeathState())
    {
        if(!m_bRushMode)
            m_bAttackMode = true;
    }


    if (Hit)
    {
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

        if (m_iHitCount < 3)
        {
            spAnimModel->SetAnimation(L"hit_1");
            spAnimModel->UpdateAttackData(false, spAnimModel->GetAttackCollider());       
            m_iHitCount++;
        }

        if (m_iHitCount >= 3)
        {
            m_bisHitCooldown = true;
        }
        spMinotaur->SetPrevHealth(spMinotaur->GetHealth());
    }
   
    if(m_bisHitCooldown)
    {
        m_dHitCooldownTime += _dTimeDelta;

        if (m_dHitCooldownTime >= HIT_COOLDOWN_DURATION)
        {
            m_iHitCount = 0;
            m_dHitCooldownTime = 0;
            m_bisHitCooldown = false;
        }
    }

    // Handle attack mode state
    if (m_bAttackMode && !Hit && !m_bRushMode)
    {
        m_dlastAttackTime += _dTimeDelta;
        if (m_dlastAttackTime > 3.0)
        {
            m_blastAttackWasFirst = !m_blastAttackWasFirst;
            m_dlastAttackTime = 0;
        }

        if (DistanceFromPlayer > AttackRange)
        {
            UpdateState(spAnimModel, ANIM_MOVE, L"WALK2");
        }
        else
        {
            UpdateState(spAnimModel, 
                m_blastAttackWasFirst ? ANIM_ATTACK : ANIM_ATTACK, 
                m_blastAttackWasFirst ? L"ATTACK" : L"KICK_1");
        }

        if (PlayerAnimName == L"dead01" && playerKickedTime > 30)
        {
            m_bRushMode = true;
            m_bAttackMode = false;
            m_f3RushTargetPos = spMinotaur->GetTargetPlayer()->GetTransform()->GetPos();
        }
    }

    if (m_bRushMode)
    {
        m_dRushModeTimer += _dTimeDelta;
        if (m_dRushModeTimer < 2.0)
        {
            UpdateState(spAnimModel, ANIM_MOVE, L"WALKBACK");
        }
        else {
            if (_float3::Distance(spMinotaur->GetTransform()->GetPos(), m_f3RushTargetPos) > 10)
            {
                UpdateState(spAnimModel, ANIM_MOVE, L"RUN");
            }
            else
            {
                UpdateState(spAnimModel, ANIM_ATTACK, L"KICK_2");
                m_dRushAttackTimer += _dTimeDelta;
                if(m_dRushAttackTimer > 1.f)
                {
                    m_bRushMode = false;
                    m_dRushModeTimer = 0;
                    m_dRushAttackTimer = 0;
                }
            }
        }
    }

    // Check for death
    if (spMinotaur->GetHealth() <= 0)
    {
        spAnimModel->UpdateAttackData(false, spAnimModel->GetAttackCollider());
        spMinotaur->SetDeathState(true);
    }

    // Handle death state
    if (spMinotaur->GetDeathState())
    {
        UpdateState(spAnimModel, ANIM_DEATH, L"DEATH");
    }

    // Tick event
    spAnimModel->TickEvent(spMinotaur.get(), GetTrigger(), _dTimeDelta);
}
