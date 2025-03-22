#include "CObjectView.h"
#include "GameInstance.h"
#include "GameObject.h"

CObjectView::CObjectView(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CImGuiClass(_pDevice, _pContext)
{
}

HRESULT CObjectView::NativeConstruct()
{
    m_tMainView = DOCKDESC("ObjectView", ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar, ImVec2(200.f, 1.f), ImVec2(500.f, 720.f), true);
    m_tPerspective = DOCKDESC("PerspectiveShow", ImGuiWindowFlags_NoFocusOnAppearing);
    m_tHierarchy = DOCKDESC("HierarchySelect", ImGuiWindowFlags_NoFocusOnAppearing);

    m_pName = "ObjectView";
    return S_OK;
}

HRESULT CObjectView::NativeConstruct_InnerValue()
{
    return S_OK;
}

HRESULT CObjectView::Tick(const _double& _dDeltaSeconds)
{
    if (FAILED(__super::Tick(_dDeltaSeconds)))
        return E_FAIL;
    return S_OK;
}

HRESULT CObjectView::Late_Tick(const _double& _dDeltaSeconds)
{
    if (FAILED(__super::Late_Tick(_dDeltaSeconds)))
        return E_FAIL;

    Show_Window();
    return S_OK;
}

HRESULT CObjectView::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;
    return S_OK;
}

HRESULT CObjectView::Release_InnerValue()
{
    return S_OK;
}

void CObjectView::Show_Window()
{
    ImVec2 imVec2 = ImVec2(static_cast<_float>(g_iWinSizeX - 180.f), 10.f);
    ImGui::SetNextWindowPos(ImVec2(imVec2.x + m_tMainView.v2Pos.x, 5.f + imVec2.y + m_tMainView.v2Pos.y), ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(m_tMainView.v2Size, ImGuiCond_Appearing);
    {
        m_bOpen = m_tMainView.bEnable;
        ImGui::Begin(m_tMainView.strName.c_str(), &m_tMainView.bEnable, m_tMainView.imGuiWindowFlags);
        {
            // MainView Setting (DockBuild Setting)
            {
                m_tMainView.iID = ImGui::GetID(m_tMainView.strName.c_str());
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
                    ImGui::DockBuilderDockWindow(m_tPerspective.strName.c_str(), dock_id_prop);
                    ImGui::DockBuilderDockWindow(m_tHierarchy.strName.c_str(), dock_id_Hier);
                    ImGui::DockBuilderFinish(m_tMainView.iID);

                    m_bInitSetting = true;
                }
                ImGui::DockSpace(m_tMainView.iID);
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
        }
        ImGui::End();
    }
}

void CObjectView::Hierachy_Control()
{
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        Show_Clones_Update(pGameInstance);
        for (auto& iter : m_ShowCloneMap)
        {
            if (ImGui::TreeNode(iter.first.c_str()))
            {
                for (auto& value : iter.second)
                {
                    if (ImGui::TreeNode(value.first.c_str()))
                    {
                        for (auto& Objects : value.second)
                        {
                            if (true == ImGui::Selectable(Objects.strName.c_str()))
                            {
                                m_tSelectDesc = Objects;
                            }
                        }
                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
            }
        }
    }
    RELEASE_INSTANCE(CGameInstance);
}

void CObjectView::Perspective_Control()
{
    if (nullptr != m_tSelectDesc.pGameObject)
    {
        if (ImGui::TreeNodeEx(m_tSelectDesc.strName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
        {
            m_tSelectDesc.pGameObject->Extern_ImGuiClass();
            if (false == m_tSelectDesc.pGameObject->Is_Active())
                m_tSelectDesc.pGameObject = nullptr;
            ImGui::TreePop();
        }
    }
}

void CObjectView::Show_Clones_Update(CGameInstance* _pGameInstance)
{

    if (m_ClonesLayerStr.size() != _pGameInstance->Cur_LayerSize(g_iCurrentLevel))
    {
        m_ClonesLayerStr.clear();
        _pGameInstance->Out_LayerTags(g_iCurrentLevel, &m_ClonesLayerStr);
    }

    _uint iCheck = 0;
    for (auto& Name : m_ClonesLayerStr)
    {
        list<CGameObject*> List = _pGameInstance->Find_GameObjectList(g_iCurrentLevel, Name);
        iCheck += (_uint)List.size();
    }

    if (iCheck <= 0)
        return;

    if (iCheck != m_iShowSize)
    {
        m_ShowCloneMap.clear();
        m_iShowSize = (_uint)iCheck;
        for (auto& Name : m_ClonesLayerStr)
        {
            list<CGameObject*> List = _pGameInstance->Find_GameObjectList(g_iCurrentLevel, Name);
            if (List.size() > 0)
            {
                map<_string, list<SELECTDESC>> TooObj;
                _uint iIndex = 0;
                for (auto& iter : List)
                {
                    CGameObject* pObject = static_cast<CGameObject*>(iter);
                    if (nullptr != pObject)
                    {
                        _string TypeID = typeid(*iter).name();
                        size_t Find = TypeID.find("::");
                        TypeID = TypeID.substr(Find + 2, TypeID.length());
                        _bool bIsTrue = false;
                        for (auto& value : TooObj)
                        {
                            if (!strcmp(value.first.c_str(), TypeID.c_str()))
                            {
                                SELECTDESC tDesc;
                                tDesc.pGameObject = pObject;
                                tDesc.strName = CMethod::Combine_Int(iIndex, TypeID.c_str());
                                value.second.push_back(tDesc);
                                bIsTrue = true;
                                break;
                            }
                        }
                        if (false == bIsTrue)
                        {
                            list<SELECTDESC> pGameObjectList;
                            SELECTDESC tDesc;
                            tDesc.pGameObject = pObject;
                            tDesc.strName = CMethod::Combine_Int(iIndex, TypeID.c_str());
                            pGameObjectList.push_back(tDesc);
                            TooObj.emplace(TypeID.c_str(), pGameObjectList);
                        }
                    }

                    ++iIndex;
                }
                string tag = CMethod::ConvertWToS(Name);
                m_ShowCloneMap.emplace(tag, TooObj);
            }
        }
    }
}

CObjectView* CObjectView::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CObjectView* pInstance = new CObjectView(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct()))
    {
        Safe_Release(pInstance);
#ifdef _DEBUG
        MSG_BOX("CObjectView::Create To Failed");
#endif
    }
    return pInstance;
}

void CObjectView::Free()
{
    __super::Free();
}
