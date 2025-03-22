#include "CFile_Manager.h"

IMPLEMENT_SINGLETON(CFile_Manager)

CFile_Manager::CFile_Manager()
{
	m_tFileGroup = new FILEGROUP;
}

FILEGROUP* CFile_Manager::Find_Folder(vector<_wstring>* _pFolderList)
{
	if (nullptr == m_tFileGroup && nullptr == _pFolderList)
		return nullptr;

	if (_pFolderList->size() <= 0)
		return nullptr;

	FILEGROUP* pFindGroup = m_tFileGroup;
	FILEGROUP* pTarget = nullptr;
	_uint i = 0;
	_uint iMax = (_uint)_pFolderList->size() - 1;
	_bool bRun = true;
	while (bRun)
	{
		_bool bJudge = false;
		for (auto& iter : pFindGroup->lsUnderList)
		{
			if (!lstrcmp(iter->strUppderName.c_str(), (*_pFolderList)[i].c_str()))
			{
				if (i == iMax)
					pTarget = iter;
				else
				{
					bJudge = true;
					pFindGroup = iter;
				}
				++i;
				break;
			}
		}
		bRun = bJudge;
	}

	return pTarget;
}

FILEGROUP* CFile_Manager::Find_Folder(const _wstring& _strPath)
{
	list<FILEDATA> tFindData;
	vector<_wstring> strFolderPath;
	_wstring strPath = _strPath;
	int i = 0;
	int j = 0;
	while (j > 0)
	{
		int j = (int)strPath.find(L"\\");
		_wstring str = strPath.substr(i, j);
		i = j;
		strFolderPath.push_back(str);
	}

	return Find_Folder(&strFolderPath);
}

list<FILEDATA> CFile_Manager::Find_FolderList(vector<_wstring>* _pFolderList)
{
	if (nullptr == m_tFileGroup && nullptr == _pFolderList)
		return m_EmptyList;

	if (_pFolderList->size() <= 0)
		return m_EmptyList;

	list<FILEDATA> FileList;
	FILEGROUP* pFindGroup = m_tFileGroup;
	_uint i = 0;
	_uint iMax = (_uint)_pFolderList->size() - 1;
	_bool bRun = true;
	while (bRun)
	{
		_bool bJudge = false;
		for (auto& iter : pFindGroup->lsUnderList)
		{
			if (!lstrcmp(iter->strUppderName.c_str(), (*_pFolderList)[i].c_str()))
			{
				for (auto& value : iter->vecFileDataList)
					FileList.push_back(value);

				if (i != iMax)
				{
					bJudge = true;
					pFindGroup = iter;
				}

				++i;
				break;
			}
		}
		bRun = bJudge;
	}

	return FileList;
}

HRESULT CFile_Manager::Load_FirstFolder(const _tchar* _pFindPath)
{
	WIN32_FIND_DATA findData = {};
	WIN32_FIND_DATA parentData = {};

	wstring strPath = _pFindPath;
	strPath.append(L"\\*.*");

	HANDLE hFile = FindFirstFile(strPath.c_str(), &findData);
	// 최상위 폴더가 없을 경우 리턴
	if (hFile == INVALID_HANDLE_VALUE)
		return E_FAIL;

	BOOL bResult = true;
	m_tFileGroup->strUppderName = L"Rersource";
	m_tFileGroup->strPath = _pFindPath;
	while (bResult)
	{
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			FILEGROUP tDesc;
			// . , .. 폴더 제외
			if (lstrcmp(findData.cFileName, L".") && lstrcmp(findData.cFileName, L".."))
			{
				wstring strNewPath = _pFindPath;
				strNewPath.append(L"\\");
				strNewPath.append(findData.cFileName);
				FILEGROUP* tFileGroup = new FILEGROUP;
				tFileGroup->strUppderName = findData.cFileName;
				tFileGroup->strPath = strNewPath;
				m_tFileGroup->lsUnderList.push_back(tFileGroup);
				Load_UpperFolder(findData.cFileName, strNewPath.c_str(), tFileGroup);
			}
		}
		bResult = (FindNextFile(hFile, &findData));
	}
	FindClose(hFile);
	return S_OK;
}

