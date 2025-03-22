#include "Player.h"
#include "GameInstance.h"
#include "Sword.h"
#include "Sheath.h"
#include "Controller.h"
#include "Animator.h"
#include "Battle_Manager.h"
#include "Camera_Free.h"
#include "BonesCollider.h"
#include "AnimationClip.h"
#include "MainView.h"
#include "ImGuiClass_Manager.h"
#include "CutScene_Manager.h"

CPlayer::CPlayer(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CCharacter(_pDevice, _pContext)
{
}

CPlayer::CPlayer(const CPlayer& _rhs) 
    : CCharacter(_rhs)
{
    m_bObjectMotionBlur = false;
}

HRESULT CPlayer::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::NativeConstruct_Clone(void* pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(pArg)))
        return E_FAIL;
    
    m_fMoveSpeed = 80.f;
    m_fSaveMoveSpeed = m_fMoveSpeed;
    m_fRotateSpeed = XMConvertToRadians(90.f) * 3.f;

    if (CBattle_Manager::Get_Instance()->Get_BattleInfo().bSabitoAI)
    {
        m_tBattleData = BATTLEDATA(200.f, 5.f, 0.f);
        m_tBattleData.fChargeValue = m_tBattleData.fMaxChargeValue;
    }
    else
    {
        m_tBattleData = BATTLEDATA(100.f, 5.f, 0.f);
        m_tBattleData.fChargeValue = m_tBattleData.fMaxChargeValue;
    }
    return S_OK;
}

void CPlayer::Tick(const _double& _dTimeDelta)
{
    if (false == m_bActiveCutScene)
    {
        Update_TargetTransform();

        Fill_Datas();
        if (nullptr != m_pController)
            m_pController->Tick(_dTimeDelta);

        VOIDDATAS Datas;
        if (nullptr != m_pController)
        {
            Datas = m_pController->Out_Datas();
            // 들어있던 트리거를 비운다.
            m_strCurrentTrigger = "";

           
            // 배틀이 멈춰있다면 인풋을 막는다.
            if (true == m_tBattleData.bBattleStop)
            {
                if (CONTROLLER_TYPE::CONTROLLER_AI != m_eControllerType)
                    static_cast<vector<_string>*>(Datas[OUTDATA::OUT_STATE])->clear();
            }
        }

        if (true == m_tBattleData.bIsAwake)
            m_fMoveSpeed = 120.f;
        else
            m_fMoveSpeed = 80.f;

        // 받아온 인풋으로 애니메이션 트리거를 넣는다.
        Animation_Control_State(_dTimeDelta, Datas);

        if (false == m_bActive)
            return;

        if (nullptr != m_pAnimator)
        {
            // 애니메이션 업데이트
            m_pAnimator->Tick(_dTimeDelta);

            // 애니메이션 타입을 받아온다.
            if (nullptr != m_pAnimator)
                m_strAnimType = m_pAnimator->Get_AnimationType();

            if (nullptr != m_pSword)
            {
                m_pSword->Change_Bind_Matrix(m_pAnimator->Get_WeaponBone());
            }
        }
        // 상태에 따른 이동등의 행동을 한다.
        Behavior_State(_dTimeDelta, Datas);
        Check_SuccessDash();
        Control_Trail_Active();

        __super::Tick(_dTimeDelta);
    }
    else
    {
        Control_Trail_Active();
    }

    // 현재 개방상태라면 엣지 글로우를 킨다.
    if (true == m_tBattleData.bIsAwake)
    {
        m_vEdgeGlowColor.w = 1.f;
    }
    else
    {
        m_vEdgeGlowColor.w = 0.f;
    }

    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

    RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Late_Tick(const _double& _dTimeDelta)
{
    if (false == m_bRenderEnable)
    {
        if (nullptr != m_pSword)
            m_pSword->Set_RenderEnable(false);
        if (nullptr != m_pSheath)
            m_pSheath->Set_RenderEnable(false);
        return;
    }
    else
    {
        if (nullptr != m_pSword)
            m_pSword->Set_RenderEnable(true);
        if (nullptr != m_pSheath)
            m_pSheath->Set_RenderEnable(true);
    }

    if (false == m_bActiveCutScene)
    {
        if (nullptr != m_pController)
            m_pController->Late_Tick(_dTimeDelta);

        if (nullptr != m_pController)
            m_pController->Insert_Datas(m_InsertVoidDatas);

        __super::Late_Tick(_dTimeDelta);
    }
    else
    {
        __super::Late_Tick(_dTimeDelta);
    }
}

HRESULT CPlayer::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    if(nullptr != m_pSword)
        m_pSword->Set_Active(m_pAnimator->Is_MeshActive());
    if (nullptr != m_pSheath)
        m_pSheath->Set_Active(m_pAnimator->Is_MeshActive());

    return S_OK;
}

void CPlayer::Success_Dash()
{
    m_bDashCollision = true;
    m_fMoveSpeed = 0.f;
}

void CPlayer::Check_SuccessDash()
{
    CBattle_Manager* pInstance = GET_INSTANCE(CBattle_Manager)
    {
        CBattle_Manager::CHARACTER_TYPE eType = (CBattle_Manager::CHARACTER_TYPE)(Get_EnemyType());
        if (m_strEnemyState == "Dash")
        {
            CBattle_Manager::CHARACTER_TYPE eType = (CBattle_Manager::CHARACTER_TYPE)(Get_EnemyType());
            // 대쉬 성공 했다는 함수 호출
            static_cast<CPlayer*>(pInstance->Get_BattleCharacter(eType))->Success_Dash();
        }
    }
    RELEASE_INSTANCE(CBattle_Manager);
}


// 데미지 받을 때, 호출 되는 함수
void CPlayer::Damaged(CPlayer* _pPlayer)
{
    if (nullptr == _pPlayer)
        return;

    BATTLEDATA tData = _pPlayer->Get_BattleData();
    // 맞은 카운트를 증가 시킨다.
    ++m_tBattleData.iHitCount;
    // HP를 감소 시킨다. 
    m_tBattleData.Decrease_HP(tData.fAttackPower);
    // Damage Collision
    m_bDamagedToCollision = true;
}

void CPlayer::Hit_ByEnemy(CCharacter* _pEnemyCharcter, CBattle_Manager* _pBattleManager)
{
    __super::Hit_ByEnemy(_pEnemyCharcter, _pBattleManager);

    CImGuiClass_Manager* pInstance = GET_INSTANCE(CImGuiClass_Manager);
    {
        CBattle_Manager::CHARACTER_TYPE eType = (CBattle_Manager::CHARACTER_TYPE)(Get_EnemyType());

        if (eType == CBattle_Manager::CHAR_PLAYER)
            static_cast<CMainView*>(pInstance->Get_ImGuiClass(IMGCLASS_MAIN))->Add_Explain("AI Damaged");
        else if (eType == CBattle_Manager::CHAR_AI)
            static_cast<CMainView*>(pInstance->Get_ImGuiClass(IMGCLASS_MAIN))->Add_Explain("Player Damaged");
    }
    RELEASE_INSTANCE(CImGuiClass_Manager);
}

void CPlayer::Hit_Effect(_string strColliderType, CCharacter* _pEnemyCharcter)
{
    __super::Hit_Effect(strColliderType, _pEnemyCharcter);
}

// SetUp Controller
HRESULT CPlayer::SetUp_Controller(const CONTROLLER_TYPE& _eType)
{
    CBattle_Manager* pInstnace = GET_INSTANCE(CBattle_Manager);
    {
        m_eControllerType = _eType;
        if (CONTROLLER_AI == _eType)
        {
            pInstnace->Start_Battle(this, CBattle_Manager::CHAR_AI);
        }
        else if (CONTROLLER_KEYBOARD == _eType)
        {
            if(nullptr == pInstnace->Get_BattleCharacter(CBattle_Manager::CHAR_PLAYER))
                pInstnace->Start_Battle(this, CBattle_Manager::CHAR_PLAYER);
            else
            {
                pInstnace->Start_Battle(this, CBattle_Manager::CHAR_AI);
            }
        }
        else if (CONTROLLER_GAMEPAD == _eType)
        {
            if (nullptr == pInstnace->Get_BattleCharacter(CBattle_Manager::CHAR_PLAYER))
                pInstnace->Start_Battle(this, CBattle_Manager::CHAR_PLAYER);
            else
            {
                pInstnace->Start_Battle(this, CBattle_Manager::CHAR_AI);
            }
        }
    }
    RELEASE_INSTANCE(CBattle_Manager);
    // Player일 때 정의

    // AI 일떄 정의

    return S_OK;
}

