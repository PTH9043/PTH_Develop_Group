#include "Battle_Manager.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Character.h"
#include "BonesCollider.h"
#include "AnimationClip.h"
#include "MainView.h"
#include "ImGuiClass_Manager.h"

#include "Oni.h"
#include "Level_Story.h"
#include "Level_Battle.h"
#include "CutScene_Manager.h"
#include "Player.h"
#include "CutSceneModul.h"

IMPLEMENT_SINGLETON(CBattle_Manager);

CBattle_Manager::CBattle_Manager()
{
}

HRESULT CBattle_Manager::NativeConstruct_StartCutScene()
{
    if (nullptr == m_pCharacterArr[CHAR_PLAYER] || nullptr == m_pCharacterArr[CHAR_AI])
        return E_FAIL;

    m_pCharacterArr[CHAR_PLAYER]->Start_Scene(m_pCharacterArr[CHAR_AI]);
    m_pCharacterArr[CHAR_AI]->Start_Scene(m_pCharacterArr[CHAR_PLAYER]);

    CCutScene_Manager* pInstance = GET_INSTANCE(CCutScene_Manager);
    {
        pInstance->Update_Battle(m_pCharacterArr[CHAR_PLAYER], m_pCharacterArr[CHAR_AI]);
    }
    RELEASE_INSTANCE(CCutScene_Manager);
    return S_OK;
}

void CBattle_Manager::Start_Battle(CCharacter* _pCharacter, const CHARACTER_TYPE& _eCharType)
{
    Safe_Release(m_pCharacterArr[_eCharType]);
    m_pCharacterArr[_eCharType] = _pCharacter;
    Safe_AddRef(m_pCharacterArr[_eCharType]);
}

CCharacter* CBattle_Manager::Get_BattleCharacter(const CHARACTER_TYPE& _eCharType){
    return m_pCharacterArr[_eCharType];
}

_bool CBattle_Manager::Is_MotionBlurOn()
{
    if (nullptr == m_pCharacterArr[CHAR_AI] || nullptr == m_pCharacterArr[CHAR_PLAYER])
        return false;

    return m_pCharacterArr[CHAR_AI]->Is_MotionBlur() | m_pCharacterArr[CHAR_PLAYER]->Is_MotionBlur();
}

void CBattle_Manager::Update(const _double& _dTimeDelta)
{
    CCutScene_Manager* pInstance = GET_INSTANCE(CCutScene_Manager);
    {
        if(nullptr == pInstance->Get_SelectCutSceneModel())
        {
            // 업데이트 시간 조절
            _double TimeDelta = _dTimeDelta * (_double)m_tBattleInfo.fUpdateRatio;

            InitBattle(TimeDelta);

            {
                // 대전 맵
                if (LEVEL_BATTLE == g_iCurrentLevel || LEVEL_GAMEPLAY == g_iCurrentLevel)
                {
                    if (nullptr == m_pCharacterArr[CHAR_PLAYER] || nullptr == m_pCharacterArr[CHAR_AI])
                    {
                        RELEASE_INSTANCE(CCutScene_Manager);
                        return;
                    }

                    // 충돌 처리
                    Push();

                    Check_BattleDelayTimer(TimeDelta);

                    // 배틀이 종료될때는 피격되지 않는다.
                    if (!m_pCharacterArr[CHAR_AI]->Get_BattleData().bBattleStop && !m_pCharacterArr[CHAR_PLAYER]->Get_BattleData().bBattleStop)
                    {

                        // 플레이어가 공격할 때 처리
                        m_pCharacterArr[CHAR_PLAYER]->Check_AttackCollision(m_pCharacterArr[CHAR_AI]);
                        // AI가 공격할때 처리
                        m_pCharacterArr[CHAR_AI]->Check_AttackCollision(m_pCharacterArr[CHAR_PLAYER]);
                    }

                    if (CGameInstance::Get_Instance()->Get_DIKeyDown(DIK_0))
                    {
                        m_pCharacterArr[CHAR_AI]->Get_BattleData().Decrease_HP(20.f);
                    }

                }
            }

            {
                // 스토리 맵
                if (LEVEL_STORY == g_iCurrentLevel)
                {
                    SetUp_OniLayer();
                    // 오니와 플레이어 피격 처리
                    for (auto& pOni : m_pOniArr)
                    {
                        if ("DIE" == pOni->Get_Animator()->Get_AnimationType()
                            || "DOWN" == pOni->Get_Animator()->Get_AnimationType())
                            continue;

                        m_pCharacterArr[CHAR_PLAYER]->Push(pOni);
                        pOni->Push(m_pCharacterArr[CHAR_PLAYER]);

                        Check_StoryDelayTimer(TimeDelta);

                        // 오니 공격 처리
                        pOni->Check_AttackCollision(m_pCharacterArr[CHAR_PLAYER]);
                        // 플레이어 공격 처리
                        m_pCharacterArr[CHAR_PLAYER]->Check_AttackCollision(pOni);
                    }

                    
                }
            }

            if (m_tBattleInfo.bBattleEnd)
            {
                if (LEVEL_BATTLE == g_iCurrentLevel)
                {
                    CCharacter::BATTLEDATA& tPlayerData = m_pCharacterArr[CHAR_PLAYER]->Get_BattleData();
                    CCharacter::BATTLEDATA& tAIData = m_pCharacterArr[CHAR_AI]->Get_BattleData();

                    if (tPlayerData.iWinCount > tAIData.iWinCount)
                    {
                        CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pCharacterArr[CBattle_Manager::CHAR_PLAYER]);
                        if (nullptr != pPlayer)
                            pPlayer->Play_EndCutScene(m_tBattleInfo.bSabitoAI);
                    }
                    else
                    {
                        CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pCharacterArr[CBattle_Manager::CHAR_AI]);
                        if (nullptr != pPlayer)
                            pPlayer->Play_EndCutScene();
                    }
                }
            }
        } 
        else
        {
            if (m_tBattleInfo.bBattleEnd)
            {
                if (true == pInstance->Get_SelectCutSceneModel()->Is_End() && false == m_bFadeOut)
                {
                    CRenderer* pRenderer = (CRenderer*)m_pCharacterArr[CHAR_PLAYER]->Get_Component(COM_RENDERER);
                    if (nullptr != pRenderer)
                    {
                        pRenderer->Start_FadeInOut(0.05f, false, 1);
                    }
                    m_bFadeOut = true;
                }
            }
        }
    }
    RELEASE_INSTANCE(CCutScene_Manager);
}

