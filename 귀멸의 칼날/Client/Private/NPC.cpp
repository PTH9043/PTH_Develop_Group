#include "NPC.h"
#include "GameInstance.h"
#include "Animation.h"
#include "HierarchyNode.h"
#include "Player.h"
#include "UI_Icon.h"
#include "Camera_Free.h"
#include "Story_Tanjiro.h"
#include "Sheath.h"
#include "NPC_Item.h"
#include "Camera.h"
#include "Battle_Manager.h"

CNPC::CNPC(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CPawn(_pDevice, _pContext)
{
}

CNPC::CNPC(const CNPC& _rhs)
    : CPawn(_rhs)
{
}

HRESULT CNPC::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC::NativeConstruct_Clone(void* pArg)
{
    m_iType = *(_int*)pArg;
	
    if (FAILED(__super::NativeConstruct_Clone(pArg)))
        return E_FAIL;

   // if (FAILED(SetUp_Components()))
     //   return E_FAIL;

    Change_Model(m_iType);
    Setting_Positon(m_iType);

    return S_OK;
}

void CNPC::Tick(const _double& _dTimeDelta)
{
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

    __super::Tick(_dTimeDelta);

    if (m_pAnimator != nullptr)
        m_pAnimator->Tick(_dTimeDelta);


    if (nullptr != m_pModel && nullptr != m_pAllAnimModel)
    {
        for (auto& iter : m_pModel->Get_HierachyNodeList())
        {
            for (auto& pAnimBone : m_pAllAnimModel->Get_HierachyNodeList())
            {
                if (iter->Get_Name() == pAnimBone->Get_Name())
                {
                    iter->Set_TransformationMatrix(pAnimBone->Get_TransformMatrix());
                    continue;
                }
            }
        }
    }

    Setting_Trigger(_dTimeDelta);
	
    RELEASE_INSTANCE(CGameInstance);
}

void CNPC::Late_Tick(const _double& _dTimeDelta)
{
    __super::Late_Tick(_dTimeDelta);

    if (true == CGameInstance::Get_Instance()->Is_Rendering(m_pTransformCom->Get_Pos(), 20.f))
    {
        if (nullptr != m_pRendererCom)
            m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
    }
}

HRESULT CNPC::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    if (FAILED(SetUp_ConstantTable()))
        return E_FAIL;

    if (nullptr != m_pModel)
    {
        _uint iNumMeshContainers = m_pModel->Get_NumMeshContainers();

        for (_uint i = 0; i < iNumMeshContainers; ++i)
        {
            if (FAILED(m_pModel->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TEXTYPE::TextureType_DIFFUSE)))
                return E_FAIL;

            m_pModel->Bind_SRV(m_pShaderCom, "g_OutLineTexture", i, TextureType_SPECULAR);

            m_pModel->Render(i, m_pShaderCom, 0);
        }
    }

    return S_OK;
}

void CNPC::Extern_ImGuiClass()
{

     _float3 vPos;
     XMStoreFloat3(&vPos, m_pTransformCom->Get_Pos());

     _float fRotate;

     if (ImGui::DragFloat3("Pos", &vPos.x, 0.1f, -10000.f, 10000.f))
         m_pTransformCom->Set_PosF3(vPos);

     if (ImGui::DragFloat("Rotate", &fRotate, 0.1f, -300.f, 300.f))
     m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 1.f), fRotate);


     if (ImGui::DragFloat3("Lentgh", &m_Length.x, 0.1f, -400.f, 400.f))
     {
		
     }
}

void CNPC::Change_Model(_uint Type)
{
    if (g_iCurrentLevel == LEVEL_STORY)
    {
        switch (Type)
        {
        case Client::CNPC::NPC_CANDIDATE:
            m_pModel = m_ModelList[NPC_CANDIDATE];
            m_pAnimator = m_AnimatorList[NPC_CANDIDATE];
            break;
        case Client::CNPC::NPC_SIT_CANDIDATE:
            m_pModel = m_ModelList[NPC_SIT_CANDIDATE];
            m_pAnimator = m_AnimatorList[NPC_SIT_CANDIDATE];
            break;
        case Client::CNPC::NPC_DEAD_CANDIDATE:
            m_pModel = m_ModelList[NPC_DEAD_CANDIDATE];
            m_pAnimator = m_AnimatorList[NPC_DEAD_CANDIDATE];
            break;
        }
    }
    else if (g_iCurrentLevel == LEVEL_RATTAN)
    {
        switch (Type)
        {
        case Client::CNPC::NPC_AOI:
            m_pModel = m_ModelList[NPC_AOI];
            m_pAnimator = m_AnimatorList[NPC_AOI];
            break;
        case Client::CNPC::NPC_SISTER_L:
            m_pModel = m_ModelList[NPC_SISTER_L];
            m_pAnimator = m_AnimatorList[NPC_SISTER_L];
            break;
        case Client::CNPC::NPC_SISTER_R:
            m_pModel = m_ModelList[NPC_SISTER_R];
            m_pAnimator = m_AnimatorList[NPC_SISTER_R];
            break;
        case Client::CNPC::NPC_ZENITSU:
            m_pModel = m_ModelList[NPC_ZENITSU];
            m_pAnimator = m_AnimatorList[NPC_ZENITSU];
            break;
        case Client::CNPC::NPC_KANAWO:
            m_pModel = m_ModelList[NPC_KANAWO];
            m_pAnimator = m_AnimatorList[NPC_KANAWO];
            break;
        case Client::CNPC::RATTAN_CANDIDATE:
            m_pModel = m_ModelList[RATTAN_CANDIDATE];
            m_pAnimator = m_AnimatorList[RATTAN_CANDIDATE];
            break;
        case Client::CNPC::NPC_GENYA:
            m_pModel = m_ModelList[NPC_GENYA];
            m_pAnimator = m_AnimatorList[NPC_GENYA];
            break;
  
        }
    }
}