HRESULT CPlayer::SetUp_ConstantTable()
{
    if (FAILED(__super::SetUp_ConstantTable()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::SetUp_Components()
{
    if (FAILED(__super::SetUp_Components()))
        return E_FAIL;

    // COM_NAVIGATION
    if (LEVEL_STORY == g_iCurrentLevel)
    {
        if (FAILED(Add_Component(COM_NAVIGATION, g_iCurrentLevel, PROTO_COMP_NAVIGATION_STORY, (CComponent**)&m_pNavigationCom)))
            return E_FAIL;
    }
    else if (LEVEL_BATTLE == g_iCurrentLevel)
    {
        if (FAILED(Set_BattleMap_Navigation()))
            return E_FAIL;
        //if (FAILED(Add_Component(COM_NAVIGATION, g_iCurrentLevel, PROTO_COMP_NAVIGATION_UROKODAKI, (CComponent**)&m_pNavigationCom)))
    }
    else if (LEVEL_GAMEPLAY == g_iCurrentLevel)
    {
        if (FAILED(Add_Component(COM_NAVIGATION, g_iCurrentLevel, PROTO_COMP_NAVIGATION_STORY_TEONI, (CComponent**)&m_pNavigationCom)))
            return E_FAIL;
    }
    else if (LEVEL_RATTAN == g_iCurrentLevel)
    {
        if (FAILED(Add_Component(COM_NAVIGATION, g_iCurrentLevel, PROTO_COMP_NAVIGATION_RATTAN, (CComponent**)&m_pNavigationCom)))
            return E_FAIL;
    }

    CCollider::COLLIDERDESC ColliderDesc;
    ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
    ColliderDesc.vScale = _float3{ 2.5f, 4.f, 1.5f };
    ColliderDesc.vTranslation = _float3{ 0.f, 5.f, 0.f };
    if (FAILED(__super::Add_Component(COM_COLLIDER_HIT, LEVEL_STATIC, PROTO_COMP_OBBCOLLIDER, (CComponent**)&m_pColliderCom, &ColliderDesc)))
        return E_FAIL;

    ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
    ColliderDesc.vScale = _float3{ 5.f, 10.f, 5.f };
    ColliderDesc.vTranslation = _float3{ 0.f, 10.f, 0.f };
    if (FAILED(__super::Add_Component(COM_COLLIDER_PREV_OBB, LEVEL_STATIC, PROTO_COMP_OBBCOLLIDER, (CComponent**)&m_pPrevCollider, &ColliderDesc)))
        return E_FAIL;

    ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
    ColliderDesc.vScale = _float3{ 15.f, 15.f, 15.f };
    ColliderDesc.vTranslation = _float3{ 0.f, 2.f, 0.f };
    if (FAILED(__super::Add_Component(COM_COLLIDER_ATTACK, LEVEL_STATIC, PROTO_COMP_SPHERECOLLIDER, (CComponent**)&m_pAttackColliderCom, &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::SetUp_Weapon(FILEDATA* _pFileData, const _wstring& _strModelProtoName, const _string& _strModelStartBone, const _string& _strModelEndBone)
{
    RETURN_IFNOT(nullptr == _pFileData, E_FAIL);

    CSword::SWORDDESC tDesc;
    {
        CSword::ITEMDESC tItemDesc;
        tItemDesc.pItemData = _pFileData;
        tItemDesc.pTargetModel = m_pModelCom;
        tItemDesc.pTargetTransform = m_pTransformCom;
        tItemDesc.pTargetAnimator = m_pAnimator;
        tItemDesc.strModelProtoName = _strModelProtoName;

        tDesc.tItemDesc = tItemDesc;
    }
    tDesc.strStartBoneName = _strModelStartBone;
    tDesc.strFinishBoneName = _strModelEndBone;

    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        m_pSword = static_cast<CSword*>(pGameInstance->Clone_GameObject_Add(g_iCurrentLevel, LAYER_SWORD, PROTO_GOB_SWORD, &tDesc));
        Safe_AddRef(m_pSword);
    }
    RELEASE_INSTANCE(CGameInstance);

    RETURN_IFNOT(nullptr == m_pSword, E_FAIL);

    return S_OK;
}

HRESULT CPlayer::SetUp_Sheath(FILEDATA* _pFileData, const _wstring& _strModelProtoName)
{
    RETURN_IFNOT(nullptr == _pFileData, E_FAIL);

    CSheath::ITEMDESC tItemDesc;
    tItemDesc.pItemData = _pFileData;
    tItemDesc.pTargetModel = m_pModelCom;
    tItemDesc.pTargetTransform = m_pTransformCom;
    tItemDesc.pTargetAnimator = m_pAnimator;
    tItemDesc.strModelProtoName = _strModelProtoName;

    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        m_pSheath = static_cast<CSheath*>(pGameInstance->Clone_GameObject_Add(g_iCurrentLevel, LAYER_SHEATH, PROTO_GOB_SHEATH, &tItemDesc));
        Safe_AddRef(m_pSheath);
    }
    RELEASE_INSTANCE(CGameInstance);

    RETURN_IFNOT(nullptr == m_pSheath, E_FAIL);

    return S_OK;
}

void CPlayer::Fill_Datas()
{
}

void CPlayer::Behavior_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

    // 대쉬에 성공했지만 상대방도 대쉬상태였다면 튕긴다.
    if (false == m_bIsSuccessDash && false == CMethod::Is_Vector_Zero(XMLoadFloat4(&m_vDashReflectDir)))
    {
        Dash_Reflect_State(_dTimeDelta);
        return;
    }

    // 움직이기
    if ("MOVE" == m_strAnimType || "Move" == m_strAnimType)
    {
        // MOVE일 경우 움직인다.
        Move_State(_dTimeDelta, _Datas);
    }
    // 지상 추적 대쉬
    else if ("DASH" == m_strAnimType || "Dash" == m_strAnimType)
    {
        Dash_Ground_State(_dTimeDelta, _Datas);
    }
    // 공중 추적 대쉬
    else if ("AIR_DASH" == m_strAnimType)
    {
        Dash_Air_State(_dTimeDelta, _Datas);
    }
    // 점프
    else if ("JUMP" == m_strAnimType || "Jump" == m_strAnimType)
    {
        Jump_State(_dTimeDelta, _Datas);
    }
    // 공중 스텝의 경우에는 스텝을 하는동안 아래로 떨어지게 된다.
    else if ("AIR_STEP" == m_strAnimType )
    {
        Step_State(_dTimeDelta, _Datas);
    }
    // 공중 공격은 애니메이션 이벤트에서 특정 구간동안 중력을 받지 않는다.
    else if ("AIR_ATTACK" == m_strAnimType || "Air_Attack" == m_strAnimType)
    {
        Attack_State(_dTimeDelta, _Datas);
    }
    else if ("ATTACK_AWAKE" == m_strAnimType && true == m_pAnimator->Is_EventPlay())
    {
        Attack_Awake_State(_dTimeDelta, _Datas);
    }
    else if ("ULTIMATE" == m_strAnimType && true == m_pAnimator->Is_EventPlay())
    {
        Ultimate_State(_dTimeDelta, _Datas);
    }
    else if ("CHARGE_ATTACK" == m_strAnimType || "Charge_Attack" == m_strAnimType)
    {
        Charge_Attack_State(_dTimeDelta, _Datas);
    }
    else if ("AIR_HIT" == m_strAnimType && true == m_pAnimator->Is_EventPlay())
    {
        Air_Hit_State(_dTimeDelta, _Datas);
    }
    else if ("AIR_DOWN" == m_strAnimType)
    {
        Air_Down_State(_dTimeDelta, _Datas);
    }
    else if ("HIT" == m_strAnimType && true == m_pAnimator->Is_EventPlay() || "Hit" == m_strAnimType)
    {
        Hit_State(_dTimeDelta, _Datas);
    }
    else if ("BOUND" == m_strAnimType && true == m_pAnimator->Is_EventPlay())
    {
        Bound_State(_dTimeDelta, _Datas);
    }
    else if ("STRIKE" == m_strAnimType)
    {
        Strike_State(_dTimeDelta, _Datas);
    }
    // 스핀 상태일때는 공격에 맞지 않고, 날아간다.
    else if ("SPIN" == m_strAnimType)
    {
        Spin_State(_dTimeDelta, _Datas);
    }
    // 다운 상태일때 죽었는지 판단을 해준다.
    else if ("DOWN" == m_strAnimType)
    {
        Down_State(_dTimeDelta, _Datas);
    }
    // Blow 됐을때는 Air_Down과 같은 움직임으로 움직인다.
    else if ("BLOW" == m_strAnimType && true == m_pAnimator->Is_EventPlay())
    {
        Blow_State(_dTimeDelta, _Datas);
    }
    // 지상 추적 대쉬
    else if ("ROLL" == m_strAnimType)
    {
        Roll_State(_dTimeDelta, _Datas);
    }
    // DEAD 상태일때
    else if ("DIE" == m_strAnimType)
    {
        Dead_State(_dTimeDelta);
    }
    else if ("PASSIVE" == m_strAnimType)
    {
        Passive_State(_dTimeDelta, _Datas);
    }

    RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Animation_Control_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    RETURN_IFNOT(_Datas.size() <= 0, ;);

    // 이번 프레임에 들어온 트리거 리스트
    vector<_string> triggerList = *static_cast<vector<_string>*>(_Datas[OUTDATA::OUT_STATE]);
    // 움직이는 방향값
    m_vDirection = *static_cast<_float3*>(_Datas[OUTDATA::OUT_DIRECTION]);

    // IDLE이 아니라면 차지 타이머를 리셋한다.
    if ("IDLE" != m_strAnimType)
        m_tChargeTimer.Reset_Timer();

    if (nullptr != m_pAnimator)
    {
        m_strAnimType = m_pAnimator->Get_AnimationType();

        for (auto& trigger : triggerList)
        {
            // 기본 상태일 때
            if ("IDLE" == m_strAnimType)
            {
                // 방향값이 0이 아니라면 MOVE 상태로 진행한다.
                if (!CMethod::Is_Vector_Zero(m_vDirection.Get_Vector()))
                {
                    if ("MOVE" == trigger)
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            // 이동방향쪽을 바라본다.
                            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + m_vDirection.Get_Vector());
                            break;
                        }
                    }
                }
                else if ("ATTACK" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        // 기력게이지가 차기위한 시간을 초기화한다.
                        m_tBattleData.tChargeTimer.Reset_Timer();

                        // 상대방을 바라본다.
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                else if ("JUMP" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;

                        if (!CMethod::Is_Vector_Zero(m_vBehaviorDir.Get_Vector()))
                            m_vBehaviorDir = _float4(0.f, 0.f, 0.f, 0.f);
                        break;
                    }
                }
                // 트리거가 가드일 때
                else if ("GUARD" == trigger)
                {
                    // 가드 트리거가 성공하면 Look으로 Target을 바라본다.
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        // 상대방을 바라본다.
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                // 대쉬 사용시
                else if ("DASH" == trigger)
                {
                    if (true == Input_AirDash_Trigger("AIR_DASH"))
                        break;
                    else
                    {
                        if (true == Input_Dash_Trigger(trigger))
                            break;
                    }
                }
                else if ("STEP" == trigger)
                {
                    // 스텝의 방향별 트리거를 받아온다.
                    _string strStepTrigger = Get_StepDir();

                    if (true == m_pAnimator->Input_Trigger(strStepTrigger))
                    {
                        // 스텝시 Look을 설정합니다.
                        m_strCurrentTrigger = trigger;
                        Set_StepLook(strStepTrigger);
                        break;
                    }
                }
                // SKILL_1 사용시
                else if ("SKILL_1" == trigger)
                {
                    if (true == Check_ChargeValue())
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            m_tBattleData.Decrease_Charge(20.f);

                            // 상대방을 바라본다.
                            m_strCurrentTrigger = trigger;
                            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                            break;
                        }
                    }
                }
                // 개방 사용시
                else if ("AWAKE" == trigger)
                {
                    if (m_tBattleData.iUltimateCount > 0)
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            m_tBattleData.Active_AwakeMode();
                            m_strCurrentTrigger = trigger;
                            // 상대방을 바라본다.
                            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                            break;
                        }
                    }
                }
                // 오의 사용시
                else if ("ULTIMATE" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        // 상대방을 바라본다.
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                else if ("CHARGE_ATTACK" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        // 기력게이지가 차기위한 시간을 초기화한다.
                        m_tBattleData.tChargeTimer.Reset_Timer();

                        // 상대방을 바라본다.
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        m_vBehaviorDir = XMVector3Normalize(Get_DirToTarget());
                        break;
                    }
                }
                // 스킬3 시전중일때 스킬3의 시전이 가능하다.
                else if ("SKILL_3" == trigger)
                {
                    if (true == Check_ChargeValue())
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            m_tBattleData.Decrease_Charge(20.f);

                            // 상대방을 바라본다.
                            m_strCurrentTrigger = trigger;
                            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                            break;
                        }
                    }
                }

                // 차지로 넘어간다.
                // 방향값이 0이 아니라면 MOVE 상태로 진행한다.
                if (CMethod::Is_Vector_Zero(m_vDirection.Get_Vector()))
                {
                    if (false == m_tBattleData.Is_CheckMaxCharge() && true == m_tBattleData.tChargeTimer.bPass)
                    {
                        if (m_tChargeTimer.Is_Over(_dTimeDelta))
                        {
                            if (true == m_pAnimator->Input_Trigger("CHARGE"))
                                m_tChargeTimer.Reset_Timer();
                        }
                    }
                }
            }
            // 현재 재생중인 애니메이션 타입이 MOVE일 경우
            else if ("MOVE" == m_strAnimType)
            {
                // 움직이고있다면 움직여야하는 방향을 쳐다본다.
                if (!CMethod::Is_Vector_Zero(m_vDirection.Get_Vector()))
                    m_pTransformCom->LookAt(Get_MoveDirection());

                // 달리기를 멈춘다.
                if ("MOVE_END" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        break;
                    }
                }
                else if ("JUMP" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        m_vBehaviorDir = m_pTransformCom->Get_Look();//m_pTransformCom->LookAt(m_vDirection.Get_Vector());
                        break;
                    }
                }
                else if ("ATTACK" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        // 기력게이지가 차기위한 시간을 초기화한다.
                        m_tBattleData.tChargeTimer.Reset_Timer();
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                else if ("STEP" == trigger)
                {
                    // 스텝의 방향별 트리거를 받아온다.
                    _string strStepTrigger = Get_StepDir();

                    if (true == m_pAnimator->Input_Trigger(strStepTrigger))
                    {
                        // 스텝시 Look을 설정합니다.
                        m_strCurrentTrigger = trigger;
                        Set_StepLook(strStepTrigger);
                        break;
                    }
                }
                else if ("GUARD" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                // 개방 사용시
                else if ("AWAKE" == trigger)
                {
                    if (m_tBattleData.iUltimateCount > 0)
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            m_tBattleData.Active_AwakeMode();
                            m_strCurrentTrigger = trigger;
                            // 상대방을 바라본다.
                            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                            break;
                        }
                    }
                }
                // 스킬2 시전
                else if ("SKILL_2" == trigger)
                {
                    if (true == Check_ChargeValue())
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            m_tBattleData.Decrease_Charge(20.f);
                            // 상대방을 바라본다.
                            m_strCurrentTrigger = trigger;
                            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                            break;
                        }
                    }
                }
                // 스킬3 시전
                else if ("SKILL_3" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        // 상대방을 바라본다.
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                // 오의 사용시
                else if ("ULTIMATE" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        // 상대방을 바라본다.
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
            }
            // 현재 재생중인 애니메이션의 타입이 공격일 경우
            else if ("ATTACK" == m_strAnimType)
            {
                // 상단 공격시
                if (trigger == "ATTACK_UPPER")
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        // 기력게이지가 차기위한 시간을 초기화한다.
                        m_tBattleData.tChargeTimer.Reset_Timer();
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                // 하단 공격시
                else if (trigger == "ATTACK_DOWN")
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        // 기력게이지가 차기위한 시간을 초기화한다.
                        m_tBattleData.tChargeTimer.Reset_Timer();
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                // 일반 공격시
                else if (trigger == "ATTACK")
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        // 기력게이지가 차기위한 시간을 초기화한다.
                        m_tBattleData.tChargeTimer.Reset_Timer();
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                // 개방 공격 시
                else if (trigger == "ATTACK_AWAKE")
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        // 기력게이지가 차기위한 시간을 초기화한다.
                        m_tBattleData.tChargeTimer.Reset_Timer();
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                // 점프 시
                else if (trigger == "JUMP")
                {
                    // 기력게이지를 20 소모할 수 있다면 점프를 시전한다.
                    if (true == Check_ChargeValue())
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            m_tBattleData.Decrease_Charge(20.f);

                            m_strCurrentTrigger = trigger;

                            if (!CMethod::Is_Vector_Zero(m_vBehaviorDir.Get_Vector()))
                                m_vBehaviorDir = _float4(0.f, 0.f, 0.f, 0.f);
                            break;
                        }
                    }
                }
                // 대쉬인 경우
                else if (trigger == "DASH")
                {
                    if (true == Input_AirDash_Trigger("AIR_DASH"))                    
                        break;
                    
                    if (true == Check_ChargeValue())
                    {
                        if (true == Input_Dash_Trigger(trigger))
                        {
                            m_tBattleData.Decrease_Charge(20.f);
                            break;
                        }
                    }
                }
                else if (trigger == "STEP")
                {
                    // 기력게이지를 20 소모할 수 있다면 스텝을 시전한다.
                    // 스텝의 방향별 트리거를 받아온다.
                    _string strStepTrigger = Get_StepDir();

                    if (true == Check_ChargeValue())
                    {
                        if (true == m_pAnimator->Input_Trigger(strStepTrigger))
                        {
                            m_tBattleData.Decrease_Charge(20.f);

                            m_strCurrentTrigger = trigger;
                            // 스텝시 Look을 설정합니다.
                            Set_StepLook(strStepTrigger);
                            break;
                        }
                    }
                }
                // 개방 사용시
                else if ("AWAKE" == trigger)
                {
                    if (m_tBattleData.iUltimateCount > 0)
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            m_tBattleData.Active_AwakeMode();
                            m_strCurrentTrigger = trigger;
                            // 상대방을 바라본다.
                            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                            break;
                        }
                    }
                }
                // 스킬2 시전
                else if ("SKILL_2" == trigger)
                {
                    if (true == Check_ChargeValue())
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            m_tBattleData.Decrease_Charge(20.f);
                            m_strCurrentTrigger = trigger;
                            // 상대방을 바라본다.
                            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                            break;
                        }
                    }
                }
                // 스킬3 시전
                else if ("SKILL_3" == trigger)
                {
                    if (true == Check_ChargeValue())
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            m_tBattleData.Decrease_Charge(20.f);
                            m_strCurrentTrigger = trigger;
                            // 상대방을 바라본다.
                            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                            break;
                        }
                    }
                }
                // 오의 사용시
                else if ("ULTIMATE" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        // 상대방을 바라본다.
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
            }
            // 공중 공격시 할 수 있는 행동들
            else if ("AIR_ATTACK" == m_strAnimType)
            {
                // 일반 공격시
                if (trigger == "ATTACK")
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                else if (trigger == "DASH")
                {
                    if (true == Check_ChargeValue())
                    {
                        if (true == Input_AirDash_Trigger("AIR_DASH"))
                        {
                            m_tBattleData.Decrease_Charge(20.f);
                            m_strCurrentTrigger = trigger;
                            break;
                        }
                    }
                }
                else if (trigger == "STEP")
                {
                    // 기력게이지를 20 소모할 수 있다면 스텝을 시전한다.
                    // 스텝의 방향별 트리거를 받아온다.
                    _string strStepTrigger = Get_StepDir();

                    if (true == Check_ChargeValue())
                    {
                        if (true == m_pAnimator->Input_Trigger(strStepTrigger))
                        {
                            m_tBattleData.Decrease_Charge(20.f);
                            m_strCurrentTrigger = trigger;
                            // 스텝시 Look을 설정합니다.
                            Set_StepLook(strStepTrigger);
                            break;
                        }
                    }
                }
            }
            // 현재 재생중인 애니메이션 타입이 스텝인 경우
            else if ("STEP" == m_strAnimType)
            {
                if (trigger == "STEP")
                {
                    // 스텝의 방향별 트리거를 받아온다.
                    _string strStepTrigger = Get_StepDir();

                    if (true == m_pAnimator->Input_Trigger(strStepTrigger))
                    {
                        m_strCurrentTrigger = trigger;
                        // 스텝시 Look을 설정합니다.
                        Set_StepLook(strStepTrigger);
                        break;
                    }
                }
            }
            // 현재 재생중인 애니메이션 타입이 차지 공격인 경우
            else if ("CHARGE_ATTACK" == m_strAnimType)
            {
                if (trigger == "CHARGE_ATTACK_END")
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        break;
                    }
                }
            }
            // 현재 재생중인 애니메이션의 타입이 점프인 경우
            else if ("JUMP" == m_strAnimType)
            {
                if (trigger == "ATTACK")
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        // 기력 회복시간을 초기화한다.
                        m_tBattleData.tChargeTimer.Reset_Timer();
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                else if (trigger == "STEP")
                {
                    // 스텝의 방향별 트리거를 받아온다.
                    _string strStepTrigger = Get_StepDir();

                    if (true == m_pAnimator->Input_Trigger(strStepTrigger))
                    {
                        m_strCurrentTrigger = trigger;
                        // 스텝시 Look을 설정합니다.
                        Set_StepLook(strStepTrigger);
                        m_dFallTimeAcc = 0.0;
                        break;
                    }
                }
                else if (trigger == "DASH")
                {
                    // 상대방이 공중 추적대쉬가 가능할 경우 공중 추적대쉬를 사용한다.
                    // 점프 중일때는 일반 대쉬는 사용 불가능하다.
                    if (true == Input_AirDash_Trigger("AIR_DASH"))
                    {
                        m_strCurrentTrigger = trigger;
                        break;
                    }
                }
            }
            // 현재 재생중인 애니메이션이 가드인 경우
            else if ("GUARD" == m_strAnimType)
            {
                if (trigger == "GUARD_END")
                {
                    m_strCurrentTrigger = trigger;
                    m_pAnimator->Input_Trigger(trigger);
                    break;
                }
                // 밀쳐내기
                else if (trigger == "PUSH")
                {
                    // 밀치기를 수행하면 상대방을 바라본다.
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                // 반격
                else if (trigger == "PARING")
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                // 스킬3 사용
                else if (trigger == "SKILL_3")
                {
                    if (true == Check_ChargeValue())
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            m_tBattleData.Decrease_Charge(20.f);
                            m_strCurrentTrigger = trigger;
                            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                            break;
                        }
                    }
                }
                // 잡기
                else if (trigger == "THROW")
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                // 대쉬
                else if (trigger == "DASH")
                {
                    if (true == Input_Dash_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        break;
                    }
                }
            }
            // 현재 재생중인 애니메이션 타입이 DASH인 경우
            else if ("DASH" == m_strAnimType)
            {
                // 공격 사용시
                if ("ATTACK" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        // 상대방을 바라본다.
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                // 스텝 사용시
                else if ("STEP" == trigger)
                {
                    // 스텝의 방향별 트리거를 받아온다.
                    _string strStepTrigger = Get_StepDir();

                    if (true == m_pAnimator->Input_Trigger(strStepTrigger))
                    {
                        m_strCurrentTrigger = trigger;
                        // 스텝시 Look을 설정합니다.
                        Set_StepLook(strStepTrigger);
                        break;
                    }
                }
                // 방어 사용시
                else if ("GUARD" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        // 상대방을 바라본다.
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                // 점프 사용시
                else if ("JUMP" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        // 대쉬도중 점프를 사용하면 대쉬를 하던 방향으로 점프를 한다.
                        m_vBehaviorDir = m_pTransformCom->Get_Look();
                        break;
                    }
                }
                // 개방 사용시
                else if ("AWAKE" == trigger)
                {
                    if (m_tBattleData.iUltimateCount > 0)
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            m_tBattleData.Active_AwakeMode();
                            m_strCurrentTrigger = trigger;
                            // 상대방을 바라본다.
                            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                            break;
                        }
                    }
                }
                // 오의 사용시
                else if ("ULTIMATE" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        // 상대방을 바라본다.
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
            }
            // 다운 된 경우
            else if ("DOWN" == m_strAnimType)
            {
                // 다운일 때 움직이면 ROLL로 전환한다.
                if ("MOVE" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger("ROLL"))
                    {
                        m_strCurrentTrigger = trigger;
                        break;
                    }
                }
                // 낙법
                else if ("PASSIVE" == trigger)
                {
                    // 낙법 시에는 뒤쪽으로 점프해야한다 ( 포물선 이동 해야한다 )
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        // 상대방을 바라본다.
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
            }
            else if ("CHARGE" == m_strAnimType)
            {
                // 공격 사용시
                if ("ATTACK" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        // 상대방을 바라본다.
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                else if ("MOVE" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        // 이동방향쪽을 바라본다.
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + m_vDirection.Get_Vector());
                        break;
                    }
                }
                else if ("JUMP" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;

                        if (!CMethod::Is_Vector_Zero(m_vBehaviorDir.Get_Vector()))
                            m_vBehaviorDir = _float4(0.f, 0.f, 0.f, 0.f);
                        break;
                    }
                }
                else if ("GUARD" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        // 상대방을 바라본다.
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                else if ("AWAKE" == trigger)
                {
                    if (m_tBattleData.iUltimateCount > 0)
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            m_tBattleData.Active_AwakeMode();
                            m_strCurrentTrigger = trigger;
                            // 상대방을 바라본다.
                            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                            break;
                        }
                    }
                }
                else if ("SKILL_1" == trigger)
                {
                    if (true == Check_ChargeValue())
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            m_tBattleData.Decrease_Charge(20.f);
                            m_strCurrentTrigger = trigger;
                            // 상대방을 바라본다.
                            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                            break;
                        }
                    }
                }
                else if ("SKILL_3" == trigger)
                {
                    if (true == Check_ChargeValue())
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            m_tBattleData.Decrease_Charge(20.f);
                            m_strCurrentTrigger = trigger;
                            // 상대방을 바라본다.
                            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                            break;
                        }
                    }
                }
                else if ("ULTIMATE" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        // 상대방을 바라본다.
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
            }
        }
    }
}

