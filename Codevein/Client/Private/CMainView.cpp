#include "CMainView.h"
#include "CObjectTool.h"
#include "CMapTool.h"
#include "CCamTool.h"
#include "CEffect_ParticleTool.h"
#include "CNavigationTool.h"
#include "CImGui_Manager.h"
#include "CGameInstance.h"
#include "CLevel_Loading.h"
#include "CGameObject_Client.h"
#include "CData_Manager.h"
#include "CCamera_Tool.h"
#include "CCamera_Free.h"

CMainView::CMainView(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CIMGUI(_pDevice, _pContext)
{
}

HRESULT CMainView::Add_ProtoObjMap(const _tchar* _pTag, CGameObject_Client* _pObject)
{
    if (nullptr == _pObject)
        return E_FAIL;

    // Add
     m_mProtoObjMap.emplace(pair<const _tchar*, CGameObject_Client*>(_pTag, _pObject));
     Safe_AddRef(_pObject);
    return S_OK;
}

HRESULT CMainView::NativeConstruct()
{
    if (FAILED(__super::NativeConstruct()))
        return E_FAIL;

    m_tMainView = DOCKDESC("Main", ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar | 
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar, ImVec2(5.f, 5.f), ImVec2(500.f, 720.f));
    m_tPerspective = DOCKDESC("Perspective", ImGuiWindowFlags_NoFocusOnAppearing);
    m_tHierarchy = DOCKDESC("Hierarchy", ImGuiWindowFlags_NoFocusOnAppearing);
    // Name
    m_pName = "MainView";
    if (FAILED(Ready_ImGuiClass()))
        return E_FAIL;

    m_bOpen = true;

    m_ClonesLayerStr.push_back(LAYER_TERRAIN);
    m_ClonesLayerStr.push_back(LAYER_WORLDOBJ);
    m_ClonesLayerStr.push_back(LAYER_PLAYER);
    m_ClonesLayerStr.push_back(LAYER_WEAPON);
    m_ClonesLayerStr.push_back(LAYER_BOSS);
    m_ClonesLayerStr.push_back(LAYER_MONSTER);
    m_ClonesLayerStr.push_back(LAYER_CHECKPOINTS);
    m_ClonesLayerStr.push_back(LAYER_WORLDEFFECT);
    return S_OK;
}

HRESULT CMainView::NativeConstruct_Tool()
{
    if (FAILED(__super::NativeConstruct_Tool()))
        return E_FAIL;

    for (auto& iter : m_vecImGui)
        if (nullptr != iter)
        {
            if (FAILED(iter->NativeConstruct_Tool()))
                return E_FAIL;
        }
    return S_OK;
}

HRESULT CMainView::Tick(const _double& _dDeltaSeconds)
{
    if (LEVEL_TOOL != g_iCurrentLevel)
        return E_FAIL;

    if (FAILED(__super::Tick(_dDeltaSeconds)))
        return E_FAIL;

    Reset_Value();
    Picking_Clones();

    for (auto& iter : m_vecImGui)
        if (nullptr != iter)
            iter->Tick(_dDeltaSeconds);

    return S_OK;
}

HRESULT CMainView::Render()
{
    if (LEVEL_TOOL != g_iCurrentLevel)
        return E_FAIL;

    if (FAILED(__super::Render()))
        return E_FAIL;

    Show_Window();

    for (auto& iter : m_vecImGui)
        if (nullptr != iter)
                iter->Render();

    return S_OK;
}

HRESULT CMainView::Release_Tool()
{
    return S_OK;
}

HRESULT CMainView::Ready_ImGuiClass()
{
    CIMGUI* pInstance = nullptr;
    // Object Tool
    {
        pInstance = CObjectTool::Create(m_pDevice, m_pContext);
        if (nullptr == pInstance)
            return E_FAIL;

        m_vecImGui.push_back(pInstance);
    }
    // MapTool
    {
        pInstance = CMapTool::Create(m_pDevice, m_pContext);
        if (nullptr == pInstance)
            return E_FAIL;

        m_vecImGui.push_back(pInstance);
    }
    // CamTool
    {
        pInstance = CCamTool::Create(m_pDevice, m_pContext);
        if (nullptr == pInstance)
            return E_FAIL;

        m_vecImGui.push_back(pInstance);
    }
    // Navigation Tool
    {
        pInstance = CNavigationTool::Create(m_pDevice, m_pContext);
        if (nullptr == pInstance)
            return E_FAIL;

        m_vecImGui.push_back(pInstance);
    }
    {
        pInstance = CEffect_ParticleTool::Create(m_pDevice, m_pContext);
        if (nullptr == pInstance)
            return E_FAIL;

        m_vecImGui.push_back(pInstance);
    }
    return S_OK;
}

HRESULT CMainView::Relase_Tool()
{
    for (auto& iter : m_vecImGui)
    {
        iter->Set_Open(false);
        iter->Release_Tool();
    }
    return S_OK;
}

void CMainView::Picking_Clones()
{
    if (m_vecImGui.size() <= 0)
        return;
    if (nullptr == m_vecImGui[GUI_MAP])
        return;
    if (false == m_vecImGui[GUI_MAP]->Is_Open() && false == m_vecImGui[GUI_NAV]->Is_Open())
    {
        if (false == m_bPickingWorldObj)
        {
            // Get SingleTon
            BEGIN_SINGLETON(CGameInstance, pGameInstance);
            {
                if (true == pGameInstance->Get_DIMBtnDown(CInput_Device::DIMB_L))
                {
                    _uint i = 0;
                    for (auto& Name : m_ClonesLayerStr)
                    {
                        for (auto& iter : pGameInstance->Find_GameObjectList(g_iCurrentLevel, Name))
                        {
                            CGameObject_Client* pClientObject = static_cast<CGameObject_Client*>(iter);
                            if (nullptr != pClientObject)
                                pClientObject->Is_Picking();
                        }
                    }

                    BEGIN_SINGLETON(CData_Manager, pDataManager)
                    {
                        CPlayer* pPlayer = pDataManager->Get_PlayerMgr()->Get_Player();
                        if (nullptr != pPlayer)
                            pPlayer->Is_Picking();
                    }
                    END_SINGLETON(pDataManager);

                    CPicking::PICKINGDESC* pPickDesc = pGameInstance->Get_Picking_GOB();
                    if (nullptr != pPickDesc)
                    {
                        m_tSelect.pGameObject = static_cast<CGameObject_Client*>(pPickDesc->pGameObject);
                        m_tSelect.v3Positon = pPickDesc->v3PickingPos;
                        _uint i = 0;
                        for (auto& Name : m_ClonesLayerStr)
                        {
                            for (auto& iter : pGameInstance->Find_GameObjectList(g_iCurrentLevel, Name))
                            {
                                if (m_tSelect.pGameObject == iter)
                                {
                                    _string str = Combine_Int(i, m_tSelect.pGameObject->Get_Name());
                                    m_tSelect.strName = str;
                                    break;
                                }
                                ++i;
                            }
                        }
                    }
                }
            }
            END_SINGLETON(pGameInstance);
        }
        else
        {
            BEGIN_SINGLETON(CGameInstance, pGameInstance);
            BEGIN_SINGLETON(CData_Manager, pDataManager)
            {
                if (true == pGameInstance->Get_DIMBtnDown(CInput_Device::DIMB_L))
                {
                    pDataManager->Get_StageMgr()->Get_Stage()->Is_Picking(m_iRegionIndex);
                    CPicking::PICKINGDESC* pPickDesc = pGameInstance->Get_Picking_GOB();

                    if (nullptr != pPickDesc)
                    {
                        m_tSelect.pGameObject = static_cast<CGameObject_Client*>(pPickDesc->pGameObject);
                        m_tSelect.v3Positon = pPickDesc->v3PickingPos;
                        _uint i = 0;
                        for (auto& Name : m_ClonesLayerStr)
                        {
                            for (auto& iter : pGameInstance->Find_GameObjectList(g_iCurrentLevel, Name))
                            {
                                if (m_tSelect.pGameObject == iter)
                                {
                                    _string str = Combine_Int(i, m_tSelect.pGameObject->Get_Name());
                                    m_tSelect.strName = str;
                                    break;
                                }
                                ++i;
                            }
                        }
                    }
                }
            }
            END_SINGLETON(pDataManager);
            END_SINGLETON(pGameInstance);
        }
    }
}

void CMainView::Show_Window()
{
    ImVec2 imVec2 = ImGui::GetWindowViewport()->Pos;
    ImGui::SetNextWindowPos(ImVec2(imVec2.x + m_tMainView.v2Pos.x, 5.f + imVec2.y + m_tMainView.v2Pos.x), ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(m_tMainView.v2Size, ImGuiCond_Appearing);
    {
        m_tMainView.bEnable = m_bOpen;
        if (false == m_bOpen)
            return;
        ImGui::Begin(m_tMainView.pName, &m_tMainView.bEnable, m_tMainView.imGuiWindowFlags);
        {
            // Setting MenuBar 
            {
                ImGui::BeginMenuBar();
                {
                    // Open ImGuiClass
                    {
                        Open_ImguiClass();
                    }
                    // Scene
                    {
                        Scene_Control();
                    }
                    // Game Control 
                    {
                        Game_Control();
                    }
                    {
                        Play_Button();
                    }
                    {
                        Stop_Button();
                    }
                    // Exit Tool Button
                    {
                        Exit_Button();
                    }
                }
                ImGui::EndMenuBar();
            }
            // MainView Setting (DockBuild Setting)
            {
                m_tMainView.iID = ImGui::GetID(m_tMainView.pName);
                if (false == m_bInitSetting)
                {
                    ImGui::DockBuilderRemoveNode(m_tMainView.iID); // Clear out existing layout
                    ImGui::DockBuilderAddNode(m_tMainView.iID, 0); // Add empty node

                    ImGuiID dock_main_id = m_tMainView.iID; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
                    ImGuiID dock_id_prop = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.5f, NULL, &dock_main_id);
                    ImGuiID dock_id_Hier = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.5f, NULL, &dock_main_id);

                    m_tPerspective.iID = dock_id_prop;
                    m_tHierarchy.iID = dock_id_Hier;
                    // Perspective , Heirecky
                    ImGui::DockBuilderDockWindow(m_tPerspective.pName, dock_id_prop);
                    ImGui::DockBuilderDockWindow(m_tHierarchy.pName, dock_id_Hier);
                    ImGui::DockBuilderFinish(m_tMainView.iID);

                    m_bInitSetting = true;
                }
                ImGui::DockSpace(m_tMainView.iID);
            }
            // Hierachy
            {
                // Hierachy
                ImGui::Begin(m_tHierarchy.pName, &m_tHierarchy.bEnable, m_tHierarchy.imGuiWindowFlags);
                {
                    Hierachy_Control();
                }
                ImGui::End();
            }
            // Properties
            {
                // Properties
                ImGui::Begin(m_tPerspective.pName, &m_tPerspective.bEnable, m_tPerspective.imGuiWindowFlags);
                {
                    Perspective_Control();
                }
                ImGui::End();
            }
        }
        ImGui::End();
    }

}

