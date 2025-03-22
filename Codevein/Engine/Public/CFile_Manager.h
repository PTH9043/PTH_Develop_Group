#pragma once
#include "CBase.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class CFile_Manager final : public CBase
{
	DECLARE_SINGLETON(CFile_Manager)
public:

protected:
	explicit CFile_Manager();
	virtual ~CFile_Manager() = default;
public:
	// Get File Group
	FILEGROUP* Get_FileGroup() { return m_tFileGroup; }
public:
	// Find Folder
	FILEGROUP* Find_Folder(vector<_wstring>* _pFolderList);
	// Path
	FILEGROUP* Find_Folder(const _wstring& _strPath);
	// Find Folder List
	list<FILEDATA> Find_FolderList(vector<_wstring>* _pFolderList = nullptr);
	// File Name
	HRESULT Load_FirstFolder(const _tchar* _pFindPath);
private:
	// 상위 폴더를 로드함
	HRESULT Load_UpperFolder(const _tchar* _pFirstFolderName, const _tchar* _pPath, FILEGROUP* _pFileGroup = nullptr);
private:
	HRESULT Clear_Loader(list<FILEGROUP*>& _lsFileGroup, FILEGROUP* _pFileGroup = nullptr);
private:
	// FileGroup
	FILEGROUP*			m_tFileGroup = nullptr;

	list<FILEDATA>  m_EmptyList;
public:
	// Free
	virtual void Free() override;
};

END