void CBattle_Manager::SetUp_OniLayer()
{
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        list<CGameObject*> zakolist;
        //오니(첫번째 그륩)
        if (1 == m_iCurrentOniLayer)
            zakolist = pGameInstance->Find_GameObjectList(LEVEL_STORY, LAYER_ONI1);
        //오니(두번째 그륩)
        else if (2 == m_iCurrentOniLayer)
            zakolist = pGameInstance->Find_GameObjectList(LEVEL_STORY, LAYER_ONI2);
        //오니(세번째 그륩)
        else if (3 == m_iCurrentOniLayer)
            zakolist = pGameInstance->Find_GameObjectList(LEVEL_STORY, LAYER_ONI3);

        // 기존 오니 해제
        for (auto& pPreZako : m_pOniArr)
            Safe_Release(pPreZako);
        m_pOniArr.clear();
        for (auto& pZako : zakolist)
        {
            m_pOniArr.push_back(static_cast<CCharacter*>(pZako));
            Safe_AddRef(pZako);
            if (m_bIsStoryBattle)
                pZako->Set_Active(true);
        }

        // 다음 오니 레이어로 설정
        if (zakolist.empty() && true == m_bIsStoryBattle && ("IDLE" == m_pCharacterArr[CHAR_PLAYER]->Get_Animator()->Get_AnimationType()))
        {
            //pGameInstance->Stop_All();
            m_bIsStoryBattle = false;
            CTransform* pStoryTransform = m_pCharacterArr[CHAR_AI]->Get_Transform();
            CTransform* pBattleTransform = m_pCharacterArr[CHAR_PLAYER]->Get_Transform();
            if (nullptr != pStoryTransform || nullptr != pBattleTransform)
            {
                pStoryTransform->Bind_WorldMatrix(pBattleTransform->Get_WorldMatrix());
                pStoryTransform->Update_Navigation_Cell(pStoryTransform->Get_Pos());
            }

            CRenderer* pRenderer = (CRenderer*)m_pCharacterArr[CHAR_PLAYER]->Get_Component(COM_RENDERER);
            if (nullptr != pRenderer)
            {
                pRenderer->Start_FadeInOut(0.03f, false, 0);
            }

            if (m_iCurrentOniLayer < 3)
                m_iCurrentOniLayer++;
        }
    }
    RELEASE_INSTANCE(CGameInstance);
}