void CPlayer::Skill_1(const _double& _dTimeDetla, VOIDDATAS _Datas)
{
}
void CPlayer::Skill_2(const _double& _dTimeDetla, VOIDDATAS _Datas)
{
}
void CPlayer::Skill_3(const _double& _dTimeDetla, VOIDDATAS _Datas)
{
}
void CPlayer::Move_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    _float3* vDirection = (_float3*)_Datas[OUTDATA::OUT_DIRECTION];
    if (!CMethod::Is_Vector_Zero(vDirection->Get_Vector()))
    {
        // 이동한다.
        m_pTransformCom->Translate_DirSliding(vDirection->Get_Vector(), _dTimeDelta, m_fMoveSpeed);
    }
    m_bMotionBlur = false;
}

// 공중에 떠서 공격하는 경우 아래로 내려준다.
void CPlayer::Attack_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    // 프레임당 이동값을 계산한다.
    _vector vMovePos = Compute_MovePos(_dTimeDelta, m_vBehaviorDir.Get_Vector(), 0.f);

    // 점프 위치가 네비 Y값보다 작으면 네비 Y로 고정
    _vector vPlayerPos = m_pTransformCom->Get_Pos() + vMovePos;
    _float fCurCellYPos = m_pNavigationCom->Compute_Height(vPlayerPos);
    if (fCurCellYPos >= XMVectorGetY(vPlayerPos))
    {
        m_dFallTimeAcc = 0.0;
        vPlayerPos = XMVectorSetY(vPlayerPos, fCurCellYPos);
        m_pAnimator->Input_Trigger("FALL_END");
        m_bIsAir = false;
    }
    else
        m_bIsAir = true;

    // Y는 내가 설정한 Y값, XZ는 네비에 따라 제한
    m_pTransformCom->Move_Pos_NotNavigateY(vPlayerPos);
    m_bMotionBlur = false;
}

