#pragma once
#include "Base.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class CFilePath_Manager final : public CBase
{
	DECLARE_SINGLETON(CFilePath_Manager)
public:

protected:
	explicit CFilePath_Manager();
	virtual ~CFilePath_Manager() = default;
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
	// Open Save Path
	HRESULT Open_SavePath( _wstring* _pOutPath);
	// Open Folder
	HRESULT Open_SaveFolder(_wstring* _pOutPath);
	// Open Root Save Path
	HRESULT Open_FolderSelectRoot(const _wstring& _strPath, const _wstring& _strName, _wstring* _pOutPath);
private:
	// 상위 폴더를 로드함
	HRESULT Load_UpperFolder(const _tchar* _pFirstFolderName, const _tchar* _pPath, FILEGROUP* _pFileGroup = nullptr);
private:
	HRESULT Clear_Loader(list<FILEGROUP*>& _lsFileGroup, FILEGROUP* _pFileGroup = nullptr);
private:
	// FileGroup
	FILEGROUP* m_tFileGroup = nullptr;
	list<FILEDATA>  m_EmptyList;
public:
	// Free
	virtual void Free() override;
};

END