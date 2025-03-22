#include "ClientDefines.h"
#include "CAnubisAnimController.h"
#include "CAnubis.h"
#include "UGameInstance.h"
#include "UAnimModel.h"
#include "UCharacter.h"
#include "UAnimation.h"
#include "UTransform.h"
#include "UGuard.h"
#include "UPlayer.h"
#include "UMat.h"
#include "USound.h"

CAnubisAnimController::CAnubisAnimController(CSHPTRREF<UDevice> _spDevice)
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
    m_iRandomValueforAttack{ 0 },
    m_bAttackStart{ false },
    m_dTimerForFireCircle{0},
    m_dShieldCooltime{3}
{
}

CAnubisAnimController::CAnubisAnimController(const CAnubisAnimController& _rhs)
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
    m_iRandomValueforAttack{ 0 },
    m_bAttackStart{ false },
    m_dTimerForFireCircle{0},
    m_dShieldCooltime{3}
{
}

void CAnubisAnimController::Free()
{
}

HRESULT CAnubisAnimController::NativeConstruct()
{
    return __super::NativeConstruct();
}

HRESULT CAnubisAnimController::NativeConstructClone(const VOIDDATAS& _tDatas)
{
    RETURN_CHECK_FAILED(__super::NativeConstructClone(_tDatas), E_FAIL);

    m_wpAnubisMob = std::dynamic_pointer_cast<CAnubis>(GetOwnerCharacter());

    return S_OK;
}