// 상승 공격
void CPlayer::Attack_Upper_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    m_bMotionBlur = false;
}

// 하단 공격
void CPlayer::Attack_Down_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    m_bMotionBlur = false;
}

void CPlayer::Attack_Awake_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    m_bMotionBlur = false;
}

void CPlayer::Ultimate_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    m_bMotionBlur = false;
}

// 강공격
void CPlayer::Charge_Attack_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    if (true == m_tChargeAttackTimer.Is_Over(_dTimeDelta))
    {
        Input_Trigger("CHARGE_ATTACK_END");
        m_tChargeAttackTimer.Reset_Timer();
    }

    m_bMotionBlur = false;
}

// 공중에 떠서 스텝할 경우 아래로 내려준다.
void CPlayer::Step_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    _vector vMovePos = Compute_MovePos(_dTimeDelta, m_vBehaviorDir.Get_Vector(), 0.f);

    // 점프 위치가 네비 Y값보다 작으면 네비 Y로 고정
    _vector vPlayerPos = m_pTransformCom->Get_Pos() + vMovePos;
    _float fCurCellYPos = m_pNavigationCom->Compute_Height(vPlayerPos);
    if (fCurCellYPos >= XMVectorGetY(vPlayerPos))
    {
        m_dFallTimeAcc = 0.0;
        vPlayerPos = XMVectorSetY(vPlayerPos, fCurCellYPos);
        m_pAnimator->Input_Trigger("FALL_END");
        m_bIsAir = false;
    }
    else
        m_bIsAir = true;

    // Y는 내가 설정한 Y값, XZ는 네비에 따라 제한
    m_pTransformCom->Move_Pos_NotNavigateY(vPlayerPos);

    m_bMotionBlur = true;
}
// 지상 추격 대쉬 사용시
void CPlayer::Dash_Ground_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    if (nullptr == m_pTargetTransform)
        return;

    // 추적하는 대상을 쳐다본다.
    m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
    // 대쉬는 기본 이동속도보다 1.5배 빠르게 움직인다.
    m_pTransformCom->Translate_Dir(Get_DirToTarget(), _dTimeDelta, m_fMoveSpeed * 2.f);

    m_bMotionBlur = true;
}
// 공중 추격 대쉬 사용시
void CPlayer::Dash_Air_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    if (nullptr == m_pTargetTransform)
        return;

    // 추적하는 대상을 쳐다본다.
    m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
    _vector vDir = m_pTargetTransform->Get_Pos() - m_pTransformCom->Get_Pos();
    // 대쉬는 기본 이동속도보다 1.5배 빠르게 움직인다.
    m_pTransformCom->Translate_DirNotNavigation(XMVector3Normalize(vDir), _dTimeDelta, m_fMoveSpeed * 2.f);

    m_bMotionBlur = true;
}
void CPlayer::Jump_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    // 착지시 FALL_END 트리거를 인풋한다.
    Jump(_dTimeDelta, "FALL_END");
    m_bMotionBlur = false;
}
void CPlayer::Passive_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    // 자신으로부터 상대방을 바라보는 방향벡터의 반대방향으로 점프한다.
    _vector vMovePos = Compute_MovePos_ByDist(_dTimeDelta, m_vBehaviorDir.Get_Vector(),60.f, 25.f, 0.3f, 1.f);

    // 점프 위치가 네비 Y값보다 작으면 네비 Y로 고정
    _vector vPlayerPos = m_pTransformCom->Get_Pos() + vMovePos;
    _float fCurCellYPos = m_pNavigationCom->Compute_Height(vPlayerPos);
    if (fCurCellYPos >= XMVectorGetY(vPlayerPos))
    {
        m_dFallTimeAcc = 0.0;
        vPlayerPos = XMVectorSetY(vPlayerPos, fCurCellYPos);
        m_pAnimator->Input_Trigger("FALL_END");
        m_bIsAir = false;
    }
    else
        m_bIsAir = true;

    // Y는 내가 설정한 Y값, XZ는 네비에 따라 제한
    m_pTransformCom->Move_Pos_NotNavigateY(vPlayerPos);
    m_bMotionBlur = false;
}
void CPlayer::Roll_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    // MoveDirection값이 0이 아니라면 Roll 상태동안 굴러서 이동한다.
    if (!CMethod::Is_Vector_Zero(m_vDirection.Get_Vector()))
    {
        m_pTransformCom->Translate_Dir(m_vDirection.Get_Vector(), _dTimeDelta, m_fMoveSpeed);
        m_pTransformCom->LookAt(Get_MoveDirection());
        m_bMotionBlur = false;
    }
}

