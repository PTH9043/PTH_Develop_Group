#pragma once
#include "ImGuiClass_Tool.h"

BEGIN(Tool)

class CFilePathViewer_Tool : public CImGuiClass_Tool
{
private:
	explicit CFilePathViewer_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CFilePathViewer_Tool() = default;

public:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT Tick(const _double& _dTimeDelta)  override;
	virtual HRESULT Late_Tick(const _double& _dTimeDelta)  override;

protected:
	// Show Window
	virtual void Show_Window() override;
	// Load File
	void Create_Bin(FILEGROUP* _pFileGroup, const _wstring& _strAnimModelFolderName, const _wstring& _strModelFolderName,
		const _wstring& _FBXModelFolderName, const _wstring& _strConvertModelFolderName, const _wstring& _strTextureName);
protected:
	// Show Path view
	void PathView();
	//Folder View
	void FolderView();
	// File View
	void FileView();
	// Show Folder And File (Noe Find)
	void Show_FolderAndFile_NotFind(FILEGROUP* _pFileGroup, _uint* _pIndex = nullptr, const _bool& _bFindFolder = false);
	// Show FIle Folder And File
	void Find_Folder(FILEGROUP* _pFileGroup);
	// Find FileData
	void Find_FileData(FILEGROUP* _pFileGroup);
protected:
	void Open_Folder();
	void Open_File();
private:
	FILEGROUP*				m_pFileGroup = nullptr;
	// Select
	FILEGROUP*				m_pSelectFileGroup = nullptr;
	FILEDATA*					m_pSelectFileData = nullptr;
	// Find Name
	_string						m_strFindFolderFileName = "";
	// FInd 
	list<FILEGROUP*>	m_FindFolderList;
	list<FILEDATA*>		m_FindFileDataList;
	// Init Setting
	_uint							m_iSelectIndex = 0;
	_bool							m_bInitSetting = false;
private:
	DOCKDESC				m_tMainView;
	DOCKDESC				m_tPathView;
	DOCKDESC				m_tFolderView;
	DOCKDESC				m_tFileView;
public:
	static CFilePathViewer_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END