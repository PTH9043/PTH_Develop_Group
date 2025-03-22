#include "EngineDefine.h"
#include "UFilePathManager.h"
#include <filesystem>

namespace fs = std::filesystem;

UFilePathManager::UFilePathManager() : m_spFileGroup(nullptr)
{
}

void UFilePathManager::Free()
{
	LIST<SHPTR<FILEGROUP>> FILEGROUPS;
	ClearLoader(FILEGROUPS, m_spFileGroup);
	FILEGROUPS.clear();
}

SHPTR<FILEGROUP> UFilePathManager::FindFolder(const PATHS& _vecFolderList)
{
	RETURN_CHECK(nullptr == m_spFileGroup, nullptr);
	RETURN_CHECK(0 >= _vecFolderList.size(), nullptr);

	UNORMAP<_wstring, SHPTR<FILEGROUP>>::iterator it;
	_uint i = 0;
	_uint iMax = (_uint)_vecFolderList.size() - 1;
	_bool bRun{ true };
	SHPTR<FILEGROUP> pFindGroup = m_spFileGroup;
	while (bRun)
	{
		pFindGroup = pFindGroup->FindGroup(_vecFolderList[i]);
		RETURN_CHECK(nullptr == pFindGroup, nullptr);
		if (i++ == iMax)
			break;
	}

	return pFindGroup;
}

SHPTR<FILEGROUP> UFilePathManager::FindFolder(const _wstring& _wstrFindName, const _wstring& _wstrParentsFolderName)
{
	const auto& iter = m_FileFolderGroup.find(_wstrFindName);
	if (iter == m_FileFolderGroup.end())
		return nullptr;

	if (L"" == _wstrParentsFolderName)
		return iter->second.front();
	else
	{
		for (auto& value : iter->second)
		{
			if (value->wstrFolderName == _wstrParentsFolderName)
				return value;
		}
	}
	return iter->second.front();
}

VECTOR<SHPTR<FILEGROUP>> UFilePathManager::FindSameAllFolder(const _wstring& _wstrFindFolderName)
{
	const auto& iter = m_FileFolderGroup.find(_wstrFindFolderName);
	RETURN_CHECK(m_FileFolderGroup.end() == iter, VECTOR<SHPTR<FILEGROUP>>{});
	return iter->second;
}

HRESULT UFilePathManager::LoadFirstFolder(const _wstring& _wstrFilePath)
{
	LIST<SHPTR<FILEGROUP>> FILEGROUPS;
	ClearLoader(FILEGROUPS, m_spFileGroup);
	FILEGROUPS.clear();
	m_FileFolderGroup.clear();
	m_spFileGroup.reset();
	m_spFileGroup = Make::MakeShared<FILEGROUP>();
	fs::path p{ _wstrFilePath };
	VECTOR<SHPTR<FILEGROUP>> vecFileGroup;
	vecFileGroup.push_back(m_spFileGroup);
	m_spFileGroup->wstrFolderName =p.filename();
	m_FileFolderGroup.insert(MakePair(m_spFileGroup->wstrFolderName, vecFileGroup));
	LoadUpperFolder(m_spFileGroup->wstrFolderName, _wstrFilePath, m_spFileGroup);
	m_spFileGroup->wstrPath = _wstrFilePath;
	return S_OK;
}

HRESULT UFilePathManager::ClearLoader(LIST<SHPTR<FILEGROUP>>& _lsFileGroup, CSHPTRREF<FILEGROUP> _spFileGroup)
{
	RETURN_CHECK(nullptr == _spFileGroup, E_FAIL);

	SHPTR<FILEGROUP> spFileGroup = _spFileGroup;

	for (auto& iter : spFileGroup->UnderFileGroupList)
	{
		_lsFileGroup.push_back(iter.second);
		ClearLoader(_lsFileGroup, iter.second);
	}
	_spFileGroup->FileDataList.clear();
	_spFileGroup->UnderFileGroupList.clear();
	return S_OK;
}

HRESULT UFilePathManager::LoadUpperFolder(const _wstring& _wstrFirstFolderName,
	const _wstring& _wstrPath, CSHPTRREF<FILEGROUP> _spFileGroup)
{
	fs::directory_iterator end;
	_ushort sIndex{ 0 };
	LIST<SHPTR<FILEGROUP>> MAKEGROUP;
	for (fs::directory_iterator iter(_wstrPath); iter != end; ++iter)
	{
		if (fs::is_regular_file(iter->status()))
		{
			SHPTR<FILEDATA> spFileData = Make::MakeShared<FILEDATA>();
			spFileData->wstrfileName = iter->path().filename();
			spFileData->wstrfilePath = iter->path();
			spFileData->sindex = sIndex;
			spFileData->wpFolder = _spFileGroup;
			_spFileGroup->AddFileData(spFileData->wstrfileName, spFileData);
		}
		else if (fs::is_directory(iter->status()))
		{
			SHPTR<FILEGROUP> spFileGroup = Make::MakeShared<FILEGROUP>();
			spFileGroup->wstrPath = iter->path();
			spFileGroup->wstrFolderName = iter->path().filename();
			spFileGroup->spParentsUpper = _spFileGroup;
			_spFileGroup->AddFileGroup(spFileGroup->wstrFolderName, spFileGroup);
			MAKEGROUP.push_back(spFileGroup);
			const auto& iter = m_FileFolderGroup.find(spFileGroup->wstrFolderName);
			if (iter == m_FileFolderGroup.end())
			{
				VECTOR<SHPTR<FILEGROUP>> vecFileGroup;
				vecFileGroup.push_back(spFileGroup);
				m_FileFolderGroup.insert(MakePair(spFileGroup->wstrFolderName, vecFileGroup));
			}
			else
			{
				iter->second.push_back(spFileGroup);
			}
		}
	}
	for (auto& iter : MAKEGROUP)
	{
		LoadUpperFolder(iter->wstrFolderName, iter->wstrPath, iter);
	}

	return S_OK;
}