// 지상에서 Hit된 경우 ( 높이가 없음 )
void CPlayer::Hit_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    __super::Hit_State(_dTimeDelta, _Datas);

    m_bMotionBlur = false;
}
void CPlayer::Air_Hit_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    __super::Air_Hit_State(_dTimeDelta, _Datas);

    m_bMotionBlur = false;
}

void CPlayer::Air_Down_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    // 콜라이더의 방향값을 받아옵니다.
    _vector vColliderDir = m_pAnimator->Get_ColliderDir();
    // 피격 효과에서 넘어온 PushedForce; 
    _float fDistance = m_tKnockBackInfo.fPushedForce;
    // 피격 효과에서 넘어온 MaxHeight
    _float fMaxHeight = m_tKnockBackInfo.fHeight;
    // 맞는 애니메이션 수행 시간
    _double dEventPlayTime = m_tKnockBackInfo.dPlayTime;
    // 밀려날 방향벡터
    _vector vAttackDir = m_tKnockBackInfo.vAttackDir.Get_Vector();

    // 애니메이션이 최대높이에 도달하는 비율
    _float fMaxHeightRatio = 0.7f;

    // 계산해서 나온 프레임당 이동거리
    _vector vMovePos = Compute_MovePos_ByDist(_dTimeDelta, vAttackDir, fDistance, fMaxHeight, fMaxHeightRatio, (_float)dEventPlayTime);
    // 플레이어의 위치
    _vector vPlayerPos = m_pTransformCom->Get_Pos() + vMovePos;
    _float fCurCellYPos = m_pNavigationCom->Compute_Height(vPlayerPos);
    if (fCurCellYPos >= XMVectorGetY(vPlayerPos))
    {
        m_dFallTimeAcc = 0.0;
        vPlayerPos = XMVectorSetY(vPlayerPos, fCurCellYPos);

        // 트리거 인풋이 됐는지 구분한다.
        _bool bIsInput = false;

        // 착지시에 점프키가 눌려있었다면 PASSIVE를 발동시킨다.

        if (CONTROLLER_AI != m_eControllerType)
        {
            vector<_string>* pData = static_cast<vector<_string>*>(_Datas[OUTDATA::OUT_STATE]);
            if (nullptr != pData)
            {
                vector<_string> triggerList = *pData;
                for (auto& trigger : triggerList)
                {
                    if ("PASSIVE" == trigger)
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            bIsInput = true;

                            CBattle_Manager* pInstance = GET_INSTANCE(CBattle_Manager);

                            // 상대방에서 자신을 향하는 벡터를 BehaviorDir로 만들어준다.
                            if (nullptr != m_pTargetTransform)
                            {
                                m_vBehaviorDir = XMVector3Normalize(m_pTransformCom->Get_Pos() - m_pTargetTransform->Get_Pos());
                                m_vBehaviorDir.y = 0.f;
                            }
                            RELEASE_INSTANCE(CBattle_Manager);

                            break;
                        }
                    }
                }
            }
        }
        if (false == bIsInput)
            m_pAnimator->Input_Trigger("FALL_END");

        m_bIsAir = false;
    }
    else
        m_bIsAir = true;

    // Y는 내가 설정한 Y값, XZ는 네비에 따라 제한
    m_pTransformCom->Move_Pos_NotNavigateY(vPlayerPos);

    m_bMotionBlur = false;
}
void CPlayer::Strike_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    __super::Strike_State(_dTimeDelta, _Datas);

    m_bMotionBlur = false;
}

