#include "AIConViewer.h"
#include "GameInstance.h"
#include "Player.h"
#include "Battle_Manager.h"
#include "AI_CharController.h"

CAIConViewer::CAIConViewer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)  
	: CImGuiClass(_pDevice, _pContext)
{
}

HRESULT CAIConViewer::NativeConstruct()
{
	m_bOpen = false;
	m_pName = "AIConViewer";
	return S_OK;
}

HRESULT CAIConViewer::NativeConstruct_InnerValue()
{
	return S_OK;
}

HRESULT CAIConViewer::Tick(const _double& _dDeltaSeconds)
{
	if (FAILED(__super::Tick(_dDeltaSeconds)))
		return E_FAIL;

	Find_Current_AI_Battle();
	return S_OK;
}

HRESULT CAIConViewer::Late_Tick(const _double& _dDeltaSeconds)
{
	if (FAILED(__super::Late_Tick(_dDeltaSeconds)))
		return E_FAIL;

	Show_Window();
	return S_OK;
}

HRESULT CAIConViewer::Release_InnerValue()
{
	return S_OK;
}

void CAIConViewer::Show_Window()
{
	ImVec2 imVec2 = ImVec2(static_cast<_float>(g_iWinSizeX - 180.f), 10.f);
	ImGui::SetNextWindowPos(ImVec2(imVec2.x + 20.f, 5.f + imVec2.y + 20.f), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(300.f, 300.f), ImGuiCond_Appearing);
	// Hierachy
	ImGui::Begin(m_pName, &m_bOpen);
	{
		if (nullptr != m_pAICharController)
		{
			VOIDDATAS Datas;
			Datas = *m_pAICharController->FillOut_AIControllerData(&Datas);
			if (Datas.size() > 0)
			{
				ImGui::Text("AI_ConViewer");
				ImGui::Checkbox("AI Stop", (_bool*)Datas[CAI_CharController::AICON_STOP]);
			}
		}
	}
	ImGui::End();
}

void CAIConViewer::Find_Current_AI_Battle()
{
	if (nullptr == m_pAIPlayer)
	{
		CBattle_Manager* pInstnace = GET_INSTANCE(CBattle_Manager);
		{
			m_pAIPlayer = static_cast<CPlayer*>(pInstnace->Get_BattleCharacter(CBattle_Manager::CHAR_AI));
			Safe_AddRef(m_pAIPlayer);
		}
		RELEASE_INSTANCE(CBattle_Manager);
	}
	else
	{
		if (nullptr == m_pAICharController)
		{
			m_pAICharController = static_cast<CAI_CharController*>(m_pAIPlayer->Get_Controller());
			Safe_AddRef(m_pAICharController);
		}
	}
}

CAIConViewer* CAIConViewer::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CAIConViewer* pInstance = new CAIConViewer(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct()))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CAIConViewer::Create To Failed");
#endif
	}
	return pInstance;
}

void CAIConViewer::Free()
{
	Safe_Release(m_pAICharController);
	Safe_Release(m_pAIPlayer);
	__super::Free();
}
