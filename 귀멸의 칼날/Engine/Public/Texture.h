#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
private:
	explicit CTexture(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext);
	explicit CTexture(const CTexture& _rhs);
	virtual ~CTexture() = default;
public:
	ID3D11ShaderResourceView* Get_CurTexture() { return m_pCurTexture; }
	ID3D11ShaderResourceView* Find_Texture(const _tchar* _pTextureName);
	ID3D11ShaderResourceView* Find_Texture(const _uint& _iIndex);
	vector<ID3D11ShaderResourceView*> Get_SRVS() { return m_SRVs; }
	map<_wstring, _uint> Get_TextureNames() { return m_TexNames; }
	_wstring Get_TextureName(const _uint& _iIndex);
	_uint Get_TextureSize() const { return (_uint)m_SRVs.size(); }
	FILEGROUP* Get_FileGroup() {return m_pFileGroup;}
public:
	// NativeConstruct Prototype _ FileDesc
	HRESULT NativeConstruct_ProtoType(FILEGROUP* _pFileDesc);
	// NativeConstruct ProtoType _ Folder Name
	HRESULT NativeConstruct_ProtoType(PATHS* _pFolderList);
	// NativeConstruct ProtoType _ StrPath
	HRESULT NativeConstruct_ProtoType(const _wstring& _strPath);
	// NativeConstruct Clone
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
public:
	// SetUp Constant Table
	HRESULT SetUp_ConstantTable_Index(class CShader* _pShader, const char* _pConstantName, const _uint& _iTextureIndex);
	HRESULT SetUp_ConstantTable_FileName(class CShader* _pShader, const char* _pConstantName, const _tchar* _pFileName);

private:
	void Load_Texture(FILEGROUP* _pFileGroup);
private:
	// Shader Resource View 
	typedef vector<ID3D11ShaderResourceView*> SRVS;
	SRVS m_SRVs;
	typedef map<_wstring, _uint> TEXNAMES;
	TEXNAMES m_TexNames;
	// CurTexture
	ID3D11ShaderResourceView* m_pCurTexture = nullptr;
private:
	// File Name
	wstring m_strFileName = L"";
	// Texture Index
	_uint	   m_iTextureIndex = 0;

	FILEGROUP*	m_pFileGroup = nullptr;
public:
	// Create
	static CTexture* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, FILEGROUP* _pFileDesc);
	static CTexture* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, PATHS* _pFolderList);
	static CTexture* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _wstring& _strPath);
	// Clone
	virtual CComponent* Clone(void* _pArg) override;
	// Free
	virtual void Free() override;
};


END