void CPlayer::Bound_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    __super::Bound_State(_dTimeDelta, _Datas);

    m_bMotionBlur = false;
}

void CPlayer::Blow_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    __super::Blow_State(_dTimeDelta, _Datas);

    m_bMotionBlur = false;
}

void CPlayer::Check_Attack_Type(_string _strHitType)
{
    __super::Check_Attack_Type(_strHitType);
}

void CPlayer::Success_Attack_Upper()
{
    __super::Success_Attack_Upper();

    m_bMotionBlur = false;
}

void CPlayer::Success_Attack_Down()
{
    __super::Success_Attack_Down();

    m_bMotionBlur = true;
}

void CPlayer::Success_SKILL_1()
{
    __super::Success_SKILL_1();

    m_bMotionBlur = true;
}

void CPlayer::Success_SKILL_2()
{
    __super::Success_SKILL_2();

    m_bMotionBlur = true;
}

void CPlayer::Success_SKILL_3()
{
    __super::Success_SKILL_3();

    m_bMotionBlur = true;
}

void CPlayer::Success_Throw()
{
}

void CPlayer::Success_Attack_Awake()
{
}

void CPlayer::Success_Ultimate()
{
}

void CPlayer::Control_Trail_Active()
{
    if (nullptr == m_pSword)
        return;

    m_pSword->Set_TrailActive(m_pAnimator->Is_TrailActive());
    if (true == m_pAnimator->Is_TrailActive())
    {
        _float2 vTrailValues = m_pAnimator->Get_TrailValues();
        m_pSword->Update_TrailMatrix(100, 60);
    }
}

void CPlayer::Extern_ImGuiClass()
{
    __super::Extern_ImGuiClass();

    // 현재 공중에 있는지 체크한다.
    ImGui::Checkbox("Air", &m_bIsAir);

    // 애니메이션 타입을 디버깅창에 띄운다.
    if (nullptr != m_pAnimator)
        ImGui::Text(m_strAnimType.c_str());

    _string strFallTime = "FallTime";
    strFallTime += CMethod::Convert_Number_S(m_dFallTimeAcc, 3);

    // 떨어지는 시간 디버그
    ImGui::Text(strFallTime.c_str());    
}

void CPlayer::Coll_Init()
{
    __super::Coll_Init();

    if (nullptr != m_pPrevCollider)
        m_pPrevCollider->Init_Collision();
}