void CMainView::Open_ImguiClass()
{
    if (ImGui::BeginMenu("Open_ImGuiClass"))
    {
        for (auto& iter : m_vecImGui)
        {
            if (nullptr != iter)
            {
                bool bIsOpen = iter->Is_Open();
                ImGui::MenuItem(iter->Get_Name(), NULL, &bIsOpen);
                iter->Set_Open(bIsOpen);
            }
        }
        ImGui::EndMenu();
    }
}

void CMainView::Game_Control()
{
    if (ImGui::BeginMenu("Game"))
    {
        ImGui::MenuItem("Play", 0, &m_bPlay);
        ImGui::MenuItem("Pause", 0, &m_bPause);
        ImGui::MenuItem("Stop", 0, &m_bStop);
        ImGui::EndMenu();
    }
}

#pragma region SCENE 
void CMainView::Scene_Control()
{
    if (ImGui::BeginMenu("Scene"))
    {
        ImGui::MenuItem("Save", 0, &m_bSave);
        ImGui::MenuItem("Load", 0, &m_bLoad);
        ImGui::EndMenu();
    }

    Save_Scene();
    Load_Scene();
}

void CMainView::Save_Scene()
{
     if (false == m_bSave)
         return;
    ImVec2 imVec2 = ImGui::GetWindowViewport()->Pos;
    ImGui::SetNextWindowPos(ImVec2(imVec2.x + m_tMainView.v2Pos.x, 5.f
        + imVec2.y + m_tMainView.v2Pos.x), ImGuiCond_Appearing);
    {
        ImGui::Begin("Save", &m_bSave, ImGuiWindowFlags_AlwaysAutoResize);
        {
            ImGui::PushItemWidth(200.f);
            ImGui::InputText("Name", m_szSaveName, sizeof(char) * MAX_PATH);
            if (ImGui::Button("Save"))
            {
                BEGIN_SINGLETON(CGameInstance, pGameInstance)
                {
                    for (auto& Name : m_ClonesLayerStr)
                    {
                        for (auto& iter : pGameInstance->Find_GameObjectList(g_iCurrentLevel, Name))
                        {
                            CGameObject_Client* pClientObject = static_cast<CGameObject_Client*>(iter);
                            if (nullptr != pClientObject)
                                pClientObject->Save_Data();
                        }
                    }
                }
                END_SINGLETON(pGameInstance);

                BEGIN_SINGLETON(CData_Manager, pDataManager)
                {
                    // Save Name
                    pDataManager->Save_Data(m_szSaveName);
                }
                END_SINGLETON(pDataManager);
            }
            ImGui::PopItemWidth();
        }
        ImGui::End();
    }
}

