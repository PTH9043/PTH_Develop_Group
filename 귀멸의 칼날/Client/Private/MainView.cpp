#include "MainView.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "ImGuiClass_Manager.h"
#include "ShaderView.h"
#include "CObjectView.h"
#include "TimeControlViewer.h"
#include "Level_Loading.h"
#include "AIConViewer.h"

CMainView::CMainView(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CImGuiClass(_pDevice, _pContext)
{
}

HRESULT CMainView::NativeConstruct()
{
    m_bOpen = true;
    m_pName = "MainView";

    m_tMainView = DOCKDESC("Main", ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar, ImVec2(1.f, 1.f), ImVec2(500.f, 720.f), true);
    m_tPerspective = DOCKDESC("Select", ImGuiWindowFlags_NoFocusOnAppearing);
    m_tHierarchy = DOCKDESC("Console", ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);

    CDebug_Manager* pInstance = GET_INSTANCE(CDebug_Manager);
    {
        m_pRefDebugActor = static_cast<CRefDebugActor*>(pInstance->Get_DebugActor(CDebug_Manager::DEBUG_REFACTOR));
        Safe_AddRef(m_pRefDebugActor);
    }
    RELEASE_INSTANCE(CDebug_Manager);

    if (FAILED(Ready_ImGuiClass()))
        return E_FAIL;

    m_bOpen = true;

    m_OpenLevelMap.insert(pair<_uint, _string>(LEVEL_STATIC, "LEVEL_STATIC"));
    m_OpenLevelMap.insert(pair<_uint, _string>(LEVEL_LOGO, "LEVEL_LOGO"));
    m_OpenLevelMap.insert(pair<_uint, _string>(LEVEL_MODE, "LEVEL_MODE"));
    m_OpenLevelMap.insert(pair<_uint, _string>(LEVEL_SELECT, "LEVEL_SELECT"));
    m_OpenLevelMap.insert(pair<_uint, _string>(LEVEL_BATTLE, "LEVEL_BATTLE"));
    m_OpenLevelMap.insert(pair<_uint, _string>(LEVEL_STORY, "LEVEL_STORY"));
    m_OpenLevelMap.insert(pair<_uint, _string>(LEVEL_MINIGAME, "LEVEL_MINIGAME"));
    m_OpenLevelMap.insert(pair<_uint, _string>(LEVEL_RATTAN, "LEVEL_RATTAN"));
    m_OpenLevelMap.insert(pair<_uint, _string>(LEVEL_ENDING, "LEVEL_ENDING"));
    m_OpenLevelMap.insert(pair<_uint, _string>(LEVEL_GAMEPLAY, "LEVEL_GAMEPLAY"));

	return S_OK;
}

HRESULT CMainView::NativeConstruct_InnerValue()
{
	return S_OK;
}

HRESULT CMainView::Tick(const _double& _dTimeDelta)
{
#ifdef _DEBUG
    if (true == m_bRestrictFrame)
    {
        Receive_CameraObject();
        for (auto& iter : m_CameraObjectList)
            if (nullptr != iter)
                iter->Tick(_dTimeDelta);
    }
#endif
    
    for (auto& iter : m_ImGuiList)
        iter->Tick(_dTimeDelta);

    OpenLevel();
	return S_OK;
}

HRESULT CMainView::Late_Tick(const _double& _dTimeDelta)
{
    for (auto& iter : m_ImGuiList)
    {
        iter->Late_Tick(_dTimeDelta);
    }

    Show_Window();
    return S_OK;
}

HRESULT CMainView::Release_InnerValue()
{
	return S_OK;
}

void CMainView::Add_Explain(const _string& _strExplain)
{
    if (m_AddExplain.size() >= 5)
        m_AddExplain.clear();

    m_AddExplain.push_back(_strExplain);
}


HRESULT CMainView::Ready_ImGuiClass()
{
   CImGuiClass_Manager* pInstance = GET_INSTANCE(CImGuiClass_Manager);
   {
       {
           CImGuiClass* pImGuiClass = CShaderView::Create(m_pDevice, m_pContext);
           pInstance->Insert_ImGuiClass(IMGCLASS_SHADERVIEW, pImGuiClass);
           m_ImGuiList.push_back(pImGuiClass);
           Safe_AddRef(pImGuiClass);
       }
       {
           CImGuiClass* pImGuiClass = CObjectView::Create(m_pDevice, m_pContext);
           pInstance->Insert_ImGuiClass(IMGCLASS_OBJVIEW, pImGuiClass);
           m_ImGuiList.push_back(pImGuiClass);
           Safe_AddRef(pImGuiClass);
       }
       {
           CImGuiClass* pImGuiClass = CTimerControlViewer::Create(m_pDevice, m_pContext);
           pInstance->Insert_ImGuiClass(IMGCLASS_LEVELCON, pImGuiClass);
           m_ImGuiList.push_back(pImGuiClass);
           Safe_AddRef(pImGuiClass);
       }
       {
           CImGuiClass* pImGuiClass = CAIConViewer::Create(m_pDevice, m_pContext);
           pInstance->Insert_ImGuiClass(IMGUI_AICONTROLLER, pImGuiClass);
           m_ImGuiList.push_back(pImGuiClass);
           Safe_AddRef(pImGuiClass);
       }
   }
	RELEASE_INSTANCE(CImGuiClass_Manager);
	return S_OK;
}

void CMainView::Picking_Clones()
{
    return;
}

void CMainView::Show_Window()
{
    ImVec2 imVec2 = ImVec2(static_cast<_float>(g_iWinSizeX - 180.f), 10.f);
    ImGui::SetNextWindowPos(ImVec2(imVec2.x + m_tMainView.v2Pos.x, 5.f + imVec2.y + m_tMainView.v2Pos.y), ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(m_tMainView.v2Size, ImGuiCond_Appearing);
    {
        m_tMainView.bEnable = m_bOpen;
        if (false == m_bOpen)
            return;
        ImGui::Begin(m_tMainView.strName.c_str(), &m_tMainView.bEnable, m_tMainView.imGuiWindowFlags);
        {
            // Basic info
            ImGuiContext& g = *GImGui;
            ImGuiIO& io = g.IO;
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "");
            ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), u8"N : 몬스터 생성");
            ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), u8"J : 플레이어 공격");
            ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), u8"C : 콜라이더 랜더");

            // Setting MenuBar 
            {
                ImGui::BeginMenuBar();
                {
                    // Open ImGuiClass
                    {
                        Open_ImguiClass();
                    }
                }
                ImGui::EndMenuBar();
            }
            // MainView Setting (DockBuild Setting)
            {
                m_tMainView.iID = ImGui::GetID(m_tMainView.strName.c_str());
                if (false == m_bInitSetting)
                {
                    ImGui::DockBuilderRemoveNode(m_tMainView.iID); // Clear out existing layout
                    ImGui::DockBuilderAddNode(m_tMainView.iID, 0); // Add empty node

                    ImGuiID dock_main_id = m_tMainView.iID; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
                    ImGuiID dock_id_prop = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.3f, NULL, &dock_main_id);
                    ImGuiID dock_id_Hier = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.7f, NULL, &dock_main_id);

                    m_tPerspective.iID = dock_id_prop;
                    m_tHierarchy.iID = dock_id_Hier;
                    // Perspective , Heirecky
                    ImGui::DockBuilderDockWindow(m_tPerspective.strName.c_str(), dock_id_prop);
                    ImGui::DockBuilderDockWindow(m_tHierarchy.strName.c_str(), dock_id_Hier);
                    ImGui::DockBuilderFinish(m_tMainView.iID);

                    m_bInitSetting = true;
                }
                ImGui::DockSpace(m_tMainView.iID);
            }
            // Hierachy
            {
                // Hierachy
                ImGui::Begin(m_tHierarchy.strName.c_str(), &m_tHierarchy.bEnable, m_tHierarchy.imGuiWindowFlags);
                {
                    Hierachy_Control();
                }
                ImGui::End();
            }
            // Properties
            {
                // Properties
                ImGui::Begin(m_tPerspective.strName.c_str(), &m_tPerspective.bEnable, m_tPerspective.imGuiWindowFlags);
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
        for (auto& iter : m_ImGuiList)
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

void CMainView::Hierachy_Control()
{
#ifdef _DEBUG
    ImGui::Checkbox("RestrictFrame", &m_bRestrictFrame);
    CImGuiClass_Manager* pImGuiManager = GET_INSTANCE(CImGuiClass_Manager);
    {
        if (ImGui::Button("Play Frame"))
        {
            _bool* pStopFrame = pImGuiManager->Is_StopFrame();
            if (nullptr != pStopFrame)
                *pStopFrame = false;
        }
        else
        {
            _bool* pStopFrame = pImGuiManager->Is_StopFrame();
            if (nullptr != pStopFrame)
                *pStopFrame = m_bRestrictFrame;
        }
    }
    RELEASE_INSTANCE(CImGuiClass_Manager);
#endif
    if (ImGui::BeginListBox("##listbox 3", ImVec2(-FLT_MIN, 8 * ImGui::GetTextLineHeightWithSpacing())))
    {
        for (auto& iter : m_OpenLevelMap)
        {
            if (ImGui::Selectable(iter.second.c_str()))
            {
                m_iSelectLevel = iter.first;
                m_bSelectClickLevel = false;
                m_bOpenLevel = true;
            }
        }
        ImGui::EndListBox();
    }
}

void CMainView::Perspective_Control()
{
    if (ImGui::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, 8 * ImGui::GetTextLineHeightWithSpacing())))
    {
        if (0 == m_iSelectComboBox)
        {
            for (auto& iter : m_AddExplain)
            {
                ImGui::TextWrapped(iter.c_str());
            }
        }
        ImGui::EndListBox();
    }
}

void CMainView::Show_Clones_Update(CGameInstance* _pGameInstance)
{
}

void CMainView::OpenLevel()
{
    if (false == m_bOpenLevel)
        return;

    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    if (FAILED(CGameInstance::Get_Instance()->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext,
        (LEVEL)m_iSelectLevel))))
        return;

    Safe_Release(pGameInstance);

    m_bOpenLevel = false;
}
#ifdef _DEBUG
void CMainView::Receive_CameraObject()
{
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        m_CameraObjectList.clear();
        m_CameraObjectList =  pGameInstance->Find_GameObjectList(g_iCurrentLevel, LAYER_CAM);
    }
    RELEASE_INSTANCE(CGameInstance);
}
#endif
CMainView* CMainView::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CMainView* pInstance = new CMainView(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct()))
	{
		Safe_Release(pInstance);
		MSG_BOX("MainView_Tool::Create To Failed");
	}
	return pInstance;
}

void CMainView::Free()
{
    for (auto& iter : m_ImGuiList)
        Safe_Release(iter);
  
    Safe_Release(m_pRefDebugActor);
	__super::Free();
}