void CPlayer::Coll_Tick(_matrix _WorldMatrix)
{
    __super::Coll_Tick(_WorldMatrix);

    if (nullptr != m_pPrevCollider)
    {   
        _WorldMatrix = CMethod::XMMatrixSetFloat3(m_vPreviousPosition, MATROW_POS, _WorldMatrix);
        m_pPrevCollider->Tick(_WorldMatrix);
    }
}

void CPlayer::Check_Hit_Type(_string _strHitType, CCharacter* _pEnemyCharacter)
{
    __super::Check_Hit_Type(_strHitType, _pEnemyCharacter);
}

void CPlayer::Play_EndCutScene(const _bool& _bIsAI)
{
    CCutScene_Manager* pCutSceneManager = GET_INSTANCE(CCutScene_Manager);
    {
        pCutSceneManager->Set_CutSceneModul(m_CutSceneModulMap["BATTLE_END"]);
    }
    RELEASE_INSTANCE(CCutScene_Manager);
}

HRESULT CPlayer::Set_BattleMap_Navigation()
{
    CBattle_Manager* pInstance = GET_INSTANCE(CBattle_Manager);

    if (nullptr == pInstance)
    {
        RELEASE_INSTANCE(CBattle_Manager);
        return E_FAIL;
    }

    CBattle_Manager::BATTLEMAP_TYPE battleMap = pInstance->Get_BattleInfo().eBattleMap;

    switch (battleMap)
    {
    case Client::CBattle_Manager::MAP_UROKODAKI:
        if (FAILED(Add_Component(COM_NAVIGATION, g_iCurrentLevel, PROTO_COMP_NAVIGATION_UROKODAKI, (CComponent**)&m_pNavigationCom)))
            return E_FAIL;
        break;
    case Client::CBattle_Manager::MAP_HUJIKASANE:
        if (FAILED(Add_Component(COM_NAVIGATION, g_iCurrentLevel, PROTO_COMP_NAVIGATION_UROKODAKI, (CComponent**)&m_pNavigationCom)))
            return E_FAIL;
        break;
    case Client::CBattle_Manager::MAP_ASAKUSA:
        if (FAILED(Add_Component(COM_NAVIGATION, g_iCurrentLevel, PROTO_COMP_NAVIGATION_UROKODAKI, (CComponent**)&m_pNavigationCom)))
            return E_FAIL;
        break;
    case Client::CBattle_Manager::MAP_TAMAYO:
        if (FAILED(Add_Component(COM_NAVIGATION, g_iCurrentLevel, PROTO_COMP_NAVIGATION_TAMAYO, (CComponent**)&m_pNavigationCom)))
            return E_FAIL;
        break;
    }
   
    RELEASE_INSTANCE(CBattle_Manager);
    return S_OK;
}

#ifdef _DEBUG
void CPlayer::Coll_Render()
{
    __super::Coll_Render();
}
void CPlayer::Coll_Renderer()
{
    __super::Coll_Renderer();
}
#endif

_bool CPlayer::Input_AirDash_Trigger(const _string& _strTrigger)
{
    _bool bIsAirDash = false;

    CBattle_Manager* pInstance = GET_INSTANCE(CBattle_Manager);

    // 적 객체를 받아온다.
    CCharacter* pEnemy = pInstance->Get_BattleCharacter((CBattle_Manager::CHARACTER_TYPE)Get_EnemyType());
    if (nullptr == pEnemy)
    {
        RELEASE_INSTANCE(CBattle_Manager);
        return false;
    }
    RELEASE_INSTANCE(CBattle_Manager);

    // 애니메이션 타입을 검사한다.
    if ("AIR_DOWN" == pEnemy->Get_Animator()->Get_AnimationType())
    {
        // 기력게이지를 20 소모할 수 있다면 대쉬를 시전한다.
        if (true == m_pAnimator->Input_Trigger(_strTrigger))
        {
            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
            return true;
        }
    }
    return false;
}

_bool CPlayer::Input_Dash_Trigger(const _string& _strTrigger)
{
    CBattle_Manager* pInstance = GET_INSTANCE(CBattle_Manager);

    // 적 객체를 받아온다.
    CCharacter* pEnemy = pInstance->Get_BattleCharacter((CBattle_Manager::CHARACTER_TYPE)Get_EnemyType());
    if (nullptr == pEnemy)
    {
        RELEASE_INSTANCE(CBattle_Manager);
        return false;
    }
    RELEASE_INSTANCE(CBattle_Manager);

    // 애니메이션 타입을 검사한다. 다운인 경우에는 대쉬하지 못한다.
    if ("DOWN" != pEnemy->Get_Animator()->Get_AnimationType())
    {
        // 기력게이지를 20 소모할 수 있다면 대쉬를 시전한다.
        if (true == m_pAnimator->Input_Trigger(_strTrigger))
        {
            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
            return true;
        }
    }
    return false;
}

void CPlayer::Update_TargetTransform()
{
    CBattle_Manager* pBattleInstance = GET_INSTANCE(CBattle_Manager);
    {
        if (LEVEL_BATTLE == g_iCurrentLevel || LEVEL_GAMEPLAY == g_iCurrentLevel)
        {
            if (nullptr == m_pTargetTransform)
            {
                CBattle_Manager::CHARACTER_TYPE eType = (CBattle_Manager::CHARACTER_TYPE)(Get_EnemyType());
                if (nullptr != pBattleInstance->Get_BattleCharacter(eType))
                {
                    m_pTargetTransform = pBattleInstance->Get_BattleCharacter(eType)->Get_Transform();
                }
            }
        }
        else if (LEVEL_STORY == g_iCurrentLevel)
        {
            vector<CCharacter*>& pOniArr = pBattleInstance->Get_OniArr();

            _float fMinLength = FLT_MAX;
            _float fMinLengthOniDegree = 360.f;
            // 타겟 오니 구해오기
            CCharacter* pTargetOni = nullptr;
            for (auto& pOni : pOniArr)
            {
                if (pOni->Get_BattleData().bBattleStop)
                    continue;

                {
                    _vector vPlayerLook = m_pTransformCom->Get_Look();
                    _vector vPlayerToOni = pOni->Get_Transform()->Get_Pos() - m_pTransformCom->Get_Pos();

                    _float fCurDegree = fabs(CMethod::Compute_Degree_XZ_Plane(vPlayerLook, vPlayerToOni));
                    _float fCurLength = XMVectorGetX(XMVector3Length(vPlayerToOni));

                    // MinLength보다 작을 경우 타겟 설정
                    if (fMinLength > fCurLength)
                    {
                        pTargetOni = pOni;
                        fMinLength = fCurLength;
                        fMinLengthOniDegree = fCurDegree;
                    }
                    // MinLength보다 크지만 5이하로 차이날 경우 각도로 한번 더 체크
                    else if (fMinLength + 5.f > fCurLength)
                    {
                        if (fMinLengthOniDegree > fCurDegree)
                        {
                            pTargetOni = pOni;
                            fMinLength = fCurLength;
                            fMinLengthOniDegree = fCurDegree;
                        }
                    }
                }
            }
            
            // 타겟 오니 세팅
            if (nullptr != pTargetOni)
            {
                if (CGameInstance::Get_Instance()->Get_DIKeyDown(DIK_RSHIFT)
                    || nullptr == m_pTargetTransform)
                {
                    m_pTargetTransform = pTargetOni->Get_Transform();
                }
            }
        }
    }
    RELEASE_INSTANCE(CBattle_Manager);
}

_uint CPlayer::Get_EnemyType()
{
    CBattle_Manager::CHARACTER_TYPE eType = CBattle_Manager::CHAR_END;
    if (CONTROLLER_AI == m_eControllerType)
    {
        eType = CBattle_Manager::CHAR_PLAYER;
    }
    else if (CONTROLLER_GAMEPAD == m_eControllerType || CBattle_Manager::CHAR_AI == m_eControllerType)
    {
        CBattle_Manager* pBattleManager = GET_INSTANCE(CBattle_Manager);
        {
            if(this == pBattleManager->Get_BattleCharacter(CBattle_Manager::CHAR_AI))
                eType = CBattle_Manager::CHAR_PLAYER;
            else
                eType = CBattle_Manager::CHAR_AI;
        }
        RELEASE_INSTANCE(CBattle_Manager);
    }

    return eType;
}