void CMainView::Load_Scene()
{
    if (false == m_bLoad)
        return;

    ImVec2 imVec2 = ImGui::GetWindowViewport()->Pos;
    ImGui::SetNextWindowPos(ImVec2(imVec2.x + m_tMainView.v2Pos.x, 5.f
        + imVec2.y + m_tMainView.v2Pos.x), ImGuiCond_Appearing);
    {
        ImGui::Begin("Load", &m_bLoad, ImGuiWindowFlags_AlwaysAutoResize);
        {
            ImGui::PushItemWidth(200.f);
            ImGui::InputText("Name", m_szLoadName, sizeof(char) * MAX_PATH);
            if (ImGui::Button("Load"))
            {
                BEGIN_SINGLETON(CGameInstance, pGameInstacne);
                {
                    BEGIN_SINGLETON(CData_Manager, pDataManager)
                    {
                        pGameInstacne->Release_Pickings();
                        // Save Name
                        pDataManager->Load_Data(m_szLoadName);
                    }
                    END_SINGLETON(pDataManager);
                }
                END_SINGLETON(pGameInstacne);
            }
            ImGui::PopItemWidth();
        }
        ImGui::End();
    }
}
#pragma endregion SCENE

void CMainView::Play_Button()
{
    if (true == m_bGamePlay)
        return;

    if (false == m_bPlay)
        return;

    m_bGamePlay = true;
    m_bPlay = false;

    g_bGameRun = true;

    CPlayer* pPlayer = nullptr;

    BEGIN_SINGLETON(CData_Manager, pDataManager)
    {
        pPlayer = pDataManager->Get_PlayerMgr()->Get_Player();
        {
            CTransform* pTransform = static_cast<CTransform*>(pPlayer->Get_Component(COM_TRANSFORM));
            if (nullptr != pTransform)
                pTransform->Set_Pos(XMLoadFloat3(&pDataManager->Get_PlayerMgr()->Get_PlayerSpawnPos()));
        }
    }
    END_SINGLETON(pDataManager);

    if (nullptr == pPlayer)
        return;

    CTransform* pTransform = static_cast<CTransform*>(pPlayer->Get_Component(COM_TRANSFORM));
    if (nullptr == pTransform)
        return;

    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        {
            CCamera::CAMDESC CameraDesc;
            XMStoreFloat3(&CameraDesc.v3Eye, pTransform->Get_Pos() + XMLoadFloat3(&_float3(0.f, 30.f, -30.f)));
            CameraDesc.v3At = _float3(0.f, 0.f, 0.f);
            XMStoreFloat3(&CameraDesc.v3At, pTransform->Get_Pos());
            CameraDesc.fSpeed = 10.f;
            CameraDesc.fRotSpeed = XMConvertToRadians(90.0f);
            CameraDesc.fFovy = XMConvertToRadians(60.0f);
            CameraDesc.fAspect = _float(g_iWinSizeX) / g_iWinSizeY;
            CameraDesc.fNear = 0.2f;
            CameraDesc.fFar = 1000.f;
            CGameObject* pGameObject = pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, LAYER_CAM, PROTO_GOB_CAMERA_FREE, &CameraDesc);
            pPlayer->Set_CameraObject((CCamera_Free*)pGameObject);
        }
        {
            CCamera_Tool* pCamTool = static_cast<CCamera_Tool*>(pGameInstance->Find_GameObject(LEVEL_TOOL, LAYER_CAM, 0));
            m_tCameraDesc = pCamTool->Get_CamDesc();
            pGameInstance->Add_DeleteObject(LEVEL_TOOL, LAYER_CAM, pCamTool);
        }
    }
    END_SINGLETON(pGameInstance);
}