void CNPC::Setting_Positon(_uint Type)
{
    if (g_iCurrentLevel == LEVEL_STORY)
    {
        switch (Type)
        {
        case Client::CNPC::NPC_CANDIDATE:
            m_pTransformCom->Set_PosF3(_float3(-526.1f, 169.7f, 347.2f));
            m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 1.f), 100.f);
            break;
        case Client::CNPC::NPC_SIT_CANDIDATE:
            m_pTransformCom->Set_PosF3(_float3(-1192.f, 211.9f, 542.8f));
            m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 1.f), 180.f);
            break;
        case Client::CNPC::NPC_DEAD_CANDIDATE:
            m_pTransformCom->Set_PosF3(_float3(-1305.f, 193.4f, 332.f));
            m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 1.f), 90.f);
            break;
        }
    }

    else if (g_iCurrentLevel == LEVEL_RATTAN)
    {
        switch (Type)
        {
        case Client::CNPC::NPC_AOI:
            m_pTransformCom->Set_PosF3(_float3(-5.5f, 8.6f, -28.5f));
            break;
        case Client::CNPC::NPC_SISTER_L:
            m_pTransformCom->Set_PosF3(_float3(19.4f, 8.6f, 2.5f));
            m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 1.f), 270.f);
            break;
        case Client::CNPC::NPC_SISTER_R:
            m_pTransformCom->Set_PosF3(_float3(19.4f, 8.6f, -5.7f));
            m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 1.f), 270.f);
            break;
        case Client::CNPC::NPC_ZENITSU:
            m_pTransformCom->Set_PosF3(_float3(-22.9f, 8.6f, 18.5f));
            m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 1.f), 270.f);
            break;
        case Client::CNPC::NPC_KANAWO:
            m_pTransformCom->Set_PosF3(_float3(-35.f, 8.6f,-21.9f));
            m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 1.f), 270.f);
            break;
        case Client::CNPC::RATTAN_CANDIDATE:
            m_pTransformCom->Set_PosF3(_float3(-17.7f, 8.6f, 22.6f));
            m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 1.f), 200.f);
            Change_State(STAND);
            break;
        case Client::CNPC::NPC_GENYA:
            m_pTransformCom->Set_PosF3(_float3(11.9f, 8.6f, 22.4f));
            m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 1.f), 300.f);
            break;
        }
    }
}

void CNPC::Setting_Trigger(const _double& _dTimeDelta)
{
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

#pragma region 스토리맵

    if (g_iCurrentLevel == LEVEL_STORY)
    {
        CPlayer* StoryPlayer = dynamic_cast<CPlayer*>(pGameInstance->Find_GameObject(g_iCurrentLevel, LAYER_STORYPLAYER, 0));

        if (StoryPlayer == nullptr)
        {
            RELEASE_INSTANCE(CGameInstance);
            return;
        }
        CTransform* PlayerTransform = StoryPlayer->Get_Transform();

        _float3 fLength = m_pTransformCom->Get_PosF3() - PlayerTransform->Get_PosF3();
        _vector vLength = XMLoadFloat3(&fLength);
        vLength = XMVector3Length(vLength);
        XMStoreFloat3(&m_Length, vLength);

        if (m_iType == NPC_CANDIDATE)
        {
            if (m_Length.x <= 30.f && m_PopText == false)
            {
                _uint TalkPoP = CUI_Icon::QUEST_TALK;
                if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
                    LAYER_QUEST, PROTO_GOB_UI_ICON, &TalkPoP)))
                {
                    Safe_Release(pGameInstance);
                    return;
                }
                // CUI_Icon* icon = dynamic_cast<CUI_Icon*>(pGameInstance->Clone_GameObject_Add());
                m_PopText = true;
            }

            if (m_Length.x >= 30.f && m_PopText == true) //플레이어가 멀어지고 생성이 되어있다면 
            {
                list<CGameObject*> Prototype_IconObjects = pGameInstance->Find_GameObjectList(LEVEL_STORY, LAYER_QUEST);

                CUI_Icon* icon = nullptr;
                for (auto it = Prototype_IconObjects.begin(); it != Prototype_IconObjects.end(); it++)
                {
                    if (dynamic_cast<CUI_Icon*>(*it)->Get_Type() == CUI_Icon::QUEST_TALK)
                    {
                        icon = dynamic_cast<CUI_Icon*>(*it);
                        break;
                    }
                }

                if (icon != nullptr)
                    icon->Set_TalkRender(false);
                else
                {
                    RELEASE_INSTANCE(CGameInstance);
                    return;
                }
            }
            else if (m_Length.x <= 30.f && m_PopText == true && m_PopRender == false)
            {
                list<CGameObject*> Prototype_IconObjects = pGameInstance->Find_GameObjectList(LEVEL_STORY, LAYER_QUEST);

                CUI_Icon* icon = nullptr;
                for (auto it = Prototype_IconObjects.begin(); it != Prototype_IconObjects.end(); it++)
                {
                    if (dynamic_cast<CUI_Icon*>(*it)->Get_Type() == CUI_Icon::QUEST_TALK)
                    {
                        icon = dynamic_cast<CUI_Icon*>(*it);
                        break;
                    }
                }
                if (icon != nullptr)
                    icon->Set_TalkRender(true);
                else
                {
                    Safe_Release(pGameInstance);
                    return;
                }
                if (pGameInstance->Get_DIKeyDown(DIK_F))
                {
                    pGameInstance->Play_SoundW(TEXT("UI_TalkPop"), 0.3f);
                    Change_State(TALK);
                    icon->Set_TalkRender(false);
                    m_PopRender = true;
                }
            }
        }

        //임시로 카메라 
        CPlayer* StoryCamera = dynamic_cast<CPlayer*>(pGameInstance->Find_GameObject(g_iCurrentLevel, LAYER_STORYPLAYER, 0));

        if (StoryCamera == nullptr)
        {
            RELEASE_INSTANCE(CGameInstance);
            return;
        }

        CTransform* CameraTransform = StoryCamera->Get_Transform();

        _float3 fCamLength = m_pTransformCom->Get_PosF3() - CameraTransform->Get_PosF3();
        _vector vCamLength = XMLoadFloat3(&fCamLength);
        vCamLength = XMVector3Length(vCamLength);
        XMStoreFloat3(&m_CamLength, vCamLength);

        if (m_iType == NPC_SIT_CANDIDATE)
        {
            if (m_CamLength.x <= 30.f && m_PopText == false)
            {
                _uint TalkPoP = CUI_Icon::QUEST_TALK2;
                if (FAILED(pGameInstance->Clone_GameObject(LEVEL_STORY,
                    LAYER_QUEST, PROTO_GOB_UI_ICON, &TalkPoP)))
                {
                    Safe_Release(pGameInstance);
                    return;
                }
                // CUI_Icon* icon = dynamic_cast<CUI_Icon*>(pGameInstance->Clone_GameObject_Add());
                m_PopText = true;
            }

            if (m_CamLength.x >= 30.f && m_PopText == true) //플레이어가 멀어지고 생성이 되어있다면 
            {

                list<CGameObject*> Prototype_IconObjects = pGameInstance->Find_GameObjectList(LEVEL_STORY, LAYER_QUEST);

                CUI_Icon* NPCtalk = nullptr;
                for (auto it = Prototype_IconObjects.begin(); it != Prototype_IconObjects.end(); it++)
                {
                    if (dynamic_cast<CUI_Icon*>(*it)->Get_Type() == CUI_Icon::QUEST_TALK2)
                    {
                        NPCtalk = dynamic_cast<CUI_Icon*>(*it);
                        break;
                    }
                }

                if (NPCtalk != nullptr)
                    NPCtalk->Set_TalkRender(false);
                else
                {
                    RELEASE_INSTANCE(CGameInstance);
                    return;
                }
            }
            else if (m_CamLength.x <= 30.f && m_PopText == true && m_PopRender == false)
            {

                list<CGameObject*> Prototype_IconObjects = pGameInstance->Find_GameObjectList(LEVEL_STORY, LAYER_QUEST);

                CUI_Icon* NPCtalk = nullptr;
                for (auto it = Prototype_IconObjects.begin(); it != Prototype_IconObjects.end(); it++)
                {
                    if (dynamic_cast<CUI_Icon*>(*it)->Get_Type() == CUI_Icon::QUEST_TALK2)
                    {
                        NPCtalk = dynamic_cast<CUI_Icon*>(*it);
                        break;
                    }
                }


                if (NPCtalk != nullptr) 
                    NPCtalk->Set_TalkRender(true);
                else
                {
                    Safe_Release(pGameInstance);
                    return;
                }
                if (pGameInstance->Get_DIKeyDown(DIK_F))
                {
                    pGameInstance->Play_SoundW(TEXT("UI_TalkPop"), 0.3f);
                    Change_State(NPC_TALK);
                    NPCtalk->Set_TalkRender(false);
                    m_PopRender = true;
                }
            }
        }
    }
