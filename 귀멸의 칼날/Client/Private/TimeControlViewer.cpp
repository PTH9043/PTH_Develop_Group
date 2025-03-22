#include "TimeControlViewer.h"
#include "GameInstance.h"
#include "Level_Loading.h"

CTimerControlViewer::CTimerControlViewer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CImGuiClass(_pDevice, _pContext)
{
}

HRESULT CTimerControlViewer::NativeConstruct()
{
	m_tWinDesc = WINDESC("LevelControlViewer", ImGuiWindowFlags_MenuBar,
		ImVec2(1.f, 1.f), ImVec2(620.f, 620.f), true);
	m_pName = "LevelControl_Viewer";

	return S_OK;
}

HRESULT CTimerControlViewer::NativeConstruct_InnerValue()
{
	return S_OK;
}

HRESULT CTimerControlViewer::Tick(const _double& _dDeltaSeconds)
{
	if (FAILED(__super::Tick(_dDeltaSeconds)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTimerControlViewer::Late_Tick(const _double& _dDeltaSeconds)
{
	if (FAILED(__super::Late_Tick(_dDeltaSeconds)))
		return E_FAIL;

	Show_Window();
	return S_OK;
}

HRESULT CTimerControlViewer::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTimerControlViewer::Release_InnerValue()
{
	return S_OK;
}

void CTimerControlViewer::Show_Window()
{
	ImVec2 imVec2 = ImVec2(static_cast<_float>(g_iWinSizeX - 180.f), 10.f);
	ImGui::SetNextWindowPos(ImVec2(imVec2.x + m_tWinDesc.v2Pos.x, 5.f + imVec2.y + m_tWinDesc.v2Pos.y), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(m_tWinDesc.v2Size, ImGuiCond_Appearing);
	ImGui::Begin(m_tWinDesc.strName.c_str(), &m_tWinDesc.bEnable);
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		{
			_float* pUpdateTime = nullptr, * pLateUpdateTime = nullptr;
			{
				pGameInstance->Out_LevelUpdateTime(pUpdateTime, pLateUpdateTime);
				m_ClonesLayerStr.clear();
#ifdef _DEBUG
				pGameInstance->Out_LayerTags(g_iCurrentLevel, &m_ClonesLayerStr);
#endif
			}
			ImGui::Text("Level");
			if (nullptr != pUpdateTime)
			{
				ImGui::DragFloat("UpdateTime_Level", pUpdateTime, -0.001f, 1000.f);
			}
			if (nullptr != pLateUpdateTime)
			{
				ImGui::DragFloat("LateUpdateTime_Level", pLateUpdateTime, -0.001f, 1000.f);
			}
			ImGui::NewLine();
			ImGui::Text("Layer");
			for (auto& iter : m_ClonesLayerStr)
			{
				if (ImGui::TreeNode(CMethod::ConvertWToS(iter).c_str()))
				{
					_float* pLayerUpdateTime = nullptr, * pLayerLateUpdateTime = nullptr;
					pGameInstance->Out_LayerUpdateTime(g_iCurrentLevel, iter, pLateUpdateTime, pLayerLateUpdateTime);
					if (nullptr != pUpdateTime)
					{
						ImGui::DragFloat("UpdateTime_Layer", pLayerUpdateTime, -0.001f, 1000.f);
					}
					if (nullptr != pLateUpdateTime)
					{
						ImGui::DragFloat("LateUpdateTime_Layer", pLayerLateUpdateTime, -0.001f, 1000.f);
					}
					ImGui::TreePop();
				}
			}
		}
		RELEASE_INSTANCE(CGameInstance);
	}
	ImGui::End();
}


CTimerControlViewer* CTimerControlViewer::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CTimerControlViewer* pInstance = new CTimerControlViewer(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct()))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CLevelControlViewer::Create To Failed");
#endif
	}
	return pInstance;
}

void CTimerControlViewer::Free()
{
	__super::Free();
}
