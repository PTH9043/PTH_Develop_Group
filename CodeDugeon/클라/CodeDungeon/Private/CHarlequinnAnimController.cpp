#include "ClientDefines.h"
#include "CHarlequinnAnimController.h"
#include "CHarlequinn.h"
#include "UGameInstance.h"
#include "UAnimModel.h"
#include "UCharacter.h"
#include "UAnimation.h"
#include "UTransform.h"
#include "UPlayer.h"
#include "CWarriorPlayer.h"
#include "USound.h"
#include "CShurikenThrowing.h"

CHarlequinnAnimController::CHarlequinnAnimController(CSHPTRREF<UDevice> _spDevice)
    : CMonsterAnimController(_spDevice),
    m_bAttackMode{ false },
    m_dlastHitTime{ 0 },
    m_dlastAttackTime{ 0 },
    m_bstartlastHitTime{ false },
    m_blastAttackWasFirst{ false },
    m_dIdleTimer{ 0 },
    m_didleRandomValueChoosingTimerforPatrol{ 0 },
    m_bAttack1FirstTime{ false },
    m_iRandomValueforPatrol{ 0 },
    m_bTauntMode{ false },
    m_f3ThrowingPos{},
    m_arrThrowingDir{},
    m_bDodge{false},
    m_bWillWalkUntilCloseRange{false} ,
    m_bWillJumpToCloseRange{false},
    m_iRandomValueforDodge{}
{
}

CHarlequinnAnimController::CHarlequinnAnimController(const CHarlequinnAnimController& _rhs)
    : CMonsterAnimController(_rhs),
    m_bAttackMode{ false },
    m_dlastHitTime{ 0 },
    m_dlastAttackTime{ 0 },
    m_bstartlastHitTime{ false },
    m_blastAttackWasFirst{ false },
    m_dIdleTimer{0},
    m_didleRandomValueChoosingTimerforPatrol{ 0 },
    m_bAttack1FirstTime{ false },
    m_iRandomValueforPatrol{ 0 },
    m_bTauntMode{ false },
    m_f3ThrowingPos{},
    m_arrThrowingDir{},
    m_bDodge{ false },
    m_bWillWalkUntilCloseRange{ false },
    m_bWillJumpToCloseRange{ false },
    m_iRandomValueforDodge{}
{
}

void CHarlequinnAnimController::Free()
{
}

HRESULT CHarlequinnAnimController::NativeConstruct()
{
    return __super::NativeConstruct();
}

HRESULT CHarlequinnAnimController::NativeConstructClone(const VOIDDATAS& _tDatas)
{
    RETURN_CHECK_FAILED(__super::NativeConstructClone(_tDatas), E_FAIL);

    m_wpHarlequinnMob = std::dynamic_pointer_cast<CHarlequinn>(GetOwnerCharacter());

    return S_OK;
}

void CHarlequinnAnimController::Tick(const _double& _dTimeDelta)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis_patrol(0, 3);  
    std::uniform_int_distribution<> dis_attack(0, 3);  
    std::uniform_int_distribution<> dis_dodge(0, 1);   
    std::uniform_int_distribution<> dis_hit(0, 2);

    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

    ClearTrigger();
    SetAnimState(-1);

    SHPTR<CHarlequinn> spHarlequinn = m_wpHarlequinnMob.lock();
    SHPTR<UAnimModel> spAnimModel = spHarlequinn->GetAnimModel();
    const _wstring& CurAnimName = spAnimModel->GetCurrentAnimation()->GetAnimName();
    const _wstring& PlayerAnimName = spHarlequinn->GetTargetPlayer()->GetAnimModel()->GetCurrentAnimation()->GetAnimName();
    _float playerKickedTime = static_pointer_cast<CWarriorPlayer>(spHarlequinn->GetTargetPlayer())->GetWarriorKickedTimeElapsed();
    _float DistanceFromPlayer = spHarlequinn->GetDistanceFromPlayer();

    _float3 directionToPlayer = spHarlequinn->GetTransform()->GetPos() - spHarlequinn->GetTargetPlayer()->GetTransform()->GetPos();

    if (spHarlequinn->GetTargetPlayer()->GetDeathState())
    {
        spHarlequinn->SetFoundTargetState(false);
    }

    _bool FoundPlayer = spHarlequinn->GetFoundTargetState();
    _bool Hit = false;