#pragma endregion 
        //==============================================================================

        else if (g_iCurrentLevel == LEVEL_RATTAN)
        {
            //일단 카메라로 대체 임시로 
            CPlayer* Player = dynamic_cast<CPlayer*>(pGameInstance->Find_GameObject(g_iCurrentLevel, LAYER_STORYPLAYER, 0));

            if (Player == nullptr)
            {
                RELEASE_INSTANCE(CGameInstance);
                return;
            }
            CTransform* PlayerTransform = Player->Get_Transform();

            _float3 fLength = m_pTransformCom->Get_PosF3() - PlayerTransform->Get_PosF3();
            _vector vLength = XMLoadFloat3(&fLength);
            vLength = XMVector3Length(vLength);
            XMStoreFloat3(&m_Length, vLength);


#pragma region 아오이

            if (m_iType == NPC_AOI)
            {
                if (m_Length.x <= 15.f && m_PopText == false)
                {
                    _uint AoiTalk = CUI_Icon::AOI_TALK;
                    if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
                        LAYER_QUEST, PROTO_GOB_UI_ICON, &AoiTalk)))
                    {
                        Safe_Release(pGameInstance);
                        return;
                    }
                    m_PopText = true;
                }

                if (m_Length.x <= 15.f && pGameInstance->Get_DIKeyUp(DIK_F))
                {
                    pGameInstance->Play_SoundW(TEXT("UI_TalkPop"), 0.3f);
                    Change_State(TURN);
                }
                if ((m_Length.x >= 15.f && m_PopText == true) || CurrentState == TURN || CurrentState == START) //플레이어가 멀어지고 생성이 되어있다면 
                {
                    list<CGameObject*> Prototype_IconObjects = pGameInstance->Find_GameObjectList(LEVEL_RATTAN, LAYER_QUEST);

                    CUI_Icon* Aoitalk = nullptr;
                    for (auto it = Prototype_IconObjects.begin(); it != Prototype_IconObjects.end(); it++)
                    {
                        if (dynamic_cast<CUI_Icon*>(*it)->Get_Type() == CUI_Icon::AOI_TALK)
                        {
                            Aoitalk = dynamic_cast<CUI_Icon*>(*it);
                            break;
                        }
                    }

                    if (Aoitalk != nullptr)
                        Aoitalk->Set_TalkRender(false);
                    else
                    {
                        Safe_Release(pGameInstance);
                        return;
                    }
                }
                else if (m_Length.x <= 15.f && m_PopText == true) //플레이어가 다시 가까워지고 생성이 되어있다면 
                {
                    list<CGameObject*> Prototype_IconObjects = pGameInstance->Find_GameObjectList(LEVEL_RATTAN, LAYER_QUEST);

                    CUI_Icon* Aoitalk = nullptr;
                    for (auto it = Prototype_IconObjects.begin(); it != Prototype_IconObjects.end(); it++)
                    {
                        if (dynamic_cast<CUI_Icon*>(*it)->Get_Type() == CUI_Icon::AOI_TALK)
                        {
                            Aoitalk = dynamic_cast<CUI_Icon*>(*it);
                            break;
                        }
                    }

                    if (Aoitalk != nullptr)
                        Aoitalk->Set_TalkRender(true);
                    else
                    {
                        Safe_Release(pGameInstance);
                        return;
                    }
                }
            }
#pragma endregion 

