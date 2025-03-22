#include "FilePath_Manager.h"
#include "Method.h"
#include <ShlObj_core.h>

IMPLEMENT_SINGLETON(CFilePath_Manager)

CFilePath_Manager::CFilePath_Manager() 
{
	m_tFileGroup = new FILEGROUP;
}

FILEGROUP* CFilePath_Manager::Find_Folder(vector<_wstring>* _pFolderList)
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

FILEGROUP* CFilePath_Manager::Find_Folder(const _wstring& _strPath)
{
	list<FILEDATA> tFindData;
	_wstring strPath = _strPath;

	PATHS Path = CMethod::Split_Paths(strPath, L"..\\Resources");
	return Find_Folder(&Path);
}

list<FILEDATA> CFilePath_Manager::Find_FolderList(vector<_wstring>* _pFolderList)
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

HRESULT CFilePath_Manager::Load_FirstFolder(const _tchar* _pFindPath)
{
	list<FILEGROUP*>  FileGroup;
	Clear_Loader(FileGroup, m_tFileGroup);

	if (FileGroup.size() > 0)
	{
		for (auto& iter : FileGroup)
			Safe_Delete(iter);

		Safe_Delete(m_tFileGroup);
		m_tFileGroup = new FILEGROUP;
	}

	WIN32_FIND_DATA findData = {};
	WIN32_FIND_DATA parentData = {};

	wstring strPath = _pFindPath;
	strPath.append(L"\\*.*");

	HANDLE hFile = FindFirstFile(strPath.c_str(), &findData);
	// 최상위 폴더가 없을 경우 리턴
	if (hFile == INVALID_HANDLE_VALUE)
		return E_FAIL;

	BOOL bResult = true;
	m_tFileGroup->strUppderName = L"Rersources";
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

HRESULT CFilePath_Manager::Open_SavePath( _wstring* _pOutPath)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	TCHAR szFile[260] = L"";
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"Save Files( *.bin)\\0*.bin0All Files (*.*)\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_OVERWRITEPROMPT;

	if (::GetSaveFileName(&ofn) == false)
		return E_FAIL;

	if (nullptr != _pOutPath)
		*_pOutPath = ofn.lpstrFile;
	return S_OK;
}

HRESULT CFilePath_Manager::Open_SaveFolder(_wstring* _pOutPath)
{
	TCHAR cpath[MAX_PATH] = L"";
	LPITEMIDLIST pDirList;
	BROWSEINFO browseInfo;
	browseInfo.hwndOwner = NULL;
	browseInfo.pidlRoot = NULL;
	browseInfo.lpszTitle = L"불러올 폴더를 선택해주세용";
	browseInfo.pszDisplayName = cpath;
	browseInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	browseInfo.lpfn = NULL;
	browseInfo.lParam = 0;

	pDirList = SHBrowseForFolder(&browseInfo);
	if (pDirList == NULL) return E_FAIL;
	SHGetPathFromIDList(pDirList, cpath);
	if (nullptr != _pOutPath)
		*_pOutPath = cpath;

	return S_OK;
}

HRESULT CFilePath_Manager::Open_FolderSelectRoot(const _wstring& _strPath, const _wstring& _strName, _wstring* _pOutPath)
{// set current directory to be root directory
	
	_tchar PATH[MAX_PATH] = L"";
	::GetCurrentDirectory(sizeof(_tchar) * MAX_PATH, PATH);
	_wstring str = PATH;
	size_t FIND = str.find(L"Tool");
	str = str.substr(0, FIND);
	_wstring Path = _strPath;
	size_t INDEX = Path.find_first_of(m_tFileGroup->strUppderName, 0);
	Path = Path.substr(INDEX, Path.length());
	str.append(Path);

	lstrcpy(PATH, str.c_str());
	LPITEMIDLIST pStartDir;
	ULONG nCharsParsed = 0;
	LPSHELLFOLDER pShellFolder = NULL;
	SHGetDesktopFolder(&pShellFolder);
	HRESULT hr = pShellFolder->ParseDisplayName(NULL, NULL, PATH, &nCharsParsed, &pStartDir, NULL);
	pShellFolder->Release();
	// Get directory from user
	TCHAR cpath[MAX_PATH];
	LPITEMIDLIST pDirList;
	BROWSEINFO browseInfo;
	browseInfo.hwndOwner = NULL;	// m_hWnd
	browseInfo.pidlRoot = pStartDir;
	browseInfo.lpszTitle = L"불러올 폴더를 선택해주세용";
	browseInfo.pszDisplayName = cpath;
	browseInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	browseInfo.lpfn = NULL;
	browseInfo.lParam = 0;
	pDirList = SHBrowseForFolder(&browseInfo);
	if (pDirList == NULL) return E_FAIL;

	SHGetPathFromIDList(pDirList, cpath);
	TCHAR* return_path = cpath;

	if (nullptr != _pOutPath)
		*_pOutPath = cpath;

	ShellExecute(NULL, L"open", cpath, NULL, NULL, SW_SHOWNORMAL);
	return S_OK;
}

HRESULT CFilePath_Manager::Load_UpperFolder(const _tchar* _pFirstFolderName, const _tchar* _pPath, FILEGROUP* _pFileGroup)
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
			tData.pFolderAddress = &_pFileGroup;
			// Out 
			_pFileGroup->vecFileDataList.push_back(tData);
		}
		bResult = (FindNextFile(hFile, &findData));
	}
	_pFileGroup->strUppderName = _pFirstFolderName;
	FindClose(hFile);
	return S_OK;
}

HRESULT CFilePath_Manager::Clear_Loader(list<FILEGROUP*>& _lsFileGroup, FILEGROUP* _pFileGroup)
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

void CFilePath_Manager::Free()
{
	list<FILEGROUP*>  FileGroup;
	Clear_Loader(FileGroup, m_tFileGroup);

	for (auto& iter : FileGroup)
		Safe_Delete(iter);

	Safe_Delete(m_tFileGroup);
}
