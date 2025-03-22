#include "CMainView.h"
#include "CCamTool.h"
#include "CImGui_Manager.h"
#include "CGameInstance.h"
#include "CGameObject.h"
#include "CCamera_Tool.h"

CCamTool::CCamTool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CIMGUI(_pDevice, _pContext)
{
}

HRESULT CCamTool::NativeConstruct()
{
    if (FAILED(__super::NativeConstruct()))
        return E_FAIL;

    m_pName = "CamTool";
    return S_OK;
}

HRESULT CCamTool::NativeConstruct_Tool()
{
    if (FAILED(__super::NativeConstruct_Tool()))
        return E_FAIL;

    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        m_pCamera = static_cast<CCamera_Tool*>(pGameInstance->Find_GameObject(g_iCurrentLevel, LAYER_CAM, 0));
        Safe_AddRef(m_pCamera);
    }
    END_SINGLETON(pGameInstance);

    if (nullptr == m_pCamera)
    {
        MSG_BOX("CamTool::NativeConstruct_Tool -> Find Cam Failed");
        return E_FAIL;
    }

    m_bOpen = false;
    return S_OK;
}

HRESULT CCamTool::Tick(const _double& _dDeltaSeconds)
{
    if (FAILED(__super::Tick(_dDeltaSeconds)))
        return E_FAIL;

    return S_OK;
}

HRESULT CCamTool::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    Show_Window();
    return S_OK;
}

HRESULT CCamTool::Release_Tool()
{
    Safe_Release(m_pCamera);
    return S_OK;
}

void CCamTool::Show_Window()
{
    ImVec2 imVec2 = ImGui::GetWindowViewport()->Pos;
    ImGui::SetNextWindowPos(ImVec2(imVec2.x, 360.f + imVec2.y), ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(ImVec2(100.f, 100.f), ImGuiCond_Appearing);
    ImGui::Begin(m_pName, &m_bOpen);
    {
        Control_Camera();
    }
    ImGui::End();
}

void CCamTool::Control_Camera()
{
    if (nullptr == m_pCamera)
        return;

    m_pCamera->Show_CamControlData();
}

CCamTool* CCamTool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CCamTool* pInstance = new CCamTool(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CCamTool::Create To Failed");
    }
    return pInstance;
}

void CCamTool::Free()
{
    Safe_Release(m_pCamera);
    __super::Free();
}