void CBattle_Manager::Push()
{
    if (LEVEL_GAMEPLAY == g_iCurrentLevel)
        m_pCharacterArr[CHAR_PLAYER]->Push(m_pCharacterArr[CHAR_AI]);
    else
    {
        m_pCharacterArr[CHAR_PLAYER]->Push(m_pCharacterArr[CHAR_AI]);
        m_pCharacterArr[CHAR_AI]->Push(m_pCharacterArr[CHAR_PLAYER]);
    }
}

void CBattle_Manager::InitBattle(const _double& _dTimeDelta)
{
    if (LEVEL_BATTLE == g_iCurrentLevel)
    {
        // 시작 컷신 이후에 타이머가 돌아간다.
        if (false == CCutScene_Manager::Get_Instance()->Is_GameInit())
            return;

        CCharacter::BATTLEDATA& tPlayerData = m_pCharacterArr[CHAR_PLAYER]->Get_BattleData();
        CCharacter::BATTLEDATA& tAIData = m_pCharacterArr[CHAR_AI]->Get_BattleData();

        // 배틀 초기화와 시작은 한 프레임만 true로 만들어준다.
        if (true == m_tBattleInfo.bBattleInit)
            m_tBattleInfo.bBattleInit = false;
        if (true == m_tBattleInfo.bBattleStart)
            m_tBattleInfo.bBattleStart = false;

        // 초기 한번만 들어온다.
        if (false == m_bInit)
        {
            m_bInit = true;
            m_tBattleInfo.bBattleInit = true;
        }

        // 슬로우 한 뒤 1초 뒤에 슬로우 풀고 피 회복한다.
        if (0.f != m_tBattleInfo.tStartTimer.fStandardTime)
        {
            if (m_tBattleInfo.tStartTimer.Is_Over(_dTimeDelta))
            {
                // 전투 시작
                Change_UpdateRatio(1.f);

                m_tBattleInfo.tStartTimer.fStandardTime = 0.f;
                m_tBattleInfo.tStartTimer.Reset_Timer();
                tPlayerData.Init();
                tAIData.Init();
            }
            return;
        }

        // 라운드가 끝나고 둘다 IDLE 경우에 라운드 재시작
        if ((true == tAIData.bBattleStop) && (true == tPlayerData.bBattleStop))
        {
            // 배틀종료
            if (tPlayerData.iWinCount >= 2 || tAIData.iWinCount >= 2)
            {
                tAIData.bBattleStop = false;
                tPlayerData.bBattleStop = false;
                m_tBattleInfo.bBattleStart = true;
                m_tBattleInfo.tBattleTimer.bPass = true;
                return;
            }

            // HP와 Charge게이지가 점점 회복
            if (tPlayerData.fCurHp < tPlayerData.fMaxHp || tAIData.fCurHp < tAIData.fMaxHp 
                || tPlayerData.fChargeValue < tPlayerData.fMaxChargeValue || tAIData.fChargeValue < tAIData.fMaxChargeValue)
            {
                tPlayerData.Increase_HP(1.5f);
                tAIData.Increase_HP(1.5f);
                tPlayerData.Increase_Charge(1.5f);
                tAIData.Increase_Charge(1.5f);
            }
            else if (("IDLE" == m_pCharacterArr[CHAR_PLAYER]->Get_Animator()->Get_AnimationType()) &&
                (("IDLE" == m_pCharacterArr[CHAR_AI]->Get_Animator()->Get_AnimationType()) || m_tBattleInfo.bSabitoAI))
            {
                tAIData.bBattleStop = false;
                tPlayerData.bBattleStop = false;
                m_tBattleInfo.bBattleStart = true;
                m_tBattleInfo.tBattleTimer.Reset_Timer();
            }
            return;
        }

        // 플레이어가 죽거나 타이머가 끝나면 슬로우 걸고 플레이어를 무적 처리 해준다.
        if (false == m_tBattleInfo.tBattleTimer.bPass)
        {
            if (m_tBattleInfo.tBattleTimer.Is_Over(_dTimeDelta) || tPlayerData.fCurHp <= 0.f || tAIData.fCurHp <= 0.f)
            {
                m_tBattleInfo.bBattleInit = true;
                m_tBattleInfo.tStartTimer.fStandardTime = 1.f;
                m_tBattleInfo.tStartTimer.Reset_Timer();

                tAIData.bBattleStop = true;
                tPlayerData.bBattleStop = true;
                Change_UpdateRatio(0.2f);
                if (tPlayerData.fCurHp < tAIData.fCurHp)
                    tAIData.Increase_WinCount();
                else
                    tPlayerData.Increase_WinCount();
            }
        }
    }
    else if (LEVEL_GAMEPLAY == g_iCurrentLevel || LEVEL_STORY == g_iCurrentLevel)
    {
        if (_dTimeDelta > 0.1 && false == m_bInit)
        {
            m_bInit = true;
            m_tBattleInfo.tStartTimer.fStandardTime = 0.2f;
            return;
        }

        if (true == m_tBattleInfo.bBattleStart)
            m_tBattleInfo.bBattleStart = false;

        if (0.f != m_tBattleInfo.tStartTimer.fStandardTime)
        {
            if (m_tBattleInfo.tStartTimer.Is_Over(_dTimeDelta))
            {
                m_tBattleInfo.bBattleStart = true;
                m_tBattleInfo.tStartTimer.fStandardTime = 0.f;
                m_tBattleInfo.tStartTimer.Reset_Timer();
            }
        }

    }

}