void CMainView::Stop_Button()
{
    if (false == m_bStop || false == m_bGamePlay)
        return;

    m_bGamePlay = false;
    m_bStop = false;
  
    g_bGameRun = false;

    CPlayer* pPlayer = nullptr;
    BEGIN_SINGLETON(CData_Manager, pDataManager)
    {
        pPlayer = pDataManager->Get_PlayerMgr()->Get_Player();
        {
            CTransform* pTransform = static_cast<CTransform*>(pPlayer->Get_Component(COM_TRANSFORM));
            if (nullptr != pTransform)
                pTransform->Set_Pos(XMLoadFloat3(&pDataManager->Get_PlayerMgr()->Get_PlayerSpawnPos()));
        }
    }
    END_SINGLETON(pDataManager);

    if (nullptr == pPlayer)
        return;
    
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        {
            CCamera_Free* pCamFree = static_cast<CCamera_Free*>(pGameInstance->Find_GameObject(LEVEL_TOOL, LAYER_CAM, 0));
            pGameInstance->Add_DeleteObject(LEVEL_TOOL, LAYER_CAM, pCamFree);
            // Set CameraObject nullptr
            pPlayer->Set_CameraObject(nullptr);

            if (FAILED(pGameInstance->Clone_GameObject(LEVEL_TOOL, LAYER_CAM, PROTO_GOB_CAMERA_TOOL, &m_tCameraDesc)))
            {
                END_SINGLETON(pGameInstance);
                return;
            }
            // Clone Layer Str
            for (auto& Name : m_ClonesLayerStr)
            {
                list<CGameObject*> list = pGameInstance->Find_GameObjectList(g_iCurrentLevel, Name);
                if (list.size() > 0)
                {
                    CGameObject_Client* pClientObject = static_cast<CGameObject_Client*>(list.front());
                }
            }
        }
    }
    END_SINGLETON(pGameInstance);
}