#pragma region 자매

            if (m_iType == NPC_SISTER_R)
            {
                if (m_Length.x <= 20.f && m_PopText == false)
                {
                    _uint SisterTalk = CUI_Icon::SISTER_TALK;
                    if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
                        LAYER_QUEST, PROTO_GOB_UI_ICON, &SisterTalk)))
                    {
                        Safe_Release(pGameInstance);
                        return;
                    }
                    m_PopText = true;
                }

                if (m_Length.x <= 20.f && pGameInstance->Get_DIKeyUp(DIK_F) && CurrentState != TALK_END)
                {
                    Change_State(TALK);
                    pGameInstance->Play_SoundW(TEXT("UI_MinigameTalk"), 0.3f);
                }


                if (m_Length.x >= 20.f && m_PopText == true)  //플레이어가 멀어지고 생성이 되어있다면 
                {
                    list<CGameObject*> Prototype_IconObjects = pGameInstance->Find_GameObjectList(LEVEL_RATTAN, LAYER_QUEST);

                    CUI_Icon* Sistertalk = nullptr;
                    for (auto it = Prototype_IconObjects.begin(); it != Prototype_IconObjects.end(); it++)
                    {
                        if (dynamic_cast<CUI_Icon*>(*it)->Get_Type() == CUI_Icon::SISTER_TALK)
                        {
                            Sistertalk = dynamic_cast<CUI_Icon*>(*it);
                            break;
                        }
                    }

                    if (Sistertalk != nullptr)
                        Sistertalk->Set_TalkRender(false);
                    else
                    {
                        Safe_Release(pGameInstance);
                        return;
                    }
                }
                else if (m_Length.x <= 20.f && m_PopText == true) //플레이어가 다시 가까워지고 생성이 되어있다면 
                {
                    list<CGameObject*> Prototype_IconObjects = pGameInstance->Find_GameObjectList(LEVEL_RATTAN, LAYER_QUEST);

                    CUI_Icon* Sistertalk = nullptr;
                    for (auto it = Prototype_IconObjects.begin(); it != Prototype_IconObjects.end(); it++)
                    {
                        if (dynamic_cast<CUI_Icon*>(*it)->Get_Type() == CUI_Icon::SISTER_TALK)
                        {
                            Sistertalk = dynamic_cast<CUI_Icon*>(*it);
                            break;
                        }
                    }

                    if (Sistertalk != nullptr)
                        Sistertalk->Set_TalkRender(true);
                    else
                    {
                        Safe_Release(pGameInstance);
                        return;
                    }
                }
            }
#pragma endregion 

#pragma region 젠이츠
            if (m_iType == NPC_ZENITSU)
            {
                _float3 pos = m_pTransformCom->Get_PosF3();

                //타원형으로 돌아
                if (m_WalkStop == false)
                {
                    Change_State(WALK);
                    _float3 pos = m_pTransformCom->Get_PosF3();

                    m_Angle += (_float)_dTimeDelta * 60.f;

                    if (m_Angle >= 360.f)
                        m_Angle = 0.f;

                    pos.x = cos(XMConvertToRadians(m_Angle)) * 12.f + -41.7f;
                    pos.z = sin(XMConvertToRadians(m_Angle)) * 5.f + 28.7f;

                    m_pTransformCom->Set_PosF3(pos);

                    _float3 lookat = m_pTransformCom->Get_PosF3();

                    lookat.x += -1.f * sin(XMConvertToRadians(m_Angle)) * 12.f;
                    lookat.z += cos(XMConvertToRadians(m_Angle)) * 5.f;

                    _vector vlookat = XMLoadFloat3(&lookat);

                    m_pTransformCom->LookAt(vlookat);
                    m_pTransformCom->Set_PosF3(pos);
                }

                if (m_Length.x <= 15.f && m_PopText == false) //가까이가면 말걸기 
                {
                    _uint zenitsuTalk = CUI_Icon::ZENITSU_TALK;
                    if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
                        LAYER_QUEST, PROTO_GOB_UI_ICON, &zenitsuTalk)))
                    {
                        Safe_Release(pGameInstance);
                        return;
                    }
                    m_PopText = true;
                }


                list<CGameObject*> Prototype_IconObjects = pGameInstance->Find_GameObjectList(LEVEL_RATTAN, LAYER_QUEST);

                CUI_Icon* zenitsu = nullptr;
                for (auto it = Prototype_IconObjects.begin(); it != Prototype_IconObjects.end(); it++)
                {
                    if (dynamic_cast<CUI_Icon*>(*it)->Get_Type() == CUI_Icon::ZENITSU_TALK)
                    {
                        zenitsu = dynamic_cast<CUI_Icon*>(*it);
                        break;
                    }
                }

                if (m_Length.x <= 15.f && pGameInstance->Get_DIKeyUp(DIK_F) && m_WalkStop == false && CurrentState != SCARY)
                {
                    pGameInstance->Play_SoundW(TEXT("UI_TalkPop"), 0.3f);

                    Change_State(STAND); //말걸면 서
                    m_WalkStop = true; //말걸면 걷지마 

                    //말걸면 나를 바라봐 
                    _float3 look = PlayerTransform->Get_LookF3();
                    _float3 right = PlayerTransform->Get_RightF3();
                    m_pTransformCom->Set_LookF3(look * -1.f);
                    m_pTransformCom->Set_RightF3(right * -1.f);

                    if (zenitsu != nullptr)
                    {
                        zenitsu->Set_TalkRender(false);
                        m_PopRender = true;
                    }
                    else
                    {
                        Safe_Release(pGameInstance);
                        return;
                    }
                }

                if (m_Length.x >= 15.f && m_PopText == true)  //플레이어가 멀어지고 생성이 되어있다면 
                {
                    if (zenitsu != nullptr)
                        zenitsu->Set_TalkRender(false);

                    else
                    {
                        Safe_Release(pGameInstance);
                        return;
                    }
                }
                //다시말걸지마 (m_PopRender)
                else if (m_Length.x <= 15.f && m_PopText == true && m_PopRender != true) //플레이어가 다시 가까워지고 생성이 되어있다면 
                {
                    if (zenitsu != nullptr)
                        zenitsu->Set_TalkRender(true);
                    else
                    {
                        Safe_Release(pGameInstance);
                        return;
                    }
                }

                // m_pTransformCom->Set_PosF3(pos);
            }
