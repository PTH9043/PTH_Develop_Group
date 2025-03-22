#include "CMainView.h"
#include "CImGui_Manager.h"
#include "CStage.h"
#include "CGameInstance.h"
#include "CWorldObj.h"

CStage::CStage(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: m_pDevice(_pDevice), m_pContext(_pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CRegion* CStage::Get_Region(const _uint& _iIndex)
{
	if (_iIndex >= m_RegionList.size())
		return nullptr;
	return m_RegionList[_iIndex];
}

HRESULT CStage::NativeConstruct()
{
	return S_OK;
}
#ifdef _DEBUG
HRESULT CStage::Render(const _uint& _iIndex)
{
	if (_iIndex >= m_RegionList.size())
		return E_FAIL;

	m_RegionList[_iIndex]->Add_Render();
	return S_OK;
}
void CStage::All_Render()
{
	for (auto& iter : m_RegionList)
		iter.second->Add_Render();
}
#endif

HRESULT CStage::Add_Cell(const _uint& _iCellIndex, CCell* _pCell)
{
	if (_iCellIndex >= m_RegionList.size())
		return E_FAIL;

	m_RegionList[_iCellIndex]->Add_Cell(_pCell);
	return S_OK;
}

HRESULT CStage::Modify_Cells(const _uint& _iCellIndex)
{
	if (_iCellIndex >= m_RegionList.size())
		return E_FAIL;

	m_RegionList[_iCellIndex]->Modify_Cells();
	return S_OK;
}

HRESULT CStage::Show_Cells(const _uint& _iCellIndex)
{
	if (_iCellIndex >= m_RegionList.size())
		return E_FAIL;

	m_RegionList[_iCellIndex]->Show_Cells();
	return S_OK;
}

HRESULT CStage::Clear_Cell(const _uint& _iCellIndex)
{
	if (_iCellIndex >= m_RegionList.size())
		return E_FAIL;

	m_RegionList[_iCellIndex]->Clear_Cell();
	return S_OK;
}

HRESULT CStage::LoadFile(const _wstring& _strFile)
{
	CRegion* pRegion = CRegion::Create(m_pDevice, m_pContext);
	if (nullptr != pRegion)
	{
		pRegion->LoadFile(_strFile);
		pRegion->Set_Index((_uint)m_RegionList.size());
		m_RegionList.emplace(pair<_uint, CRegion*>((_uint)m_RegionList.size(), pRegion));
	}
	return S_OK;
}

HRESULT CStage::SaveFile(const _wstring& _strFile)
{
	for (auto& iter : m_RegionList)
	{
		iter.second->SaveFile(_strFile);
	}
	return S_OK;
}

HRESULT CStage::Create_Region(const _uint& _iIndex)
{
	if (ImGui::TreeNodeEx("Create_Region", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, 3 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (auto& iter : m_RegionList)
			{
				char pName[MAX_PATH] = { "" };
				sprintf_s(pName, "%d", iter.second->Get_Index());
				ImGui::Text(pName);
			}
			ImGui::EndListBox();
		}

		if (ImGui::Button("Add_Region"))
		{
			for (auto& iter : m_RegionList)
			{
				if (iter.first == _iIndex)
				{
					ImGui::TreePop();
					return E_FAIL;
				}

				if (iter.second->Get_Index() == _iIndex)
				{
					ImGui::TreePop();
					return E_FAIL;
				}
			}
			CRegion* pRegion = CRegion::Create(m_pDevice, m_pContext);
			pRegion->Set_Index(_iIndex);
			m_RegionList.emplace(pair<_uint, CRegion*>(_iIndex, pRegion));

			Add_ArroundReigon();
		}

		ImGui::TreePop();
	}
	return S_OK;
}

_int CStage::Select_Region()
{
	_int iIndex = INVALID_MINUS_STAGEVALUE;
	if (ImGui::TreeNodeEx("Select_Region", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (map<_uint, CRegion*>::iterator it = m_RegionList.begin(); it != m_RegionList.end(); ++it)
		{
			char pName[MAX_PATH] = { "" };
			sprintf_s(pName, "%d", (*it).second->Get_Index());
			if (ImGui::Selectable(pName))
			{
				iIndex = it->first;
			}
		}
		ImGui::TreePop();
	}
	return iIndex;
}

void CStage::Control_Collider(const _uint& _iIndex)
{
	if (_iIndex >= m_RegionList.size())
		return;

	m_RegionList[_iIndex]->Control_Collider();
	return;
}

HRESULT CStage::Delete_Region(_uint& _iIndex)
{
	if (ImGui::TreeNodeEx("Delete_Region"))
	{
		if (ImGui::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, 3 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (map<_uint, CRegion*>::iterator it = m_RegionList.begin(); it != m_RegionList.end(); ++it)
			{
				char pName[MAX_PATH] = { "" };
				sprintf_s(pName, "%d", (*it).second->Get_Index());
				if (ImGui::Selectable(pName))
				{
					m_pDeleteRegion = (*it).second;
				}
			}
			ImGui::EndListBox();
		}

		if (ImGui::Button("Delete"))
		{
			for (map<_uint, CRegion*>::iterator it = m_RegionList.begin(); it != m_RegionList.end(); ++it)
			{
				if (m_pDeleteRegion == (*it).second)
				{
					Safe_Release((*it).second);
					m_RegionList.erase(it);
					m_pDeleteRegion = nullptr;
					_iIndex = 100000;
					break;
				}
			}
		}
		ImGui::TreePop();
	}
	return S_OK;
}

_bool CStage::Is_Collision(CCollider* _pCollider, CRegion** _ppOut)
{
	for (auto& iter : m_RegionList)
	{
		if (true == iter.second->Is_Collision(_pCollider))
		{
			*_ppOut = iter.second;
			return true;
		}
	}
	return false;
}

void CStage::Update_Region()
{
	for (auto& iter : m_RegionList)
		iter.second->Update_WorldObjects();

	if (g_bGameRun && false == m_bInitRegion)
	{
		Add_ArroundReigon();
		m_bInitRegion = true;
	}
}

void CStage::Add_ArroundReigon()
{
	for (auto& iter : m_RegionList)
	{
		iter.second->Clear_NeightborRegion();
		for (auto& value : m_RegionList)
		{
			if (value == iter)
				continue;
			iter.second->Add_NeighborRegion(value.second);
		}
	}
}

_bool CStage::Is_Picking(const _uint& _iIndex)
{
	if (_iIndex >= m_RegionList.size())
		return false ;

	_bool bIsTrue = false;
	BEGIN_SINGLETON(CGameInstance, pGameInstance)
	{
		for (auto& iter : m_RegionList[_iIndex]->Get_WorldObjList())
			iter->Is_PickingNav();
	}
	END_SINGLETON(pGameInstance);
	return true;
}

_bool CStage::Is_ModelPicking(const _uint& _iIndex)
{
	if (_iIndex >= m_RegionList.size())
		return false;

	_bool bIsTrue = false;
	BEGIN_SINGLETON(CGameInstance, pGameInstance)
	{
		for (auto& iter : m_RegionList[_iIndex]->Get_WorldObjList())
			iter->Is_Picking();
	}
	END_SINGLETON(pGameInstance);
	return true;
}

CStage* CStage::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CStage* pInstance = new CStage(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CStage::Create To Failed");
	}
	return pInstance;
}

void CStage::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	for_each(m_RegionList.begin(), m_RegionList.end(), [&](auto& Pair)
	{
		Safe_Release(Pair.second);
	});
}