void CMainView::Exit_Button()
{
    if (ImGui::Button("Exit Tool"))
    {
        m_bExit = true;
    }
}

void CMainView::Hierachy_Control()
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance)
    {
        Show_Clones_Update(pGameInstance);
        for (auto& iter : m_ShowCloneMap)
        {
            if (ImGui::TreeNodeEx(iter.first.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                for (auto& value : iter.second)
                {
                    if(ImGui::TreeNodeEx(value.first.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        _uint iIndex = 0;
                        for (auto& Objects : value.second)
                        {
                            if (true == ImGui::Selectable(Objects->Get_ShowName().c_str()))
                            {
                                m_tSelect.pGameObject = Objects;
                                m_tSelect.strName = Objects->Get_ShowName();
                            }
                        }
                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
            }
        }
    }
    END_SINGLETON(pGameInstance);
}


// //fdsfsafsadfsdafsadfsadfdsaf
void CMainView::Perspective_Control()
{
    ImGui::Checkbox("Picking_World_Obj", &m_bPickingWorldObj);
    if (true == m_bPickingWorldObj)
    {
        BEGIN_SINGLETON(CData_Manager, pDataManager)
        {
            ImGui::DragInt("Select_Region", (_int*)&m_iRegionIndex);
            _uint iSize = (_uint)(pDataManager->Get_StageMgr()->Get_Stage()->Get_RegionList().size());
            if (m_iRegionIndex < 0)
                m_iRegionIndex = 0;
            else if (m_iRegionIndex >= iSize)
                m_iRegionIndex = iSize;
        }
        END_SINGLETON(pDataManager);
    }
    if (nullptr == m_tSelect.pGameObject)
        return;

    _bool bActive = m_tSelect.pGameObject->Extern_HierakeyView(m_tSelect.strName);
    BEGIN_SINGLETON(CGameInstance, pGameInstance)
    {
        _bool bKey = pGameInstance->Get_DIKeyDown(DIK_DELETE);
        if (true == bActive || true == bKey)
        {
            pGameInstance->Add_DeleteObject(LEVEL_TOOL, m_tSelect.pGameObject->Get_Layer(), m_tSelect.pGameObject);
            if (m_tSelect.pGameObject == static_cast<CMapTool*>(m_vecImGui[GUI_MAP])->Get_SelectObject())
            {
                static_cast<CMapTool*>(m_vecImGui[GUI_MAP])->Reset();
            }
            m_tSelect.pGameObject = nullptr;
        }
    }
    END_SINGLETON(pGameInstance);
}

void CMainView::Show_Clones_Update(CGameInstance* _pGameInstance)
{
    if (false == g_bGameRun)
    {
        _uint iCheck = 0;
        for (auto& Name : m_ClonesLayerStr)
        {
            list<CGameObject*> List = _pGameInstance->Find_GameObjectList(g_iCurrentLevel, Name);
            iCheck += (_uint)List.size();
        }

        if (iCheck != m_iShowSize)
        {
            m_ShowCloneMap.clear();
            m_iShowSize = (_uint)iCheck;
            for (auto& Name : m_ClonesLayerStr)
            {
                list<CGameObject*> List = _pGameInstance->Find_GameObjectList(g_iCurrentLevel, Name);
                if (List.size() > 0)
                {
                    map<_string, list<CGameObject_Client*>> ClientObj;
                    _uint iIndex = 0;
                    for (auto& iter : List)
                    {
                        CGameObject_Client* pClientObject = static_cast<CGameObject_Client*>(iter);
                        if (nullptr != pClientObject)
                        {
                            _bool bIsTrue = false;
                            for (auto& value : ClientObj)
                            {
                                if (!strcmp(value.first.c_str(), pClientObject->Get_Name()))
                                {
                                    value.second.push_back(pClientObject);
                                    bIsTrue = true;
                                    break;
                                }
                            }
                            if (false == bIsTrue)
                            {
                                list<CGameObject_Client*> pGameObjectList;
                                pGameObjectList.push_back(pClientObject);
                                ClientObj.emplace(pClientObject->Get_Name(), pGameObjectList);
                            }

                            pClientObject->Set_ShowName(Combine_Int(iIndex++, pClientObject->Get_Name()));
                        }
                    }
                    _wstring str = Name;
                    string tag;
                    tag.assign(str.begin(), str.end());
                    m_ShowCloneMap.emplace(tag, ClientObj);
                }
            }
        }
    }

}

void CMainView::Reset_Value()
{
    if (true == m_bExit)
    {
        m_tSelect.pGameObject = nullptr;
        BEGIN_SINGLETON(CData_Manager, pDataManager);
        {
            // Delete WorldManager
            pDataManager->Get_PlayerMgr()->Release_Player();

            for (auto& iter : m_vecImGui)
            {
                iter->Set_Open(false);
                iter->Release_Tool();
            }

            BEGIN_SINGLETON(CGameInstance, pGameInstance);
            {
                pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_LOGO));
            }
            END_SINGLETON(pGameInstance);

            pDataManager->Get_WorldMgr()->Release_Data();
            /* Reset True Values */
            m_bOpen = m_tMainView.bEnable = false;

            m_bExit = false;
            m_bSave = false;
            m_bLoad = false;

            m_ShowCloneMap.clear();

        }
        END_SINGLETON(pDataManager);
    }
}

string CMainView::Combine_Int(const _uint& _iIndex, const string& strName)
{
    _string str = strName;
    char szChange[10];
    _itoa_s(_iIndex, szChange, sizeof(char) * 10);
    str.append(" (");
    str.append(szChange);
    str.append(" )");
    return str;
}

CMainView* CMainView::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CMainView* pInstance = new CMainView(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CMainView::Create To Failed");
    }
    return pInstance;
}

void CMainView::Free()
{
    /* Delete ImGui */
    for (auto& iter : m_vecImGui)
        Safe_Release(iter);
    /* Proto Obj Map */
    for (auto& iter : m_mProtoObjMap)
        Safe_Release(iter.second);

    __super::Free();
}
