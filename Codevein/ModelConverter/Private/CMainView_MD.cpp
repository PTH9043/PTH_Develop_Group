#include"imgui.h"
#include"imgui_internal.h"
#include"imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "CMainView_MD.h"
#include "CData_Manager_MD.h"
#include "CGameInstance.h"
#include "CShowObject.h"
#include "CAnimObject.h"
#include "CModel_FBX.h"

CMainView_MD::CMainView_MD(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CIMGUI_MD(_pDevice, _pContext)
{
}

HRESULT CMainView_MD::NativeConstruct()
{
    if (FAILED(__super::NativeConstruct()))
        return E_FAIL;

    if (FAILED(Ready_ImGuiClass()))
        return E_FAIL;


    m_tMainView = DOCKDESC("Main", ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar, _float2(5.f, 5.f), _float2(500.f, 720.f));
    // Name
    m_pName = "MainView";
    if (FAILED(Ready_ImGuiClass()))
        return E_FAIL;

    m_bOpen = true;

    BEGIN_SINGLETON(CGameInstance, pGameInstnace)
    {
        m_pDebugObject = pGameInstnace->Clone_GameObject_Add(LEVEL_STATIC, LAYER_DEBUG, PROTO_GOB_SHOWOBJECT);
        Safe_AddRef(m_pDebugObject);
        
        m_pAnimDebugObject = pGameInstnace->Clone_GameObject_Add(LEVEL_STATIC, LAYER_DEBUG, PROTO_GOB_ANIMOBJECT);
        Safe_AddRef(m_pAnimDebugObject);
    }
    END_SINGLETON(pGameInstnace);
    return S_OK;
}

HRESULT CMainView_MD::Tick(const _double& _dDeltaSeconds)
{
    if (FAILED(__super::Tick(_dDeltaSeconds)))
        return E_FAIL;
    return S_OK;
}

HRESULT CMainView_MD::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    Show_Window();
    return S_OK;
}

void CMainView_MD::Set_DebugObject_Model(CModel_FBX* _pModel)
{
    if (nullptr == _pModel)
        return;

    static_cast<CShowObject*>(m_pDebugObject)->Set_Model_FBX(_pModel);
    static_cast<CShowObject*>(m_pDebugObject)->Set_Active(true);
    static_cast<CAnimObject*>(m_pAnimDebugObject)->Set_Active(false);
}

void CMainView_MD::Set_DebugObject_Model(CModel* _pModel)
{
    if (nullptr == _pModel)
        return;

    static_cast<CShowObject*>(m_pDebugObject)->Set_Model(_pModel);
    static_cast<CShowObject*>(m_pDebugObject)->Set_Active(true);
    static_cast<CAnimObject*>(m_pAnimDebugObject)->Set_Active(false);
}

void CMainView_MD::Set_AnimDebugObject_Model(CModel_FBX* _pModel)
{
    if (nullptr == _pModel)
        return;

    static_cast<CAnimObject*>(m_pAnimDebugObject)->Set_Model_FBX(_pModel);
    static_cast<CAnimObject*>(m_pAnimDebugObject)->Set_Active(true);
    static_cast<CShowObject*>(m_pDebugObject)->Set_Active(false);
}

void CMainView_MD::Set_AnimDebugObject_Model(CModel* _pModel)
{
    if (nullptr == _pModel)
        return;

    static_cast<CAnimObject*>(m_pAnimDebugObject)->Set_Model(_pModel);
    static_cast<CAnimObject*>(m_pAnimDebugObject)->Set_Active(true);
    static_cast<CShowObject*>(m_pDebugObject)->Set_Active(false);
}

