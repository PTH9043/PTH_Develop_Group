#pragma once
#include "UResource.h"

BEGIN(Engine)
class UTexture;
class UShader;

class UTexGroup : public UResource{
		using TEXTURECONTAINER = VECTOR<SHPTR<UTexture>>;
		using TEXNAMES = UNORMAP<_wstring, _uint>;
public:
	UTexGroup(CSHPTRREF<UDevice> _spDevice);
	UTexGroup(const UTexGroup& _rhs);
	DESTRUCTOR(UTexGroup)
public:
	const TEXTURECONTAINER& GetTextureContainer() const { return m_vecTextureContainer; }
	const TEXNAMES& GetTextureNames() const { return m_TextureNames; }
	SHPTR<UTexture> GetTexture(const _wstring& _wstrFileName);
	SHPTR<UTexture> GetTexture(const _uint& _iIndex);

	const _uint GetTextureCount() const { return static_cast<_uint>(m_vecTextureContainer.size()); }
public:
	// UResource을(를) 통해 상속됨
	CLONE_MACRO(UTexGroup, "UTexGroup::Clone To Failed");
	virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	HRESULT NativeConstruct(const _wstring& _wstrPath, const _bool& _isUnderLoadFolder = false,
		const _bool _isAllLoadFolder = false, const TEXTURECREATETYPE _eTextureCreateType = TEXTURECREATETYPE::TEX2D);
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _datas) override;
	// SetUp Texture
	HRESULT SetUpTextureIndex(CSHPTRREF<UShader> _spShader, const SRV_REGISTER& _eReg, const _uint _iIndex);
	HRESULT SetUpTextureName(CSHPTRREF<UShader> _spShader, const SRV_REGISTER& _eReg, const _wstring& _wstrName);
	// Get name
	const _wstring& GetTextureName(const _uint _iIndex);
	const _uint GetTextureIndex(const _wstring& _wstrTextureName);
private:
	HRESULT LoadTexture(const _wstring& _wstrPath, const _bool& _isUnderLoadFolder, const _bool _isAllLoadFolder
	, const TEXTURECREATETYPE _eTextureCreateType, DirectX::ResourceUploadBatch* _pResourceUploader);
	void LoadFolder(const _wstring& _wstrPath, const _bool& _isUnderLoadFolder, const _bool _isAllLoadFolder,
		const TEXTURECREATETYPE _eTextureCreateType, DirectX::ResourceUploadBatch* _pResourceUploader);
	void CreateTexture(const _wstring& _wstrPath, const TEXTURECREATETYPE _eTextureCreateType, DirectX::ResourceUploadBatch* _pResourceUploader);

private:
	TEXTURECONTAINER		m_vecTextureContainer;
	TEXNAMES						m_TextureNames;

	const	static	_wstring		EMPTY_STRING;
};

END
