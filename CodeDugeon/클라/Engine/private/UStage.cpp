#include "EngineDefine.h"
#include "UStage.h"
#include "UGameInstance.h"
#include "UCell.h"
#include "URegion.h"
#include "UTransform.h"
#include "UDefaultCube.h"
#include <locale>
#include <codecvt>

UStage::UStage(CSHPTRREF<UDevice> _spDevice)
	: UComponent(_spDevice),
	m_spRegionList{ nullptr },
	m_bInitRegion{ false  }
{
}

UStage::UStage(const UStage& _rhs)
	: UComponent(_rhs),
	m_spRegionList{ nullptr },
	m_bInitRegion{ false }
{
}

void UStage::Free()
{
}

HRESULT UStage::NativeConstruct()
{
	RETURN_CHECK_FAILED(__super::NativeConstruct(), E_FAIL);

	m_spRegionList = Create<REGIONLIST>();

	return S_OK;
}

HRESULT UStage::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	return S_OK;
}

SHPTR<URegion> UStage::GetRegion(const _uint& _iIndex)
{
	RETURN_CHECK(_iIndex >= m_spRegionList->size(), nullptr)
		return (*m_spRegionList.get())[_iIndex];
}


#ifdef _EDIT_NAVI
HRESULT UStage::AddRender(const _uint& _iIndex, _int& begin, _int& end)
{
	if (_iIndex >= m_spRegionList->size())
		return E_FAIL;

	(*m_spRegionList.get())[_iIndex]->AddCellsRenderGroup(begin, end);
	return S_OK;
}

HRESULT UStage::AddRenderAll(const _uint& _iIndex)
{
	if (_iIndex >= m_spRegionList->size())
		return E_FAIL;

	(*m_spRegionList.get())[_iIndex]->AddAllCellsRenderGroup();
	return S_OK;
}

HRESULT UStage::AddCell(const _uint& _iCellIndex, SHPTR<UCell>& _pCell)
{
	if (_iCellIndex >= m_spRegionList->size())
		return E_FAIL;

	(*m_spRegionList.get())[_iCellIndex]->AddCell(_pCell);
	return S_OK;
}

HRESULT UStage::RearrangeCells(const _uint& _iCellIndex)
{
	if (_iCellIndex >= m_spRegionList->size())
		return E_FAIL;

	(*m_spRegionList.get())[_iCellIndex]->RearrageCells();
	return S_OK;
}

HRESULT UStage::ShowCells(const _uint& _iCellIndex)
{
	if (_iCellIndex >= m_spRegionList->size())
		return E_FAIL;

	(*m_spRegionList.get())[_iCellIndex]->ShowCells();
	return S_OK;
}

HRESULT UStage::ClearCell(const _uint& _iCellIndex)
{
	if (_iCellIndex >= m_spRegionList->size())
		return E_FAIL;

	(*m_spRegionList.get())[_iCellIndex]->ClearCell();
	return S_OK;
}

HRESULT UStage::SetColor(const _uint& _iCellIndex)
{
	if (_iCellIndex >= m_spRegionList->size())
		return E_FAIL;

	(*m_spRegionList.get())[_iCellIndex]->SetColor();
	return S_OK;
}

HRESULT UStage::SetRegionName(const _uint& _iCellIndex)
{
	if (_iCellIndex >= m_spRegionList->size())
		return E_FAIL;

	(*m_spRegionList.get())[_iCellIndex]->SetName();
	return S_OK;
}

HRESULT UStage::FlushDeleteCells()
{
	for (auto& iter : (*m_spRegionList.get()))
		if(iter->IsDeletion())
			iter->FlushDeleteCells();
	return S_OK;

}

HRESULT UStage::CreateRegion()
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	if (ImGui::TreeNodeEx("Create_Region", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, 3 * ImGui::GetTextLineHeightWithSpacing())))
		{
			_uint iRegionNum = 0;
			for (auto& iter : (*m_spRegionList.get()))
			{
				char pName[MAX_PATH] = { "Region " };
				sprintf_s(pName, "%d", iRegionNum);
				ImGui::Text(pName);
				iRegionNum++;
			}
			ImGui::EndListBox();
		}

		if (ImGui::Button("Add_Region"))
		{			
			SHPTR<URegion> pRegion = CreateConstructorNative<URegion>(spGameInstance->GetDevice());
			m_spRegionList->push_back(pRegion);

			AddArroundRegion();
		}
		ImGui::TreePop();
	}
	return S_OK;
}