#pragma endregion 

#pragma region 카나오
            if (m_iType == NPC_KANAWO)
            {
                list<CGameObject*> Prototype_IconObjects = pGameInstance->Find_GameObjectList(LEVEL_RATTAN, LAYER_QUEST);

                CUI_Icon* kanawo = nullptr;
                for (auto it = Prototype_IconObjects.begin(); it != Prototype_IconObjects.end(); it++)
                {
                    if (dynamic_cast<CUI_Icon*>(*it)->Get_Type() == CUI_Icon::KANAWO_TALK)
                    {
                        kanawo = dynamic_cast<CUI_Icon*>(*it);
                        break;
                    }
                }

                if (m_Length.x <= 15.f && m_PopText == false) //가까이가면 말걸기 아이콘
                {
                    _uint kanawoTalk = CUI_Icon::KANAWO_TALK;
                    if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
                        LAYER_QUEST, PROTO_GOB_UI_ICON, &kanawoTalk)))
                    {
                        Safe_Release(pGameInstance);
                        return;
                    }
                    m_PopText = true;
                }

                if (m_Length.x <= 15.f && pGameInstance->Get_DIKeyUp(DIK_F) && CurrentState != TALK_END)
                {
                    pGameInstance->Play_SoundW(TEXT("UI_TalkPop"), 0.3f);

                    //말걸면 나를 바라봐 
                    _float3 look = PlayerTransform->Get_LookF3();
                    _float3 right = PlayerTransform->Get_RightF3();
                    m_pTransformCom->Set_LookF3(look * -1.f);
                    m_pTransformCom->Set_RightF3(right * -1.f);
                    Change_State(TALK);

                    if (kanawo != nullptr)
                    {
                        kanawo->Set_TalkRender(false);
                        m_PopRender = true;
                    }
                    else
                    {
                        Safe_Release(pGameInstance);
                        return;
                    }
                }

                if (m_Length.x >= 15.f && m_PopText == true)  //플레이어가 멀어지고 생성이 되어있다면 
                {
                    if (kanawo != nullptr)
                        kanawo->Set_TalkRender(false);

                    else
                    {
                        Safe_Release(pGameInstance);
                        return;
                    }
                }
                //다시말걸지마 (m_PopRender)
                else if (m_Length.x <= 15.f && m_PopText == true && m_PopRender != true) //플레이어가 다시 가까워지고 생성이 되어있다면 
                {
                    if (kanawo != nullptr)
                        kanawo->Set_TalkRender(true);
                    else
                    {
                        Safe_Release(pGameInstance);
                        return;
                    }
                }

            }
#pragma endregion 

#pragma region 견습대원
            if (m_iType == RATTAN_CANDIDATE)
            {
               
                list<CGameObject*> Prototype_IconObjects = pGameInstance->Find_GameObjectList(LEVEL_RATTAN, LAYER_QUEST);

                CUI_Icon* candidate = nullptr;
                for (auto it = Prototype_IconObjects.begin(); it != Prototype_IconObjects.end(); it++)
                {
                    if (dynamic_cast<CUI_Icon*>(*it)->Get_Type() == CUI_Icon::CANDIDATE_TALK)
                    {
                        candidate = dynamic_cast<CUI_Icon*>(*it);
                        break;
                    }
                }

                if (m_Length.x <= 15.f && m_PopText == false) //가까이가면 말걸기 아이콘
                {
                    _uint CandidateTalk = CUI_Icon::CANDIDATE_TALK;
                    if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
                        LAYER_QUEST, PROTO_GOB_UI_ICON, &CandidateTalk)))
                    {
                        Safe_Release(pGameInstance);
                        return;
                    }
                    m_PopText = true;
                }

                if (m_Length.x <= 15.f && pGameInstance->Get_DIKeyUp(DIK_F) && CurrentState != TALK_END)
                {
                    pGameInstance->Play_SoundW(TEXT("UI_TalkPop"), 0.3f);

                    //말걸면 나를 바라봐 
                    _float3 look = PlayerTransform->Get_LookF3();
                    _float3 right = PlayerTransform->Get_RightF3();
                    m_pTransformCom->Set_LookF3(look * -1.f);
                    m_pTransformCom->Set_RightF3(right * -1.f);
                    Change_State(TALK);

                    if (candidate != nullptr)
                    {
                        candidate->Set_TalkRender(false);
                        m_PopRender = true;
                    }
                    else
                    {
                        Safe_Release(pGameInstance);
                        return;
                    }
                }

                if (m_Length.x >= 15.f && m_PopText == true)  //플레이어가 멀어지고 생성이 되어있다면 
                {
                    if (candidate != nullptr)
                        candidate->Set_TalkRender(false);

                    else
                    {
                        Safe_Release(pGameInstance);
                        return;
                    }
                }
                //다시말걸지마 (m_PopRender)
                else if (m_Length.x <= 15.f && m_PopText == true && m_PopRender != true) //플레이어가 다시 가까워지고 생성이 되어있다면 
                {
                    if (candidate != nullptr)
                        candidate->Set_TalkRender(true);
                    else
                    {
                        Safe_Release(pGameInstance);
                        return;
                    }
                }
            }
#pragma endregion 