#ifndef _ENABLE_PROTOBUFF
    if (spHarlequinn->GetPrevHealth() > spHarlequinn->GetHealth())
        Hit = true;
#else
    if (true == spHarlequinn->IsDamaged())
    {
        Hit = true;
        spHarlequinn->SetDamaged(false);
    }
#endif

    _float AttackRange_Long = 50;
    _float AttackRange_Jump = 40;
    _float AttackRange_Close = 10.0f;
    // Handle found player state
    if (!FoundPlayer || spHarlequinn->GetTargetPlayer()->GetDeathState())
    {
        spAnimModel->UpdateAttackData(false, spAnimModel->GetAttackCollider());
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
            m_didleRandomValueChoosingTimerforPatrol += _dTimeDelta;
            if (m_didleRandomValueChoosingTimerforPatrol > 2)
            {
                m_iRandomValueforPatrol = dis_patrol(gen); // Use the random generator
                m_didleRandomValueChoosingTimerforPatrol = 0;
            }

            if (m_iRandomValueforPatrol == 0)
            {
                UpdateState(spAnimModel, ANIM_IDLE, L"IDLE");
            }
            else if (m_iRandomValueforPatrol != 0)
            {
                UpdateState(spAnimModel, ANIM_MOVE, L"WALK");
            }
        }

        if (CurAnimName == L"Crouch Idle")
        {
           spAnimModel->SetAnimation(L"idle");
           UpdateState(spAnimModel, ANIM_IDLE, L"IDLE");
        }
    }
    if (FoundPlayer && !m_bAttackMode && !spHarlequinn->GetTargetPlayer()->GetDeathState())
    {
        m_bTauntMode = true;
        m_dIdleTimer = 0;
    }




    if (m_bTauntMode)
    {
        UpdateState(spAnimModel, ANIM_TAUNT, L"TAUNT");
        if (CurAnimName == L"Idle break")
        {
            m_bAttackMode = true;
            m_bTauntMode = false;
        }
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
            spAnimModel->SetAnimation(L"Get Hit");
            spAnimModel->UpdateAttackData(false, spAnimModel->GetAttackCollider());
            m_iHitCount++;
        }

        if (m_iHitCount >= 3)
        {
            m_bisHitCooldown = true;
        }
        spHarlequinn->SetPrevHealth(spHarlequinn->GetHealth());
    }

    if (m_bisHitCooldown)
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
    if (m_bAttackMode && !Hit&& !spHarlequinn->GetTargetPlayer()->GetDeathState())
    {
        if (DistanceFromPlayer > AttackRange_Long)
        {
            if (CurAnimName != L"Crouch Idle")
                UpdateState(spAnimModel, ANIM_ATTACK, L"ATTACK1");
            else 
                UpdateState(spAnimModel, ANIM_ATTACK, L"ATTACK4");
        }
        else if (DistanceFromPlayer >= AttackRange_Jump && DistanceFromPlayer < AttackRange_Long)
        {
            m_bWillJumpToCloseRange = false;
            if (CurAnimName == L"Crouch Idle")
            {
                m_bWillJumpToCloseRange = true;
            }
            else
                UpdateState(spAnimModel, ANIM_MOVE, L"WALK");
        }
        else if (DistanceFromPlayer < AttackRange_Jump && DistanceFromPlayer > AttackRange_Close)
        {
             m_bWillJumpToCloseRange = true;

            if (CurAnimName == L"Crouch Idle")
            {
                UpdateState(spAnimModel, ANIM_ATTACK, L"ATTACK4");
                m_bWillJumpToCloseRange = false;
            }
        }
        else if (DistanceFromPlayer < AttackRange_Close)
        {
            m_bWillJumpToCloseRange = false;
            if (CurAnimName == L"Crouch Idle")
                UpdateState(spAnimModel, ANIM_ATTACK, L"ATTACK3");
            else
            {
                if (spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() >= 0.9)
                    m_iRandomValueforDodge = dis_dodge(gen); // Use the random generator

                if (m_iRandomValueforDodge == 0)
                    UpdateState(spAnimModel, ANIM_ATTACK, L"ATTACK2");
                if (m_iRandomValueforDodge == 1)
                    UpdateState(spAnimModel, ANIM_ATTACK, L"DODGE");
            }
        }
    }

    if (m_bWillJumpToCloseRange)
    {
        UpdateState(spAnimModel, ANIM_MOVE, L"JUMP");
        m_bWillJumpToCloseRange = false;
    }
    if (CurAnimName == L"Jump Forward")
    {
        if (spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() >= 0.0 && spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() < 0.1)
            spHarlequinn->GetTransform()->SetDirectionFixedUp(directionToPlayer, _dTimeDelta, 5);
        if (spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() >= 0.8)
        {
            spHarlequinn->GetTransform()->SetDirectionFixedUp(directionToPlayer, _dTimeDelta, 5);
            UpdateState(spAnimModel, ANIM_IDLE, L"CROUCH");
        }
    }
    if (CurAnimName == L"Attack 4")
    {
        if (spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() >= 0.8)
        {
            UpdateState(spAnimModel, ANIM_MOVE, L"WALK");
        }
    }

    if (CurAnimName == L"Attack 1")
    {
        m_f3ThrowingPos = spHarlequinn->GetTransform()->GetPos();
        m_f3ThrowingPos.y += 7.f;

        spHarlequinn->GetTransform()->SetDirectionFixedUp(directionToPlayer, _dTimeDelta, 5);

        if (spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() >= 0.2 && spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() < 0.22)
        {
            for (int i = 0; i < 3; i++)
            {
                if ((*spHarlequinn->GetShurikens())[i]->GetTraveledDistance() == 0)
                {
                    if (i == 0)
                    {
                        _float3 currentLookDir = -spHarlequinn->GetTransform()->GetLook();
                        _float angleRadians = 10 *(DirectX::XM_PI / 180.0f);;
                        _float cosAngle = std::cos(angleRadians);
                        _float sinAngle = std::sin(angleRadians);

                        _float newX = currentLookDir.x * cosAngle + currentLookDir.z * sinAngle;
                        _float newZ = -currentLookDir.x * sinAngle + currentLookDir.z * cosAngle;


                        m_arrThrowingDir[i] = _float3(newX, currentLookDir.y, newZ);
                    }
                    else if (i == 1)
                        m_arrThrowingDir[i] = -spHarlequinn->GetTransform()->GetLook();
                    else if (i == 2)
                    {
                        _float3 currentLookDir = -spHarlequinn->GetTransform()->GetLook();
                        _float angleRadians = -10 *(DirectX::XM_PI / 180.0f);
                        _float cosAngle = std::cos(angleRadians);
                        _float sinAngle = std::sin(angleRadians);

                        _float newX = currentLookDir.x * cosAngle + currentLookDir.z * sinAngle;
                        _float newZ = -currentLookDir.x * sinAngle + currentLookDir.z * cosAngle;


                        m_arrThrowingDir[i] = _float3(newX, currentLookDir.y, newZ);
                    }

                    (*spHarlequinn->GetShurikens())[i]->GetTransform()->SetPos(m_f3ThrowingPos);
                    (*spHarlequinn->GetShurikens())[i]->GetTransform()->SetDirection(-spHarlequinn->GetTransform()->GetRight());
                    (*spHarlequinn->GetShurikens())[i]->SetThrow(true);
                }
            }
        }
        else if (spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() >= 0.6 && spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() < 0.62)
        {
            for (int i = 3; i < 6; i++)
            {
                if ((*spHarlequinn->GetShurikens())[i]->GetTraveledDistance() == 0)
                {
                    if (i == 3)
                    {
                        _float3 currentLookDir = -spHarlequinn->GetTransform()->GetLook();
                        _float angleRadians = 10 *(DirectX::XM_PI / 180.0f);
                        _float cosAngle = std::cos(angleRadians);
                        _float sinAngle = std::sin(angleRadians);

                        _float newX = currentLookDir.x * cosAngle + currentLookDir.z * sinAngle;
                        _float newZ = -currentLookDir.x * sinAngle + currentLookDir.z * cosAngle;


                        m_arrThrowingDir[i] = _float3(newX, currentLookDir.y, newZ);
                    }
                    else if (i == 4)
                        m_arrThrowingDir[i] = -spHarlequinn->GetTransform()->GetLook();
                    else if (i == 5)
                    {
                        _float3 currentLookDir = -spHarlequinn->GetTransform()->GetLook();
                        _float angleRadians = -10 *(DirectX::XM_PI / 180.0f);;
                        _float cosAngle = std::cos(angleRadians);
                        _float sinAngle = std::sin(angleRadians);

                        _float newX = currentLookDir.x * cosAngle + currentLookDir.z * sinAngle;
                        _float newZ = -currentLookDir.x * sinAngle + currentLookDir.z * cosAngle;


                        m_arrThrowingDir[i] = _float3(newX, currentLookDir.y, newZ);
                    }
                    (*spHarlequinn->GetShurikens())[i]->GetTransform()->SetPos(m_f3ThrowingPos);
                    (*spHarlequinn->GetShurikens())[i]->GetTransform()->SetDirection(-spHarlequinn->GetTransform()->GetRight());
                    (*spHarlequinn->GetShurikens())[i]->SetThrow(true);
                }
            }
        }
    }

    if (CurAnimName == L"Attack 4")
    {
        m_f3ThrowingPos = spHarlequinn->GetTransform()->GetPos();
        m_f3ThrowingPos.y += 7.f;
        if (spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() >= 0.2 && spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() < 0.22)
        {
            for (int i = 0; i < 6; i++)
            {
                _float angle = 0;
                if ((*spHarlequinn->GetShurikens())[i]->GetTraveledDistance() == 0)
                {
                    if (i == 0)
                    {
                        m_arrThrowingDir[i] = -spHarlequinn->GetTransform()->GetLook();                
                    }         
                    else
                    {                      
                        angle = 60 * (i + 1);
                        _float3 currentLookDir = -spHarlequinn->GetTransform()->GetLook();
                        _float angleRadians = angle * (DirectX::XM_PI / 180.0f);
                        _float cosAngle = std::cos(angleRadians);
                        _float sinAngle = std::sin(angleRadians);

                        _float newX = currentLookDir.x * cosAngle + currentLookDir.z * sinAngle;
                        _float newZ = -currentLookDir.x * sinAngle + currentLookDir.z * cosAngle;


                        m_arrThrowingDir[i] = _float3(newX, currentLookDir.y, newZ);
                    }

                    (*spHarlequinn->GetShurikens())[i]->GetTransform()->SetPos(m_f3ThrowingPos);
                    (*spHarlequinn->GetShurikens())[i]->GetTransform()->SetDirection(-spHarlequinn->GetTransform()->GetRight());
                    (*spHarlequinn->GetShurikens())[i]->SetThrow(true);
                }
            }
        }
    }

    //½´¸®ÄË ´øÁö±â 
    for (int i = 0; i < 6; i++)
    {
        if ((*spHarlequinn->GetShurikens())[i]->GetTraveledDistance() > 100)
        {
            (*spHarlequinn->GetShurikens())[i]->SetThrow(false);
            (*spHarlequinn->GetShurikens())[i]->GetTransform()->SetPos(spHarlequinn->GetTransform()->GetPos());
            (*spHarlequinn->GetShurikens())[i]->SetTraveledDistance(0);
        }
        else
            spHarlequinn->ThrowShurikens(i, _dTimeDelta, m_arrThrowingDir[i]);
    }

    // Check for death
    if (spHarlequinn->GetHealth() <= 0)
    {
        spAnimModel->UpdateAttackData(false, spAnimModel->GetAttackCollider());
        spHarlequinn->SetDeathState(true);
    }

    // Handle death state
    if (spHarlequinn->GetDeathState())
    {
        UpdateState(spAnimModel, ANIM_DEATH, L"DEATH");
    }

    // Tick event
    spAnimModel->TickEvent(spHarlequinn.get(), GetTrigger(), _dTimeDelta);
}