_int UStage::SelectRegion()
{
	_int iIndex = INVALID_MINUS_STAGEVALUE;
	if (ImGui::TreeNodeEx("Select_Region", ImGuiTreeNodeFlags_DefaultOpen))
	{
		_uint iRegionNum = 0;
		for (SHPTR<URegion> it : (*m_spRegionList.get()))
		{
			_wstring nameWstr = it->Get_Name();
			// Convert wstring to UTF-8 string using MultiByteToWideChar
			int utf8Length = WideCharToMultiByte(CP_UTF8, 0, nameWstr.c_str(), -1, NULL, 0, NULL, NULL);
			std::string name(utf8Length, 0);
			WideCharToMultiByte(CP_UTF8, 0, nameWstr.c_str(), -1, &name[0], utf8Length, NULL, NULL);

			if (ImGui::Selectable(name.c_str()))
			{
				iIndex = iRegionNum;
			}
			iRegionNum++;

		}
		ImGui::TreePop();
	}

	return iIndex;
}



HRESULT UStage::Delete_Region(_uint& _iIndex)
{
	if (ImGui::TreeNodeEx("Delete_Current_Region"))
	{
		if (ImGui::Button("Delete"))
		{
			if (_iIndex < m_spRegionList->size())
			{
				(m_spRegionList->begin() + _iIndex)->reset();
				m_spRegionList->erase(m_spRegionList->begin() + _iIndex);
			}
		}
		ImGui::TreePop();
	}
	return S_OK;
}

_bool UStage::Is_Collision(SHPTR<UCollider>& _pCollider, SHPTR<URegion>* _ppOut)
{
	for (auto& iter : (*m_spRegionList.get()))
	{
		if (true == iter->Is_Collision(_pCollider))
		{
			*_ppOut = iter;
			return true;
		}
	}
	return false;
}
#endif

_bool UStage::Load()
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

	m_spRegionList.reset();
	m_spRegionList = Create<REGIONLIST>();

	SHPTR<FILEGROUP> NaviFolder = spGameInstance->FindFolder(L"Navigation");
	// Folders 
	for (const FILEPAIR& File : NaviFolder->FileDataList)
	{
		SHPTR<URegion> pRegion = CreateConstructorNative<URegion>(GetDevice());
		if (nullptr != pRegion)
		{
			pRegion->Load(File.second->wstrfilePath);
			pRegion->SetName(File.first);
			m_spRegionList->push_back(pRegion);
		}
	}
	return true;
}

_bool UStage::Save(const _wstring& _wstrPath)
{
	_wstring str;
	str.assign(_wstrPath.begin(), _wstrPath.end());
	str.append(L"\\Navigation\\");

	for (auto& iter : (*m_spRegionList.get()))
	{
		_wstring RegionNameStr = iter->Get_Name();
		str.append(RegionNameStr);
		iter->Save(str);

		// numStr을 다시 제거
		size_t pos = str.find(RegionNameStr);
		if (pos != _wstring::npos)
			str.erase(pos, RegionNameStr.length());
	}
	return true;
}

HRESULT UStage::SetCellJumpState(const _uint& _iCellIndex)
{
	if (_iCellIndex >= m_spRegionList->size())
		return E_FAIL;

	//(*m_spRegionList.get())[_iCellIndex]->SetCellJumpState();
	return S_OK;
}


void UStage::UpdateRegion()
{
	if (false == m_bInitRegion)
	{
		AddArroundRegion();
		m_bInitRegion = true;
	}
}

void UStage::AddArroundRegion()
{
	for (auto& iter : (*m_spRegionList.get()))
	{
		iter->ClearNeightborRegion();
		for (auto& value : (*m_spRegionList.get()))
		{
			if (value == iter)
				continue;
			iter->Add_NeighborRegion(value);
		}
	}
}