#pragma region 겐야
            if (m_iType == NPC_GENYA)
            {
                list<CGameObject*> Prototype_IconObjects = pGameInstance->Find_GameObjectList(LEVEL_RATTAN, LAYER_QUEST);

                CUI_Icon* genya = nullptr;
                for (auto it = Prototype_IconObjects.begin(); it != Prototype_IconObjects.end(); it++)
                {
                    if (dynamic_cast<CUI_Icon*>(*it)->Get_Type() == CUI_Icon::GENYA_TALK)
                    {
                        genya = dynamic_cast<CUI_Icon*>(*it);
                        break;
                    }
                }

                if (m_Length.x <= 15.f && m_PopText == false) //가까이가면 말걸기 아이콘
                {
                    _uint GenyaTalk = CUI_Icon::GENYA_TALK;
                    if (FAILED(pGameInstance->Clone_GameObject(LEVEL_RATTAN,
                        LAYER_QUEST, PROTO_GOB_UI_ICON, &GenyaTalk)))
                    {
                        Safe_Release(pGameInstance);
                        return;
                    }
                    m_PopText = true;
                }

                if (m_Length.x <= 15.f && pGameInstance->Get_DIKeyUp(DIK_F) && CurrentState != TALK_END)
                {
                    pGameInstance->Play_SoundW(TEXT("UI_TalkPop"), 0.3f);

                    //말걸면 나를 바라봐 
                    _float3 look = PlayerTransform->Get_LookF3();
                    _float3 right = PlayerTransform->Get_RightF3();
                    m_pTransformCom->Set_LookF3(look * -1.f);
                    m_pTransformCom->Set_RightF3(right * -1.f);
                    Change_State(TALK);

                    if (genya != nullptr)
                    {
                        genya->Set_TalkRender(false);
                        m_PopRender = true;
                    }
                    else
                    {
                        Safe_Release(pGameInstance);
                        return;
                    }
                }

                if (m_Length.x >= 15.f && m_PopText == true)  //플레이어가 멀어지고 생성이 되어있다면 
                {
                    if (genya != nullptr)
                        genya->Set_TalkRender(false);

                    else
                    {
                        Safe_Release(pGameInstance);
                        return;
                    }
                }
                //다시말걸지마 (m_PopRender)
                else if (m_Length.x <= 15.f && m_PopText == true && m_PopRender != true) //플레이어가 다시 가까워지고 생성이 되어있다면 
                {
                    if (genya != nullptr)
                        genya->Set_TalkRender(true);
                    else
                    {
                        Safe_Release(pGameInstance);
                        return;
                    }
                }
            }
#pragma endregion 
        }
        RELEASE_INSTANCE(CGameInstance);
    }


void CNPC::Change_State(_uint TriggerType)
{
   

    CurrentState = TriggerType;

    if (PrevState == CurrentState)
        return;
    PrevState = CurrentState;

    switch (CurrentState)
    {
        case Client::CNPC::TURN: //아오이 말걸었을때
           m_pAnimator->Input_Trigger("Turn");
           {
               CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
               pGameInstance->Play_SoundW(TEXT("UI_MinigameTalk"), 0.2f);
               pGameInstance->Play_SoundW(TEXT("UI_AoiTalk"), 0.3f);
               RELEASE_INSTANCE(CGameInstance);
               CCharacter* pStoryChar = CBattle_Manager::Get_Instance()->Get_BattleCharacter(CBattle_Manager::CHAR_AI);
               if (nullptr != pStoryChar)
               {
                   pStoryChar->Get_Animator()->Input_Trigger("TALK");
               }
           }
             break;
        case Client::CNPC::START: //아오이 대화끝난후
        { 
            CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
            pGameInstance->Play_SoundW(TEXT("UI_RattanSelect"), 0.3f);
            
             RELEASE_INSTANCE(CGameInstance);
             m_pAnimator->Input_Trigger("Start");
        }
            break;
        case Client::CNPC::TALK://트리거 설정x 말걸었을때의 상태를 주기위해 
        {
            CCharacter* pStoryChar = CBattle_Manager::Get_Instance()->Get_BattleCharacter(CBattle_Manager::CHAR_AI);
            if (nullptr != pStoryChar)
            {
                pStoryChar->Get_Animator()->Input_Trigger("TALK");
            }
        }
            break;
        case Client::CNPC::TALK_END://트리거 설정x 
        {
            CCharacter* pStoryChar = CBattle_Manager::Get_Instance()->Get_BattleCharacter(CBattle_Manager::CHAR_AI);
            if (nullptr != pStoryChar)
            {
                pStoryChar->Get_Animator()->Input_Trigger("TALK_END");
            }
        }
            break;
        case Client::CNPC::WALK:
            m_pAnimator->Input_Trigger("Walk");
            break;
        case Client::CNPC::STAND: //젠이츠 대화 걸었을떄
            m_pAnimator->Input_Trigger("Stand");
            {
                CCharacter* pStoryChar = CBattle_Manager::Get_Instance()->Get_BattleCharacter(CBattle_Manager::CHAR_AI);
                if (nullptr != pStoryChar)
                {
                    pStoryChar->Get_Animator()->Input_Trigger("TALK");
                }
            }
            break;
        case Client::CNPC::SCARY: //젠이츠 대화 끝난후
            m_pAnimator->Input_Trigger("Scary");
            {
                CCharacter* pStoryChar = CBattle_Manager::Get_Instance()->Get_BattleCharacter(CBattle_Manager::CHAR_AI);
                if (nullptr != pStoryChar)
                {
                    pStoryChar->Get_Animator()->Input_Trigger("TALK_END");
                }
            }
            break;
        case Client::CNPC::NPC_TALK: //겐야 말걸었을때 
        { 
            m_pAnimator->Input_Trigger("Talk");
              CCharacter* pStoryChar = CBattle_Manager::Get_Instance()->Get_BattleCharacter     (CBattle_Manager::CHAR_AI);
              if (nullptr != pStoryChar)
              {
                  pStoryChar->Get_Animator()->Input_Trigger("TALK");
              }
        }
            break;
        case Client::CNPC::TRIGGER_END: 
        break;
    }
   
}

