#include "ToolDefines.h"
#include "TMainView.h"
#include "UGameInstance.h"
#include "UTexture.h"
#include "TMainScene.h"
#include "UGrid.h"
#include "UTransform.h"

TMainView::TMainView(CSHPTRREF<UDevice> _spDevice) :
    TImGuiView(_spDevice, "MainView"),
    m_stMainDesc{},
    m_stDebuggingView{},
    m_OpenImGuies{},
    m_dShowDeltaTime{ 0.0 },
    m_isDockBuilding{ false },
    m_bGridActive{true},
    m_bFogActive{true}
{
}

void TMainView::Free()
{

}

HRESULT TMainView::NativeConstruct()
{
    m_stMainDesc = MAINDESC(
        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus
        | ImGuiWindowFlags_NoBackground
        , ImGuiDockNodeFlags_PassthruCentralNode);
    return S_OK;
}

void TMainView::InsertImGuiView(const CSHPTRREF<TImGuiView> _spImGuiView)
{
    RETURN_CHECK(nullptr == _spImGuiView, ;);
    m_CloseImGuies.push_back(_spImGuiView);
}

HRESULT TMainView::LoadResource()
{
    return S_OK;
}

HRESULT TMainView::ReleaseResource()
{
    return S_OK;
}

void TMainView::TickActive(const _double& _dTimeDelta)
{
    m_dShowDeltaTime = _dTimeDelta;
    for (auto& iter : m_OpenImGuies)
    {
        iter->Tick(_dTimeDelta);
    }
}

void TMainView::LateTickActive(const _double& _dTimeDetla)
{
    for (auto& iter : m_OpenImGuies)
    {
        iter->LateTick(_dTimeDetla);
    }
}

void TMainView::RenderActive()
{
    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
    
    ImGuiViewport* pViewPort = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(pViewPort->WorkPos, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(pViewPort->WorkSize, ImGuiCond_Appearing);
    ImGui::Begin(GetName().c_str(), nullptr, m_stMainDesc.imgWindowFlags);
    {
    
        // Basic info
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", m_dShowDeltaTime, 1.f / m_dShowDeltaTime);
        ImGui::Text("Application Delta %1f", m_dShowDeltaTime);
        ImGui::Text("Main Cam Pos (%1f,%1f,%1f) ", spGameInstance->GetCameraPosition(0).x, spGameInstance->GetCameraPosition(0).y, spGameInstance->GetCameraPosition(0).z);
        ImGui::Text("Main Cam Look (%1f,%1f,%1f) ", spGameInstance->GetMainCameraTransform()->GetLook().x, spGameInstance->GetMainCameraTransform()->GetLook().y, spGameInstance->GetMainCameraTransform()->GetLook().z);
        ImGui::Text("Main Cam Rotation (%1f,%1f,%1f) ", spGameInstance->GetMainCameraTransform()->GetRotationValue().x, spGameInstance->GetMainCameraTransform()->GetRotationValue().y, spGameInstance->GetMainCameraTransform()->GetRotationValue().z);
        m_stMainDesc.iDockSpaceID = ImGui::GetID(GetName().c_str());
        ImGui::DockSpace(m_stMainDesc.iDockSpaceID, ImVec2{ 0, 0 }, m_stMainDesc.imgDockNodeFlags);

        RenderMenu();

        for (auto& iter : m_OpenImGuies)
        {
            iter->Render();
        }
    }
    ImGui::End();
}

void TMainView::RenderMenu()
{

    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
    ImGui::BeginMainMenuBar();
    {
        if (ImGui::BeginMenu("Tools"))
        {
            for (auto& iter : m_CloseImGuies)
            {
                _bool isOpen = iter->IsOpen();
                if (ImGui::MenuItem(iter->GetName().c_str(), "", &isOpen))
                {
                    if (false == iter->IsActive()) {
                        m_OpenImGuies.insert(iter);
                        iter->OpenImGui();
                    }
                    else
                    {
                        iter->CloseImGui();
                        m_OpenImGuies.erase(iter);
                    }
                }
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Explain"))
        {
            ImGui::Text("Camera Rotation Key - Mouse R Btn");
            ImGui::Text("Camera ZoomIn ZoomOut Key - Mouse Scroll Btn");
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Grid"))
        {
            if (ImGui::MenuItem("Active Grid", nullptr, &m_bGridActive))
            {
                static_pointer_cast<TMainScene>(GetGameInstance()->GetCurScene())->GetGrid()->SetActive(m_bGridActive);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Fog"))
        {
            if (ImGui::MenuItem("Active Fog", nullptr, &m_bFogActive))
            {
                if (m_bFogActive)
                    spGameInstance->TurnOnFog();
                else
                    spGameInstance->TurnOffFog();
            }
            ImGui::EndMenu();
        }

        for (SET<SHPTR<TImGuiView>>::iterator iter = m_OpenImGuies.begin(); iter != m_OpenImGuies.end();)
        {
            if (false == (*iter)->IsOpen()) {
                (*iter)->CloseImGui();
                iter = m_OpenImGuies.erase(iter);
            }
            else {
                ++iter;
            }
        }
    }
    ImGui::EndMainMenuBar();
}