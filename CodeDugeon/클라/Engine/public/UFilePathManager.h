#pragma once

#include "UBase.h"

BEGIN(Engine)

class UFilePathManager : public UBase {
public:
	UFilePathManager();
	NO_COPY(UFilePathManager)
		DESTRUCTOR(UFilePathManager)
public:
	CSHPTRREF<FILEGROUP> GetFirstFileGroup() const { return m_spFileGroup; }
public:
	virtual void Free() override;

	SHPTR<FILEGROUP> FindFolder(const PATHS& _vecFolderList);
	SHPTR<FILEGROUP>	FindFolder(const _wstring& _wstrFindName, const _wstring& _wstrParentsFolderName = L"");
	VECTOR<SHPTR<FILEGROUP>> FindSameAllFolder(const _wstring& _wstrFindFolderName);

	HRESULT LoadFirstFolder(const _wstring& _wstrFilePath);
	HRESULT ClearLoader(LIST<SHPTR<FILEGROUP>>& _lsFileGroup, CSHPTRREF<FILEGROUP> _spFileGroup = nullptr);
private:
	HRESULT LoadUpperFolder(const _wstring& _wstrFirstFolderName, const _wstring& _wstrPath, CSHPTRREF<FILEGROUP> _spFileGroup = nullptr);
private:
	using FILEFONDERGROUP = UNORMAP<_wstring, VECTOR<SHPTR<FILEGROUP>>>;
	FILEFONDERGROUP			m_FileFolderGroup;
	SHPTR<FILEGROUP>		m_spFileGroup;
};

END