HRESULT CNPC::SetUp_ConstantTable()
{
    if (nullptr == m_pTransformCom || nullptr == m_pShaderCom)
        return E_FAIL;


    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        _float4x4 WorldMatrix, ViewMatrix, ProjMatrix, PrevViewMatrix;
        WorldMatrix = m_pTransformCom->Get_WorldFloat4x4_TP();
        ViewMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW);
        ProjMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ);
        PrevViewMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PREVVIEW);
        _float camFar = pGameInstance->Get_CamFar();

        if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
        {
            RELEASE_INSTANCE(CGameInstance);
            return E_FAIL;
        }
        if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
        {
            RELEASE_INSTANCE(CGameInstance);
            return E_FAIL;
        }
        if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
        {
            RELEASE_INSTANCE(CGameInstance);
            return E_FAIL;
        }
        if (FAILED(m_pShaderCom->Set_RawValue("g_CamFar", &camFar, sizeof(_float))))
        {
            RELEASE_INSTANCE(CGameInstance);
            return E_FAIL;
        }
        if (FAILED(m_pShaderCom->Set_RawValue("g_PrevWorldMatrix", &m_PrevWorldMatrix, sizeof(_float4x4))))
        {
            RELEASE_INSTANCE(CGameInstance);
            return E_FAIL;
        }
        if (FAILED(m_pShaderCom->Set_RawValue("g_PrevViewMatrix", &PrevViewMatrix, sizeof(_float4x4))))
        {
            RELEASE_INSTANCE(CGameInstance);
            return E_FAIL;
        }

        m_PrevWorldMatrix = WorldMatrix;
    }
    RELEASE_INSTANCE(CGameInstance);
    return S_OK;

}