HRESULT CFile_Manager::Load_UpperFolder(const _tchar* _pFirstFolderName, const _tchar* _pPath, FILEGROUP* _pFileGroup)
{
	WIN32_FIND_DATA findData = {};
	WIN32_FIND_DATA parentData = {};

	wstring strPath = _pPath;
	strPath.append(L"\\*.*");

	HANDLE hFile = FindFirstFile(strPath.c_str(), &findData);
	// 최상위 폴더가 없을 경우 리턴
	if (hFile == INVALID_HANDLE_VALUE)
		return E_FAIL;

	BOOL bResult = true;
	_uint iFileCount = 0;
	FILEGROUP* tFileGroup = nullptr;
	while (bResult)
	{
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// . , .. 폴더 제외
			if (lstrcmp(findData.cFileName, L".") && lstrcmp(findData.cFileName, L".."))
			{
				wstring strNewPath = _pPath;
				strNewPath.append(L"\\");
				strNewPath.append(findData.cFileName);
				FILEGROUP* tFileGroup = new FILEGROUP;
				tFileGroup->strUppderName = _pFirstFolderName;
				tFileGroup->strPath = strNewPath;
				_pFileGroup->lsUnderList.push_back(tFileGroup);
				tFileGroup->pParentsUpper = _pFileGroup;
				Load_UpperFolder(findData.cFileName, strNewPath.c_str(), tFileGroup);
			}
		}
		else
		{
			// New Path
			wstring strNewPath = _pPath;
			strNewPath.append(L"\\");
			strNewPath.append(findData.cFileName);
			// File Name
			wstring strFileName = findData.cFileName;
			_uint iIndex = (_uint)strFileName.find(L".");
			// Data Define
			FILEDATA tData;
			tData.iIndex = iFileCount++;
			tData.strFileName = strFileName.substr(0, iIndex);
	//		tData.strFileType = strFileName.substr(iIndex, strFileName.length());
			tData.strFilePath = strNewPath;
			// Out 
			_pFileGroup->vecFileDataList.push_back(tData);
		}
		bResult = (FindNextFile(hFile, &findData));
	}
	_pFileGroup->strUppderName = _pFirstFolderName;
	FindClose(hFile);
	return S_OK;
}

HRESULT CFile_Manager::Clear_Loader(list<FILEGROUP*>& _lsFileGroup, FILEGROUP* _pFileGroup)
{
	if (nullptr == _pFileGroup)
		return E_FAIL;

	FILEGROUP* pFileGroup = _pFileGroup;

	for (auto& iter : pFileGroup->lsUnderList)
	{
		_lsFileGroup.push_back(iter);
		Clear_Loader(_lsFileGroup, iter);
	}
	pFileGroup->lsUnderList.clear();
	return S_OK;
}

void CFile_Manager::Free()
{
	list<FILEGROUP*>  FileGroup;
	Clear_Loader(FileGroup, m_tFileGroup);

	for (auto& iter : FileGroup)
		Safe_Delete(iter);

	Safe_Delete(m_tFileGroup);
}


/*
WIN32_FIND_DATA findData = {};
	WIN32_FIND_DATA parentData = {};

	wstring strPath = _pPath;
	strPath.append(L"\\*.*");

	HANDLE hFile = FindFirstFile(strPath.c_str(), &findData);
	// 최상위 폴더가 없을 경우 리턴
	if (hFile == INVALID_HANDLE_VALUE)
		return E_FAIL;

	BOOL bResult = true;
	_uint iFileCount = 0;
	while (bResult)
	{
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// . , .. 폴더 제외
			if (lstrcmp(findData.cFileName, L".") && lstrcmp(findData.cFileName, L".."))
			{
				FILEDESC tDesc = _tDesc;
				wstring strNewPath = _pPath;
				strNewPath.append(L"\\");
				strNewPath.append(findData.cFileName);
				tDesc.strUppderName.append(_pFirstFolderName);
				tDesc.strUppderName.append(L"\\");
				tDesc.lsFolderList.emplace_back(_pFirstFolderName);
				FILEGROUP* tFileGroup = new FILEGROUP;
				tFileGroup->strUppderName = findData.cFileName;
				_pFileGroup->m_lsUnderList.emplace_back(tFileGroup);
				Load_UpperFolder(findData.cFileName, strNewPath.c_str(), tDesc, tFileGroup);
			}
		}
		else
		{
			// New Path
			wstring strNewPath = _pPath;
			strNewPath.append(L"\\");
			strNewPath.append(findData.cFileName);
			// File Name
			wstring strFileName = findData.cFileName;
			_uint iIndex = (_uint)strFileName.find(L".");
			// Data Define
			FILEDATA tData;
			tData.iIndex = iFileCount++;
			tData.strFileName = strFileName.substr(0, iIndex);
			tData.strFileType = strFileName.substr(iIndex, strFileName.length());
			tData.strFilePath = strNewPath;
			// Out
			(_tDesc).lsFileDataList.emplace_back(tData);
			_pFileGroup->lsFileDataList.emplace_back(tData);
		}
		bResult = (FindNextFile(hFile, &findData));
	}
	if (_tDesc.lsFileDataList.size() > 0)
	{
		_tDesc.strUppderName.append(_pFirstFolderName);
		_tDesc.lsFolderList.emplace_back(_pFirstFolderName);
		m_FileGroup.emplace_back(_tDesc);
	}
	FindClose(hFile);
*/