_float CPlayer::Get_EnemyToPlayerDot()
{
    _float fDot = 0.f;
    CBattle_Manager* pInstance = GET_INSTANCE(CBattle_Manager)
    {
        CBattle_Manager::CHARACTER_TYPE eType = (CBattle_Manager::CHARACTER_TYPE)(Get_EnemyType());
        fDot = XMConvertToDegrees(OtherCharacterDirToLook(pInstance->Get_BattleCharacter(eType)));
    }
    RELEASE_INSTANCE(CBattle_Manager);
    return fDot;
}

_vector CPlayer::Get_MoveDirection()
{
    return m_pTransformCom->Get_Pos() + m_vDirection.Get_Vector();
}

_vector CPlayer::Get_DirToTarget()
{
    // 상대방의 Transform을 갖고있지 않다면 자기 자신의 Look을 반환한다.
    if (nullptr == m_pTargetTransform)
        return m_pTransformCom->Get_Look();

    // Y값을 제외시킨다.
    _vector vDir = m_pTargetTransform->Get_Pos() - m_pTransformCom->Get_Pos();
    vDir = XMVectorSetY(vDir, 0.f);

    m_vBehaviorDir = XMVector3Normalize(vDir);

    return XMVector3Normalize(vDir);
}

_string CPlayer::Get_StepDir()
{
    // 타겟이 없을 경우에는 간단하게 대쉬한다.
    if (nullptr == m_pTargetTransform)
    {
        // 이동 벡터가 0이 아닌 경우에 사용한다.
        if (!CMethod::Is_Vector_Zero(m_vDirection.Get_Vector()))
        {
            // Step 트리거의 조건을 검사하여 올바른 트리거를 넣어주며, Look또한 조건에 따라 바꿔준다.

            // m_vDirection 과 Target과의 Look 을 비교하여 트리거를 전달한다.
            _vector vLook = XMVector3Normalize(m_pTransformCom->Get_Look());

            // 정확한 각도 계산을 위해 Y를 없앤다.
            vLook = XMVectorSetY(vLook, 0);
            m_vDirection.y = 0;

            // Target까지의 Look과 진행방향 Direct의 각도가 몇도인지를 판단하기 위해 내적한다.
            _vector vDot = XMVector3Dot(vLook, XMVector3Normalize(m_vDirection.Get_Vector()));

            // Degree값으로 구한다.
            _float fDegree = XMConvertToDegrees(acosf(XMVectorGetX(vDot)));

            // 타겟을 바라보는 기준으로 진행방향이 오른쪽인지 왼쪽인지를 판단하기위해 외적한다.
            _vector vCross = XMVector3Cross(vLook, m_vDirection.Get_Vector());

            // fDegree가 50도 보다 작거나 같다면 STEP_FRONT를 시전한다.
            if (80 >= fDegree)
                return "STEP_FRONT";
            // 50도 보다크고 140보다 작거나 같다면 좌우 판별을 통해 STEP_LEFT 혹은 STEP_RIGHT를 시전한다.
            else if (80 < fDegree && 100 >= fDegree)
            {
                // 타겟을 향한 Look 기준으로 진행방향이 우측일때
                if (0 < XMVectorGetY(vCross))
                    return "STEP_RIGHT";
                // 타겟을 향한 Look 기준으로 진행방향이 좌측일때
                else
                    return "STEP_LEFT";
            }
            // 140보다 fDegree가 크다면 STEP_BACK을 사용한다.
            else if (100 < fDegree)
                return "STEP_BACK";
        }
    }
    else
    {
        // Step 트리거의 조건을 검사하여 올바른 트리거를 넣어주며, Look또한 조건에 따라 바꿔준다.

        // m_vDirection 과 Target과의 Look 을 비교하여 트리거를 전달한다.
        _vector vLookToTarget = XMVector3Normalize(m_pTargetTransform->Get_Pos() - m_pTransformCom->Get_Pos());

        // 정확한 각도 계산을 위해 Y를 없앤다.
        vLookToTarget = XMVectorSetY(vLookToTarget, 0);
        m_vDirection.y = 0;

        // Target까지의 Look과 진행방향 Direct의 각도가 몇도인지를 판단하기 위해 내적한다.
        _vector vDot = XMVector3Dot(vLookToTarget, XMVector3Normalize(m_vDirection.Get_Vector()));

        // Degree값으로 구한다.
        _float fDegree = XMConvertToDegrees(acosf(XMVectorGetX(vDot)));

        // 타겟을 바라보는 기준으로 진행방향이 오른쪽인지 왼쪽인지를 판단하기위해 외적한다.
        _vector vCross = XMVector3Cross(vLookToTarget, m_vDirection.Get_Vector());

        // fDegree가 50도 보다 작거나 같다면 STEP_FRONT를 시전한다.
        if (50 >= fDegree)
            return "STEP_FRONT";
        // 50도 보다크고 140보다 작거나 같다면 좌우 판별을 통해 STEP_LEFT 혹은 STEP_RIGHT를 시전한다.
        else if (50 < fDegree && 140 >= fDegree)
        {
            // 타겟을 향한 Look 기준으로 진행방향이 우측일때
            if (0 < XMVectorGetY(vCross))
                return "STEP_RIGHT";
            // 타겟을 향한 Look 기준으로 진행방향이 좌측일때
            else 
                return "STEP_LEFT";            
        }
        // 140보다 fDegree가 크다면 STEP_BACK을 사용한다.
        else if (140 < fDegree)        
            return "STEP_BACK";        
    }

    return _string();
}

void CPlayer::Set_StepLook(const _string& _strTrigger)
{
    if (!CMethod::Is_Vector_Zero(m_vDirection.Get_Vector()))
    {
        if ("STEP_RIGHT" == _strTrigger)
        {
            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + XMVector3Normalize(m_vDirection.Get_Vector()));
            m_pTransformCom->Rotate_TurnToNotFix(_float3(0.f, -90.f, 0.f));
            m_vBehaviorDir = m_pTransformCom->Get_Look();
            m_pAnimator->Set_OwnerDirection(m_vDirection.Get_Vector());


        }
        else if ("STEP_LEFT" == _strTrigger)
        {
            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + XMVector3Normalize(m_vDirection.Get_Vector()));
            m_pTransformCom->Rotate_TurnToNotFix(_float3(0.f, 90.f, 0.f));
            m_vBehaviorDir = m_pTransformCom->Get_Look();
            m_pAnimator->Set_OwnerDirection(m_vDirection.Get_Vector());
        }
        else if ("STEP_FRONT" == _strTrigger)
        {
            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + XMVector3Normalize(m_vDirection.Get_Vector()));
            m_vBehaviorDir = m_pTransformCom->Get_Look();
            m_pAnimator->Set_OwnerDirection(m_vDirection.Get_Vector());
        }
        else if ("STEP_BACK" == _strTrigger)
        {
            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + XMVector3Normalize(m_vDirection.Get_Vector() * -1.f));
            m_vBehaviorDir = m_pTransformCom->Get_Look();
            m_pAnimator->Set_OwnerDirection(m_vDirection.Get_Vector());
        }
    }
}

_bool CPlayer::Check_ChargeValue()
{
    // 기력게이지를 20만큼 깎아도 0보다 크다면 true 를 return한다.
    if (0 < m_tBattleData.fChargeValue - 20.f)
        return true;
    else
        return false;
}

CGameObject* CPlayer::Clone(void* pArg)
{
    return nullptr;
}

void CPlayer::Free()
{
    Safe_Release(m_pSheath);
    Safe_Release(m_pSword);
    Safe_Release(m_pPrevCollider);

    __super::Free();
}