HRESULT CNPC::SetUp_Components()
{
    /* if (FAILED(__super::SetUp_Components()))
         return E_FAIL;*/

    if (FAILED(Ready_Transform(COM_TRANSFORM)))
        return E_FAIL;

    /* For.Com_Renderer */
    if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, (CComponent**)&m_pRendererCom)))
        return E_FAIL;

     /* For.Com_Shader */
    if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXANIMMODEL, (CComponent**)&m_pShaderCom)))
        return E_FAIL;
    
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

   

    //=====================================================

    if (g_iCurrentLevel == LEVEL_STORY)
    {
        CModel* pCandidateModel = nullptr;
        pCandidateModel = (CModel*)pGameInstance->Clone_Component(LEVEL_STORY, PROTO_COMP_MODEL_CANDIDATE, (CComponent**)&pCandidateModel);
        m_ModelList.push_back(pCandidateModel);

        CModel* pCandidate2Model = nullptr;
        pCandidate2Model = (CModel*)pGameInstance->Clone_Component(LEVEL_STORY, PROTO_COMP_MODEL_NPC_SIT_CANDIDATE, (CComponent**)&pCandidate2Model);
        m_ModelList.push_back(pCandidate2Model);

        CModel* pCandidate3Model = nullptr;
        pCandidate3Model = (CModel*)pGameInstance->Clone_Component(LEVEL_STORY, PROTO_COMP_MODEL_NPC_DEAD_CANDIDATE, (CComponent**)&pCandidate3Model);
        m_ModelList.push_back(pCandidate3Model);


        CAnimator::ANIMATORDESC tDesc;
        tDesc.pModel = pCandidateModel;
        tDesc.pTransform = m_pTransformCom;
        tDesc.strFilePath = L"NPC_Candidate";
        CAnimator* pCandidateAnimator = nullptr;
        pCandidateAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
        pCandidateAnimator->Set_Model(m_ModelList[NPC_CANDIDATE]);
        m_AnimatorList.push_back(pCandidateAnimator);

        tDesc.pModel = pCandidate2Model;
        tDesc.pTransform = m_pTransformCom;
        tDesc.strFilePath = L"NPC_Candidate2";
        CAnimator* pCandidate2Animator = nullptr;
        pCandidate2Animator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
        pCandidate2Animator->Set_Model(m_ModelList[NPC_SIT_CANDIDATE]);
        m_AnimatorList.push_back(pCandidate2Animator);

        tDesc.pModel = pCandidate3Model;
        tDesc.pTransform = m_pTransformCom;
        tDesc.strFilePath = L"NPC_Candidate3";
        CAnimator* pCandidate3Animator = nullptr;
        pCandidate3Animator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
        pCandidate3Animator->Set_Model(m_ModelList[NPC_DEAD_CANDIDATE]);
        m_AnimatorList.push_back(pCandidate3Animator);
    }
	
    else if (g_iCurrentLevel == LEVEL_RATTAN)
    {
        //모델
        CModel* pAoiModel = nullptr;
        pAoiModel = (CModel*)pGameInstance->Clone_Component(LEVEL_RATTAN, PROTO_COMP_MODEL_AOI, (CComponent**)&pAoiModel);
        m_ModelList.push_back(pAoiModel);
		
        CModel* pSisterLModel = nullptr;
        pSisterLModel = (CModel*)pGameInstance->Clone_Component(LEVEL_RATTAN, PROTO_COMP_MODEL_SISTER_L, (CComponent**)&pSisterLModel);
        m_ModelList.push_back(pSisterLModel);

        CModel* pSisterRModel = nullptr;
        pSisterRModel = (CModel*)pGameInstance->Clone_Component(LEVEL_RATTAN, PROTO_COMP_MODEL_SISTER_R, (CComponent**)&pSisterRModel);
        m_ModelList.push_back(pSisterRModel);

        CModel* pZenitsuModel = nullptr;
        pZenitsuModel = (CModel*)pGameInstance->Clone_Component(LEVEL_RATTAN, PROTO_COMP_MODEL_NPC_ZENITSU, (CComponent**)&pZenitsuModel);
        m_ModelList.push_back(pZenitsuModel);

        CModel* pKanawoModel = nullptr;
        pKanawoModel = (CModel*)pGameInstance->Clone_Component(LEVEL_RATTAN, PROTO_COMP_MODEL_NPC_KANAWO, (CComponent**)&pKanawoModel);
        m_ModelList.push_back(pKanawoModel);

        CModel* pCandidateModel = nullptr;
        pCandidateModel = (CModel*)pGameInstance->Clone_Component(LEVEL_RATTAN, PROTO_COMP_MODEL_CANDIDATE, (CComponent**)&pCandidateModel);
        m_ModelList.push_back(pCandidateModel);

        CModel* pGenyaModel = nullptr;
        pGenyaModel = (CModel*)pGameInstance->Clone_Component(LEVEL_RATTAN, PROTO_COMP_MODEL_NPC_GENYA, (CComponent**)&pCandidateModel);
        m_ModelList.push_back(pGenyaModel);

		//애니메이션
        CAnimator::ANIMATORDESC tDesc;
        tDesc.pModel = pAoiModel;
        tDesc.pTransform = m_pTransformCom;
        tDesc.strFilePath = L"NPC_Aoi";
        CAnimator* pAoiAnimator = nullptr;
        pAoiAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
        pAoiAnimator->Set_Model(m_ModelList[NPC_AOI]);
        m_AnimatorList.push_back(pAoiAnimator);

        tDesc.pModel = pSisterLModel;
        tDesc.pTransform = m_pTransformCom;
        tDesc.strFilePath = L"NPC_SisterL";
        CAnimator* pSisterLAnimator = nullptr;
        pSisterLAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
        pSisterLAnimator->Set_Model(m_ModelList[NPC_SISTER_L]);
        m_AnimatorList.push_back(pSisterLAnimator);

        tDesc.pModel = pSisterRModel;
        tDesc.pTransform = m_pTransformCom;
        tDesc.strFilePath = L"NPC_SisterR";
        CAnimator* pSisterRAnimator = nullptr;
        pSisterRAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
        pSisterRAnimator->Set_Model(m_ModelList[NPC_SISTER_R]);
        m_AnimatorList.push_back(pSisterRAnimator);

        tDesc.pModel = pZenitsuModel;
        tDesc.pTransform = m_pTransformCom;
        tDesc.strFilePath = L"NPC_Zenitsu";
        CAnimator* pZenitsuAnimator = nullptr;
        pZenitsuAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
        pZenitsuAnimator->Set_Model(m_ModelList[NPC_ZENITSU]);
        m_AnimatorList.push_back(pZenitsuAnimator);

        tDesc.pModel = pKanawoModel;
        tDesc.pTransform = m_pTransformCom;
        tDesc.strFilePath = L"NPC_Kanawo";
        CAnimator* pKanawoAnimator = nullptr;
        pKanawoAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
        pKanawoAnimator->Set_Model(m_ModelList[NPC_KANAWO]);
        m_AnimatorList.push_back(pKanawoAnimator);

        tDesc.pModel = pCandidateModel;
        tDesc.pTransform = m_pTransformCom;
        tDesc.strFilePath = L"NPC_RattanCandidate";
        CAnimator* pCandidateAnimator = nullptr;
        pCandidateAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
        pCandidateAnimator->Set_Model(m_ModelList[RATTAN_CANDIDATE]);
        m_AnimatorList.push_back(pCandidateAnimator);

        tDesc.pModel = pGenyaModel;
        tDesc.pTransform = m_pTransformCom;
        tDesc.strFilePath = L"NPC_Genya";
        CAnimator* pGenyaAnimator = nullptr;
        pGenyaAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
        pGenyaAnimator->Set_Model(m_ModelList[NPC_GENYA]);
        m_AnimatorList.push_back(pGenyaAnimator);
		

        //아이템 
        if (m_iType == NPC_SISTER_L)
        {  //아이템
            PATHS Path;
            FILEGROUP* pFolder;
            FILEDATA* pData;
            CSheath::ITEMDESC tItemDesc;

            Path = { L"Model", L"Item", L"Item_LanternL", L"FBX_Converter", L"Data" };
            pFolder = pGameInstance->Find_Folder(&Path);
            pData = pFolder->FindData(L"Item_LanternL");

            CNPC_Item* m_pLanternL = nullptr;

            tItemDesc.pItemData = pData;
            tItemDesc.pTargetModel = m_ModelList[NPC_SISTER_L];
            tItemDesc.pTargetTransform = m_pTransformCom;
            tItemDesc.strModelProtoName = PROTO_COMP_MODEL_LANTERN_L;
            m_pLanternL = static_cast<CNPC_Item*>(pGameInstance->Clone_GameObject_Add(LEVEL_RATTAN, LAYER_UI, PROTO_GOB_NPC_ITEM, &tItemDesc));
            m_ItemList.push_back(m_pLanternL);
            Safe_AddRef(m_pLanternL);
        }
        if (m_iType == NPC_SISTER_R)
        {
            PATHS Path;
            FILEGROUP* pFolder;
            FILEDATA* pData;
            CSheath::ITEMDESC tItemDesc;
			
            Path = { L"Model", L"Item", L"Item_LanternR", L"FBX_Converter", L"Data" };
            pFolder = pGameInstance->Find_Folder(&Path);
            pData = pFolder->FindData(L"Item_LanternR");

            CNPC_Item* m_pLanternR = nullptr;

            tItemDesc.pItemData = pData;
            tItemDesc.pTargetModel = m_ModelList[NPC_SISTER_R];
            tItemDesc.pTargetTransform = m_pTransformCom;
            tItemDesc.strModelProtoName = PROTO_COMP_MODEL_LANTERN_R;
            m_pLanternR = static_cast<CNPC_Item*>(pGameInstance->Clone_GameObject_Add(LEVEL_RATTAN, LAYER_UI, PROTO_GOB_NPC_ITEM, &tItemDesc));
            m_ItemList.push_back(m_pLanternR);
            Safe_AddRef(m_pLanternR);
        }
    }
    RELEASE_INSTANCE(CGameInstance);

    return S_OK;
}

CNPC* CNPC::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
    CNPC* pInstance = new CNPC(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        Safe_Release(pInstance);
#ifdef _DEBUG
        MSG_BOX("CNPC::Create To Failed");
#endif
    }
    return pInstance;
}

CGameObject* CNPC::Clone(void* pArg)
{
    CNPC* pInstance = new CNPC(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
    {
        Safe_Release(pInstance);
#ifdef _DEBUG
        MSG_BOX("CNPC::Clone To Failed");
#endif
    }
    return pInstance;
}

void CNPC::Free()
{
    for (int i = 0; i < m_ModelList.size(); ++i)
    {
        Safe_Release(m_ModelList[i]);
    }
    m_ModelList.clear();

    for (int i = 0; i < m_AnimatorList.size(); ++i)
    {
        Safe_Release(m_AnimatorList[i]);
    }
    m_AnimatorList.clear();

    for (int i = 0; i < m_ItemList.size(); ++i)
    {
        Safe_Release(m_ItemList[i]);
    }
    m_ItemList.clear();

   // Safe_Release(m_pModel);
    Safe_Release(m_pAllAnimModel);

    __super::Free();
}