void CAnubisAnimController::Tick(const _double& _dTimeDelta)
{
    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis_attack(0, 1);
    std::uniform_int_distribution<> dis_hit(0, 2);


    ClearTrigger();
    SetAnimState(-1);
    SHPTR<CAnubis> spAnubis = m_wpAnubisMob.lock();
    SHPTR<UAnimModel> spAnimModel = spAnubis->GetAnimModel();

    _float AttackRange_Long = 40;
    _float AttackRange_Close = 15.0f;

    if (spAnubis->GetTargetPlayer()->GetDeathState())
    {
        spAnubis->SetFoundTargetState(false);
    }

    const _wstring& CurAnimName = spAnimModel->GetCurrentAnimation()->GetAnimName();

    _float DistanceFromPlayer = spAnubis->GetDistanceFromPlayer();
    _bool FoundPlayer = spAnubis->GetFoundTargetState();
    _bool Hit = false;
#ifndef _ENABLE_PROTOBUFF
    if (spAnubis->GetPrevHealth() > spAnubis->GetHealth())
        Hit = true;
#else
    if (true == spAnubis->IsDamaged())
    {
        Hit = true;
        spAnubis->SetDamaged(false);
    }
#endif

    _float AttackRange = 10.0f;

    // Handle found player state
    if (FoundPlayer && !spAnubis->GetTargetPlayer()->GetDeathState())
    {
        if(!m_bFoundPlayerFirsttime)
        {
            UpdateState(spAnimModel, ANIM_AWAKE, L"TOATTACKIDLE");
            spAnimModel->SetAnimation(L"Guard Idle to Attack Idle");
            m_bFoundPlayerFirsttime = true;
        }
        else
        {
            if (CurAnimName == L"Guard Idle to Attack Idle" || CurAnimName == L"Walk")
                m_bAttackMode = true;
        }
    }
    if ((!FoundPlayer) && m_bFoundPlayerFirsttime)
    {
        m_bAttackMode = false;
        m_bAttackStart = false;

        if (_float3::Distance(spAnubis->GetTransform()->GetPos(), spAnubis->GetOriginPos()) > 2)
        {
            UpdateState(spAnimModel, ANIM_MOVE, L"WALK");
        }
        else
        {
            UpdateState(spAnimModel, ANIM_AWAKE, L"TOGUARDIDLE");
            spAnimModel->SetAnimation(L"Attack Idle to Guard Idle");
            m_bFoundPlayerFirsttime = false;
            m_dShieldCooltime = 3;
        }
    }

    if(!m_bFoundPlayerFirsttime)
    {
        if (CurAnimName == L"Attack Idle to Guard Idle")
        {
            if (spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() >= 0.9)
            {
                UpdateState(spAnimModel, ANIM_IDLE, L"GUARDIDLE");
            }
        }
        else if (CurAnimName == L"Guard Idle")
            spAnubis->GetTransform()->SetDirectionFixedUp(spAnubis->GetOriginDirection(), _dTimeDelta, 5);
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
        else if(m_irandomNumforhit == 2)
        {
            USound* HitSound3 = spGameInstance->BringSound(L"enemy_hit3").get();
            HitSound3->PlayWithInputChannel(&m_pHitChannel);
        }

        if (m_iHitCount < 3)
        {
            spAnimModel->SetAnimation(L"Hit Reaction");
            spAnimModel->UpdateAttackData(false, spAnimModel->GetAttackCollider());
            m_iHitCount++;
        }

        if (m_iHitCount >= 3)
        {
            m_bisHitCooldown = true;
        }
        spAnubis->SetPrevHealth(spAnubis->GetHealth());
    }

    if (m_bAttackMode && !Hit)
    {
        UpdateState(spAnimModel, ANIM_IDLE, L"ATTACKIDLE");
        if (spAnubis->GetShieldState() == false)
        {
            m_dShieldCooltime += _dTimeDelta;
        }
  
        if (!m_bAttackStart)
        {
            spAnimModel->SetAnimation(L"Attack Idle");
            m_bAttackStart = true;
        }

        if (m_bAttackStart)
        {
            if (spAnubis->GetShieldState() == false && m_dShieldCooltime > 3)
            {
                UpdateState(spAnimModel, ANIM_ATTACK, L"ATTACK3");
            }
            else
            {
                if (DistanceFromPlayer > AttackRange_Long)
                {
                    UpdateState(spAnimModel, ANIM_MOVE, L"WALK");
                }
                else if (DistanceFromPlayer < AttackRange_Long && DistanceFromPlayer > AttackRange_Close)
                {
                    if((spAnubis->GetTargetPlayer()->GetTransform()->GetPos().y - spAnubis->GetTransform()->GetPos().y) > 0.2 ||
                        (spAnubis->GetTargetPlayer()->GetTransform()->GetPos().y - spAnubis->GetTransform()->GetPos().y) < -0.2)
                        UpdateState(spAnimModel, ANIM_MOVE, L"WALK");
                    else
                        UpdateState(spAnimModel, ANIM_ATTACK, L"CAST");
                }
                else if (DistanceFromPlayer < AttackRange_Close)
                {
                    if (spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() >= 0.9)
                        m_iRandomValueforAttack = dis_attack(gen);

                    if (m_iRandomValueforAttack == 0)
                        UpdateState(spAnimModel, ANIM_ATTACK, L"ATTACK1");
                    if (m_iRandomValueforAttack == 1)
                        UpdateState(spAnimModel, ANIM_ATTACK, L"ATTACK4");
                }
            }
        }
    }

    if (CurAnimName == L"Attack3")
    {
        if (spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() >= 0.65 && spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() < 0.66)
        {
            m_dShieldCooltime = 0;
            spAnubis->SetShieldState(true);
        }
    }

    if (CurAnimName == L"Cast")
    {
        if (spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() >= 0.65 && spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() < 0.66)
        {
            spAnubis->SetFireAttackState(true); 
        }
    }
    if (spAnubis->GetFireAttackState())
    {
        if (m_dTimerForFireCircle == 0)
        {
            spGameInstance->SoundPlayOnce(L"FireWarm");
        }
        m_dTimerForFireCircle += _dTimeDelta;
        float scale_factor = 1 + 100 * m_dTimerForFireCircle * m_dTimerForFireCircle;
        if(m_dTimerForFireCircle < 1)
        {
            spAnubis->GetFireCircle()->GetTransform()->SetScale(_float3(scale_factor, scale_factor, 2));
            spAnubis->GetFireCircle()->SetActive(true);
            spAnubis->GetFireCircle1()->GetTransform()->SetScale(_float3(scale_factor - 5, scale_factor - 5, 2));
            spAnubis->GetFireCircle1()->SetActive(true);
        }
        else
        {
            m_dTimerForFireCircle = 0;
            spAnubis->GetFireCircle()->GetTransform()->SetScale(_float3(1, 1, 1));
            spAnubis->GetFireCircle()->SetActive(false);
            spAnubis->GetFireCircle1()->GetTransform()->SetScale(_float3(1, 1, 1));
            spAnubis->GetFireCircle1()->SetActive(false);
            spAnubis->SetFireAttackState(false);
        }
    }

    // Check for death
    if (spAnubis->GetHealth() <= 0)
    {
        spAnubis->SetDeathState(true);
    }

    // Handle death state
    if (spAnubis->GetDeathState())
    {
        spAnimModel->UpdateAttackData(false, spAnimModel->GetAttackCollider());
        UpdateState(spAnimModel, ANIM_DEATH, L"DEATH");
    }

    // Tick event
    spAnimModel->TickEvent(spAnubis.get(), GetTrigger(), _dTimeDelta);
}