void CBattle_Manager::Change_UpdateRatio(_float fUpdateRatio)
{
    // 업데이트 시간 조절
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        _float* pUpdateTime = nullptr, * pLateUpdateTime = nullptr;
        pGameInstance->Out_LevelUpdateTime(pUpdateTime, pLateUpdateTime);
        if (nullptr != pUpdateTime)
            *pUpdateTime = fUpdateRatio;
        if (nullptr != pLateUpdateTime)
            *pLateUpdateTime = fUpdateRatio;
        m_tBattleInfo.fUpdateRatio = fUpdateRatio;
    }
    RELEASE_INSTANCE(CGameInstance);
}

void CBattle_Manager::Check_BattleDelayTimer(const _double& _dTimeDelta)
{
    // 딜레이 타이머가 설정되어있다면 캐릭터를 멈춘다.
    if (0.0 != m_tDelayTimer.fStandardTime)
    {
        _bool bIsDelay = false;
        // 기준치를 넘지 않았을때 까지 돌린다.
        bIsDelay = m_tDelayTimer.Is_Over(_dTimeDelta);

        // 캐릭터들 SetActive를 조절한다.
        m_pCharacterArr[CHAR_PLAYER]->Set_Active(bIsDelay);
        m_pCharacterArr[CHAR_AI]->Set_Active(bIsDelay);

        if (true == bIsDelay)
        {
            m_tDelayTimer.Reset_Timer();
            m_tDelayTimer.fStandardTime = 0.0;
        }
    }
    else
    {
        if (false == m_pCharacterArr[CHAR_PLAYER]->Is_Active())
        {
            // 캐릭터들 SetActive를 조절한다.
            m_pCharacterArr[CHAR_PLAYER]->Set_Active(true);
        }
        if (false == m_pCharacterArr[CHAR_AI]->Is_Active())
        {
            // 캐릭터들 SetActive를 조절한다.
            m_pCharacterArr[CHAR_AI]->Set_Active(true);
        }
    }
}

void CBattle_Manager::Check_StoryDelayTimer(const _double& _dTimeDelta)
{
    if (0.0 != m_tDelayTimer.fStandardTime)
    {
        _bool bIsDelay = false;
        // 기준치를 넘지 않았을때 까지 돌린다.
        bIsDelay = m_tDelayTimer.Is_Over(_dTimeDelta);

        // 캐릭터들 SetActive를 조절한다.
        for (auto& pOni : m_pOniArr)
        {
            if (pOni->Is_Hit())
                pOni->Set_Active(bIsDelay);
        }
        m_pCharacterArr[CHAR_PLAYER]->Set_Active(bIsDelay);

        if (true == bIsDelay)
        {
            m_tDelayTimer.Reset_Timer();
            m_tDelayTimer.fStandardTime = 0.0;
        }
    }
    else
    {
        // 캐릭터들 SetActive를 조절한다.
        for (auto& pOni : m_pOniArr)
        {
            if (false == pOni->Is_Active())
                pOni->Set_Active(true);
        }
        if (false == m_pCharacterArr[CHAR_PLAYER]->Is_Active())
            m_pCharacterArr[CHAR_PLAYER]->Set_Active(true);
    }
}

void CBattle_Manager::Free()
{
    for (_uint i = 0; i < CHAR_END; ++i)
        Safe_Release(m_pCharacterArr[i]);

    for (auto& pOni : m_pOniArr)
        Safe_Release(pOni);
    m_pOniArr.clear();
}
