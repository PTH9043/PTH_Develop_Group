#include "ClientDefines.h"
#include "CUserWarriorAnimController.h"
#include "CWarriorPlayer.h"
#include "UGameInstance.h"
#include "UAnimModel.h"
#include "UCharacter.h"
#include "UAnimation.h"
#include "UTransform.h"
#include "UProcessedData.h"
#include "UMethod.h"
#include "UNavigation.h"
#include "CMainCamera.h"

CUserWarriorAnimController::CUserWarriorAnimController(CSHPTRREF<UDevice> _spDevice) 
	: UAnimationController(_spDevice), m_iWComboStack{ 0 }, m_iSComboStack{ 0 }
{
}

CUserWarriorAnimController::CUserWarriorAnimController(const CUserWarriorAnimController& _rhs) : 
	UAnimationController(_rhs), m_iWComboStack{ 0 }, m_iSComboStack{ 0 }
{
}

void CUserWarriorAnimController::Free()
{
}

HRESULT CUserWarriorAnimController::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT CUserWarriorAnimController::NativeConstructClone(const VOIDDATAS& _tDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_tDatas), E_FAIL);

    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

    SHPTR<UCharacter> spCharacter = GetOwnerCharacter();
	m_wpWarriorPlayer = std::dynamic_pointer_cast<CWarriorPlayer>(spCharacter);
   
	return S_OK;
}

