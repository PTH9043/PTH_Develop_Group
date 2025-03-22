#include"imgui.h"
#include"imgui_internal.h"
#include"imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "CCamTool_MD.h"
#include "CCamera_MD.h"
#include "CGameInstance.h"

CCamTool_MD::CCamTool_MD(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CIMGUI_MD(_pDevice, _pContext)
{
}

HRESULT CCamTool_MD::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	m_pName = "CamTool";
	m_bOpen = true;
	return S_OK;
}

HRESULT CCamTool_MD::Tick(const _double& _dDeltaSeconds)
{
	if (FAILED(__super::Tick(_dDeltaSeconds)))
		return E_FAIL;

	if (nullptr == m_pCamera)
	{
		BEGIN_SINGLETON(CGameInstance, pGameInstance);
		{
			m_pCamera = static_cast<CCamera_MD*>(pGameInstance->Find_GameObject(LEVEL_TOOL, LAYER_CAM, 0));
			Safe_AddRef(m_pCamera);
		}
		END_SINGLETON(pGameInstance);
	}

	return S_OK;
}

HRESULT CCamTool_MD::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	Show_Window();
	return S_OK;
}

void CCamTool_MD::Show_Window()
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

void CCamTool_MD::Control_Camera()
{
	if (nullptr == m_pCamera)
		return;

	m_pCamera->Show_CamControlData();
}

CCamTool_MD* CCamTool_MD::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCamTool_MD* pInstance = new CCamTool_MD(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CCamTool_MD::Create to Failed");
	}
	return pInstance;
}

void CCamTool_MD::Free()
{
	Safe_Release(m_pCamera);
	__super::Free();
}