void CMainView_MD::Show_Window()
{
    ImVec2 imVec2 = ImGui::GetWindowViewport()->Pos;
    ImVec2 imVecSize = ImVec2(m_tMainView.v2Size.x, m_tMainView.v2Size.y);
    ImGui::SetNextWindowPos(ImVec2(imVec2.x + m_tMainView.v2Pos.x, 5.f + imVec2.y + m_tMainView.v2Pos.x), ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(imVecSize, ImGuiCond_Appearing);
    {
        m_tMainView.bEnable = m_bOpen;
        ImGui::Begin(m_tMainView.pName, &m_tMainView.bEnable, m_tMainView.imGuiWindowFlags);
        {
            if (ImGui::CollapsingHeader("Anim Character"))
            {
                if (nullptr != m_pModelFBX && nullptr == m_pModel)
                {
                    ImGui::InputInt("AnimIndex", (int*)&m_iAnimationIndexFBX);
                    if (m_iAnimationIndexFBX >= m_pModelFBX->Get_AnimationCount())
                        m_iAnimationIndexFBX = m_pModelFBX->Get_AnimationCount();
                    else if (m_iAnimationIndexFBX < 0)
                        m_iAnimationIndexFBX = 0;

                    m_pModelFBX->Set_Animation(m_iAnimationIndexFBX);
                }
                else if (nullptr != m_pModel)
                {
                    ImGui::InputInt("AnimIndex", (int*)&m_iAnimationIndex);
                    if (m_iAnimationIndex >= (_uint)m_pModel->Get_Animations().size())
                        m_iAnimationIndex = (_uint)m_pModel->Get_Animations().size();
                    else if (m_iAnimationIndex < 0)
                        m_iAnimationIndex = 0;

                    m_pModel->Set_Animation(m_iAnimationIndex);
                }
            }

            if (ImGui::CollapsingHeader("Button"))
            {
                if (ImGui::Button("Convert"))
                {
                    BEGIN_SINGLETON(CData_Manager_MD, pDataManager);
                    {
                        pDataManager->Save_Data_NonAnim();
                    }
                    END_SINGLETON(pDataManager);
                }

                if (ImGui::Button("Convert_Anim"))
                {
                    BEGIN_SINGLETON(CData_Manager_MD, pDataManager);
                    {
                        pDataManager->Save_Data_Anim();
                    }
                    END_SINGLETON(pDataManager);
                }

                if (ImGui::Button("Load Data Exterminate"))
                {
                    BEGIN_SINGLETON(CData_Manager_MD, pDataManager);
                    {
                        pDataManager->Load_Data_NonAnim();
                    }
                    END_SINGLETON(pDataManager);
                }

                if (ImGui::Button("Load Anim Data Exterminate"))
                {
                    BEGIN_SINGLETON(CData_Manager_MD, pDataManager);
                    {
                        pDataManager->Load_Data_Anim();
                    }
                    END_SINGLETON(pDataManager);
                }

                if (ImGui::Button("Save Animations"))
                {
                    if (nullptr != m_pModel)
                    {
                        m_pModel->Save_Animation(m_pModel->Get_ModelFileGroup());
                    }
                }
            }

            if (ImGui::CollapsingHeader("Model_FBX", ImGuiTreeNodeFlags_Selected))
            {
                BEGIN_SINGLETON(CData_Manager_MD, pDataManager);
                {
                    for (auto& iter : pDataManager->Get_ModelList())
                    {
                        if (nullptr != iter)
                        {
                            FILEDATA* pFileData = iter->Get_FileData();
                            if (nullptr != pFileData)
                            {
                                string str;
                                str.assign(pFileData->strFileName.begin(), pFileData->strFileName.end());
                                if (ImGui::Selectable(str.c_str()))
                                {
                                    // Set Debug Model
                                    Set_DebugObject_Model(iter);
                                }
                            }
                        }
                    }
                }
                END_SINGLETON(pDataManager);
            }

            if (ImGui::CollapsingHeader("AmimModel_FBX", ImGuiTreeNodeFlags_Selected))
            {
                BEGIN_SINGLETON(CData_Manager_MD, pDataManager);
                {
                    for (auto& iter : pDataManager->Get_AnimModelList())
                    {
                        FILEDATA* pFileData = iter->Get_FileData();
                        if (nullptr != pFileData)
                        {
                            string str;
                            str.assign(pFileData->strFileName.begin(), pFileData->strFileName.end());
                            if (ImGui::Selectable(str.c_str()))
                            {
                                // Set Debug Model
                                Set_AnimDebugObject_Model(iter);
                                m_pModelFBX = iter;
                            }
                        }
                    }
                }
                END_SINGLETON(pDataManager);
            }

            if (ImGui::CollapsingHeader("Model_REAL", ImGuiTreeNodeFlags_Selected))
            {
                BEGIN_SINGLETON(CData_Manager_MD, pDataManager);
                {
                    for (auto& iter : pDataManager->Get_ShowModelList())
                    {
                        FILEDATA* pFileData = iter->Get_FileData();
                        if (nullptr != pFileData)
                        {
                            string str;
                            str.assign(pFileData->strFileName.begin(), pFileData->strFileName.end());
                            if (ImGui::Selectable(str.c_str()))
                            {
                                Set_DebugObject_Model(iter);
                            }
                        }
                    }
                }
                END_SINGLETON(pDataManager);
            }

            if (ImGui::CollapsingHeader("AmimModel_Real", ImGuiTreeNodeFlags_Selected))
            {
                BEGIN_SINGLETON(CData_Manager_MD, pDataManager);
                {
                    for (auto& iter : pDataManager->Get_ShowAnimModelList())
                    {
                        FILEDATA* pFileData = iter->Get_FileData();
                        if (nullptr != pFileData)
                        {
                            string str;
                            str.assign(pFileData->strFileName.begin(), pFileData->strFileName.end());
                            if (ImGui::Selectable(str.c_str()))
                            {
                                // Set Debug Model
                                Set_AnimDebugObject_Model(iter);
                                m_pModel = iter;
                            }
                        }
                    }
                }
                END_SINGLETON(pDataManager);
            }
        }
        ImGui::End();
    }
}

HRESULT CMainView_MD::Ready_ImGuiClass()
{
    return S_OK;
}

CMainView_MD* CMainView_MD::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CMainView_MD* pInstance = new CMainView_MD(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CMainView_MD::Create To Failed");
    }
    return pInstance;
}

void CMainView_MD::Free()
{
    Safe_Release(m_pDebugObject);
    Safe_Release(m_pAnimDebugObject);
    __super::Free();
}