void CUserWarriorAnimController::Tick(const _double& _dTimeDelta)
{
    ClearTrigger();
    SetAnimState(-1);

    SHPTR<CWarriorPlayer> spWarriorPlayer = m_wpWarriorPlayer.lock();
    SHPTR<UTransform>    spSelfTransform = spWarriorPlayer->GetTransform();
    SHPTR<UAnimModel> spAnimModel = spWarriorPlayer->GetAnimModel();
    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
    const _wstring& CurAnimName = spAnimModel->GetCurrentAnimation()->GetAnimName();

    spSelfTransform->DisableMotionblurOn();

    ifStartedGame = spWarriorPlayer->GetIfStartedGame();

    if (ifStartedGame) {
        // Input checks
        _bool isRunshift{ false }, isMoveFront{ false }, isMoveBack{ false }, isMoveLeft{ false },
            isMoveRight{ false }, isWAttack{ false }, isSAttack{ false }, isRAttack{ false },
            isCombo1{ false }, isCombo2{ false }, isAttack{ false }, isRoll{ false }, Hit{ false },
            isKicked{ false },isJump{ false }, isRise{ false };

        static constexpr _double KickedAnimSpeed = 20, KickedTimeArcOpenEnd = 35;

        if (false == spGameInstance->IsFreeModeCameraEnable() && false == spGameInstance->IsGamePause())
        {
            isRunshift = spGameInstance->GetDIKeyPressing(DIK_LSHIFT);
            isMoveFront = spGameInstance->GetDIKeyPressing(DIK_W);
            isMoveBack = spGameInstance->GetDIKeyPressing(DIK_S);
            isMoveLeft = spGameInstance->GetDIKeyPressing(DIK_A);
            isMoveRight = spGameInstance->GetDIKeyPressing(DIK_D);
            isWAttack = spGameInstance->GetDIMBtnDown(DIMOUSEBUTTON::DIMB_L);
            isSAttack = spGameInstance->GetDIMBtnDown(DIMOUSEBUTTON::DIMB_R);
            isRAttack = spGameInstance->GetDIKeyDown(DIK_Q);
            isCombo1 = spGameInstance->GetDIKeyDown(DIK_1);
            isCombo2 = spGameInstance->GetDIKeyDown(DIK_2);
            isAttack = isWAttack || isRAttack || isSAttack || isCombo1 || isCombo2;
            isRoll = spGameInstance->GetDIKeyDown(DIK_C);
            Hit = false;
            if (spWarriorPlayer->GetPrevHealth() > spWarriorPlayer->GetHealth())
                Hit = true;
            isKicked = spWarriorPlayer->GetKickedState();
            isJump = spGameInstance->GetDIKeyDown(DIK_SPACE);
            isRise = spWarriorPlayer->GetRiseState();
        }

        // Idle state check
        if (!isRoll && !isKicked && !isJump && !isRise && !isMoveFront && !isMoveBack && !isMoveLeft && !isMoveRight && !isAttack && !isCombo1 && !isCombo2) {
            UpdateState(spAnimModel, ANIM_IDLE, L"IDLE");
        }

        // Movement handling
        auto updateMovement = [&](const wchar_t* walkAnim, const wchar_t* runAnim, _bool isMove) {
            if (isMove) {
                if (!isRunshift) {
                    UpdateState(spAnimModel, ANIM_MOVE, walkAnim);
                }
                else {
                    UpdateState(spAnimModel, ANIM_MOVE, runAnim);
                }
            }
            };
        updateMovement(L"WALKF", L"RUNF", isMoveFront);
        updateMovement(L"WALKB", L"RUNB", isMoveBack);
        updateMovement(L"WALKL", L"RUNL", isMoveLeft);
        updateMovement(L"WALKR", L"RUNR", isMoveRight);

        if (isMoveFront && isMoveLeft) updateMovement(L"WALKFL", L"RUNFL", true);
        if (isMoveFront && isMoveRight) updateMovement(L"WALKFR", L"RUNFR", true);
        if (isMoveBack && isMoveLeft) updateMovement(L"WALKBL", L"RUNBL", true);
        if (isMoveBack && isMoveRight) updateMovement(L"WALKBR", L"RUNBR", true);

        // Jump or fall state
        if (spWarriorPlayer->GetJumpingState() || spWarriorPlayer->GetFallingState()) {
            UpdateState(spAnimModel, ANIM_JUMP, L"JUMP");
            m_iWComboStack = 0;
            m_iSComboStack = 0;
            spWarriorPlayer->GetTransform()->MoveForward(_dTimeDelta, isRunshift ? 40.f : 20.f);
            spSelfTransform->EnableMotionblurOn();
        }

        // Jumping logic
        if (!isAttack && !isMoveBack && isJump) {
            if (CurAnimName != L"roll_left" && CurAnimName != L"roll_right" && CurAnimName != L"roll_front" && CurAnimName != L"roll_back" && CurAnimName != L"dead03" && CurAnimName != L"hit_head_back" && CurAnimName != L"dead01" && CurAnimName != L"rise01" && CurAnimName != L"rise02"
                && CurAnimName != L"combo02_1" && CurAnimName != L"combo02_2" && CurAnimName != L"combo02_3"
                && CurAnimName != L"combo06_1" && CurAnimName != L"combo06_2" && CurAnimName != L"combo06_3"
                && CurAnimName != L"combo05" && CurAnimName != L"combo09" && CurAnimName != L"attack04")
            {
                if (CurAnimName == L"jumpZ0" && spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() < 0.9)
                {
                }
                else
                {
                    if (!spWarriorPlayer->GetJumpingState() && !spWarriorPlayer->GetFallingState())
                    {
                        spWarriorPlayer->SetJumpingState(true);
                    }
                }
            }
        }

        // Roll handling
        if (isRoll) {
            if (isMoveFront) UpdateState(spAnimModel, ANIM_ROLL, L"ROLL_F");
            else if (isMoveBack) UpdateState(spAnimModel, ANIM_ROLL, L"ROLL_B");
            else if (isMoveLeft) UpdateState(spAnimModel, ANIM_ROLL, L"ROLL_L");
            else if (isMoveRight) UpdateState(spAnimModel, ANIM_ROLL, L"ROLL_R");
            else UpdateState(spAnimModel, ANIM_ROLL, L"ROLL_F");

            spSelfTransform->EnableMotionblurOn();
        }

        // Hit state
        if (Hit && !isKicked) {
            HitStack++;
            UpdateState(spAnimModel, ANIM_HIT, L"HIT_BACK");
            spAnimModel->UpdateAttackData(false, spAnimModel->GetAttackCollider());
            spWarriorPlayer->SetPrevHealth(spWarriorPlayer->GetHealth());

            spSelfTransform->EnableMotionblurOn();
        }

        // Rise state
        if (isRise) {
            UpdateState(spAnimModel, ANIM_MOVE, L"RISE1");
        }
        if (CurAnimName == L"rise01")
            spWarriorPlayer->SetRiseState(false);

        // Kicked state
        if (isKicked && CurAnimName != L"rise01") {
            UpdateState(spAnimModel, ANIM_HIT, L"GOTKICKED");         
            spAnimModel->UpdateAttackData(false, spAnimModel->GetAttackCollider());
        }

        // Attack handling
        if (isAttack && !Hit && !spWarriorPlayer->GetDeathState()) {
            if (isWAttack) {
    
                if (CurAnimName == L"combo02_1") m_iWComboStack = 2;
                else if (CurAnimName == L"combo02_2") m_iWComboStack = 3;
                else m_iWComboStack = 1;

                switch (m_iWComboStack) {
                case 1: UpdateState(spAnimModel, ANIM_ATTACK, L"WATTACK1"); break;
                case 2: UpdateState(spAnimModel, ANIM_ATTACK, L"WATTACK2"); break;
                case 3: UpdateState(spAnimModel, ANIM_ATTACK, L"WATTACK3"); break;
                }
            }
            else if (isSAttack) {

                if (CurAnimName == L"combo06_1") m_iSComboStack = 2;
                else if (CurAnimName == L"combo06_2") m_iSComboStack = 3;
                else m_iSComboStack = 1;

                switch (m_iSComboStack) {
                case 1: UpdateState(spAnimModel, ANIM_ATTACK, L"SATTACK1"); break;
                case 2: UpdateState(spAnimModel, ANIM_ATTACK, L"SATTACK2"); break;
                case 3: UpdateState(spAnimModel, ANIM_ATTACK, L"SATTACK3"); break;
                }
            }
            else if (isRAttack && ShortAttackisAvailable && UltimateAttackOneDurationTime < 0.0001f && UltimateAttackTwoDurationTime < 0.0001f) {
                UpdateState(spAnimModel, ANIM_ATTACK, L"RATTACK");
                m_iWComboStack = 0;
                m_iSComboStack = 0;
            }
            else if (isCombo1 && UltAttackOneisAvailable && UltimateAttackTwoDurationTime < 0.0001f && ShortAttackDurationTime < 0.0001f) {
                UpdateState(spAnimModel, ANIM_ATTACK, L"COMBO5");
                m_iWComboStack = 0;
                m_iSComboStack = 0;
            }
            else if (isCombo2 && UltAttackTwoisAvailable && ShortAttackDurationTime < 0.0001f && UltimateAttackOneDurationTime < 0.0001f) {
                UpdateState(spAnimModel, ANIM_ATTACK, L"COMBO9");
                m_iWComboStack = 0;
                m_iSComboStack = 0;
            }

            spSelfTransform->EnableMotionblurOn();
        }

        if (CurAnimName == L"combo05")
        {
            UltimateAttackOneCoolTime = ONE_SKILL;

        }
        if (CurAnimName == L"combo09")
        {
            UltimateAttackTwoCoolTime = TWO_SKILL;

        }
        if (CurAnimName == L"attack04")
        {
            ShortAttackCoolTime = Q_SKILL;

        }

        if (UltimateAttackOneCoolTime > 0.f) {
            UltAttackOneisAvailable = false;

            UltimateAttackOneDurationTime -= _dTimeDelta;
            if (UltimateAttackOneDurationTime <= 0)
            {
                UltimateAttackOneCoolTime -= _dTimeDelta;
            }
        }
        else {
            UltAttackOneisAvailable = true;
        }

        if (UltimateAttackTwoCoolTime > 0.f) {
            UltAttackTwoisAvailable = false;
            UltimateAttackTwoDurationTime -= _dTimeDelta;
            if (UltimateAttackTwoDurationTime <= 0)
            {
                UltimateAttackTwoCoolTime -= _dTimeDelta;
            }
        }
        else {
            UltAttackTwoisAvailable = true;
        }
        if (ShortAttackCoolTime > 0.f) {
            ShortAttackisAvailable = false;
            ShortAttackDurationTime -= _dTimeDelta;
            if (ShortAttackDurationTime <= 0)
            {
                ShortAttackCoolTime -= _dTimeDelta;
            }

        }
        else {
            ShortAttackisAvailable = true;
        }
        if (HitStack >= 5) {
            HitStack = 0;
            BlindEffectOn = true;
        }
        else {
            BlindEffectOn = false;
        }

        // Combo attack state
        if (CurAnimName == L"attack04" || CurAnimName == L"combo09" || CurAnimName == L"combo05" || CurAnimName == L"combo06_1" || CurAnimName == L"combo06_2" || CurAnimName == L"combo06_3" || CurAnimName == L"combo02_1" || CurAnimName == L"combo02_2" || CurAnimName == L"combo02_3") {
            spWarriorPlayer->IfAttack(true);
        }
        else {
            spWarriorPlayer->IfAttack(false);
        }

        if (false == spGameInstance->IsFreeModeCameraEnable() &&  false == spGameInstance->IsGamePause())
        {
            // Mouse Move
            _long		MouseMove = spGameInstance->GetDIMMoveState(DIMM_X);
            if (MouseMove)
            {
                if (CurAnimName != L"dead01" && CurAnimName != L"dead03")
                {
                    spWarriorPlayer->GetTransform()->RotateTurn(_float3(0.f, 1.f, 0.f), MouseMove * 5.f, _dTimeDelta);
                }
            }
        }
        // Check for death
        if (spWarriorPlayer->GetHealth() <= 0)
        {
            spAnimModel->UpdateAttackData(false, spAnimModel->GetAttackCollider());
            spWarriorPlayer->SetDeathState(true);

            m_bDieEffectTurnedOn = true;
            m_bReviveSoundPlay = false;
        }
        // Handle death state
        if (spWarriorPlayer->GetDeathState())
        {
            UpdateState(spAnimModel, ANIM_DEATH, L"DEAD");
            spWarriorPlayer->SetKickedState(false);
        }

        // Tick eve
        spAnimModel->TickEvent(spWarriorPlayer.get(), GetTrigger(), _dTimeDelta);
        //if Kicked & Stunned
        if (CurAnimName == L"dead03" || (CurAnimName == L"dead01" && spWarriorPlayer->GetDeathState()))
        {
            _double DeathAnimSpeed = 20;
            if (spWarriorPlayer->GetElapsedTime() == 0.0)
            {
                spGameInstance->SoundPlayOnce(L"Die");
            }
            spWarriorPlayer->SetElapsedTime(spWarriorPlayer->GetElapsedTime() + (_dTimeDelta * DeathAnimSpeed));
            _double DeathTimeArcOpenEnd = 50;
            if (spWarriorPlayer->GetElapsedTime() < DeathTimeArcOpenEnd)
            {
                spAnimModel->TickAnimToTimeAccChangeTransform(spWarriorPlayer->GetTransform(), _dTimeDelta, spWarriorPlayer->GetElapsedTime());
            }
            if (spWarriorPlayer->GetElapsedTime() >= 70)
            {
                if (spGameInstance->GetDIKeyDown(DIK_G))
                {
                    m_bDieEffectTurnedOn = false;
                    if (!m_bReviveSoundPlay) {
                        spGameInstance->SoundPlayOnce(L"Revive");
                        m_bReviveSoundPlay = true;
                    }
                    m_bDieSoundPlay = false;

                    spWarriorPlayer->GetCurrentNavi()->FindCell(spWarriorPlayer->GetSpawnPointCell()->GetIndex());
                    spWarriorPlayer->GetTransform()->SetPos(spWarriorPlayer->GetSpawnPointCell()->GetCenterPos());
                    static_pointer_cast<CMainCamera>(spWarriorPlayer->GetFollowCamera())->GetCurrentNavi()->FindCell(spWarriorPlayer->GetSpawnPointCamera()->GetIndex());
                    spWarriorPlayer->SetElapsedTime(0);
                    spWarriorPlayer->SetDeathState(false);
                    spWarriorPlayer->SetHealth(2500);
                    spGameInstance->TurnOffDieEffect();

                    UpdateState(spAnimModel, ANIM_IDLE, L"IDLE");
                    spAnimModel->SetAnimation(L"idle01");
                }
            }
        }
        else
        {
            if (isKicked)
            {
                if (spWarriorPlayer->GetWarriorKickedTimeElapsed() == 0)
                {
                    spGameInstance->SoundPlayOnce(L"Kicked");
                    spAnimModel->SetAnimation(L"dead01");
                }
                spWarriorPlayer->SetWarriorKickedTimeElapsed(spWarriorPlayer->GetWarriorKickedTimeElapsed() + _dTimeDelta * KickedAnimSpeed);
                if (spWarriorPlayer->GetWarriorKickedTimeElapsed() < KickedTimeArcOpenEnd)
                    spAnimModel->TickAnimToTimeAccChangeTransform(spWarriorPlayer->GetTransform(), _dTimeDelta, spWarriorPlayer->GetWarriorKickedTimeElapsed());
                if (spWarriorPlayer->GetWarriorKickedTimeElapsed() >= 40)
                    if (spGameInstance->GetDIKeyDown(DIK_SPACE))
                    {
                        spWarriorPlayer->SetWarriorKickedTimeElapsed(0);
                        spWarriorPlayer->SetKickedState(false);
                        spWarriorPlayer->SetRiseState(true);
                    }
            }
            else
                spAnimModel->TickAnimChangeTransform(spWarriorPlayer->GetTransform(), _dTimeDelta);
        }
    }
}


void CUserWarriorAnimController::ReceiveNetworkProcessData(void* _pData)
{

}


