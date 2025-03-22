#pragma once

#include "UComponent.h"


BEGIN(Engine)
class UTexture;
class UTexGroup;
class UShader;

using BINDTEXTURECONTAINER = UNORMAP<SRV_REGISTER, SHPTR<UTexture>>;
using MATERIALTEXCONTAINER = VECTOR<SHPTR<UTexGroup>>;
/*
@ Date: 2024-04-10, Writer: 박태현
@ Explain
-  머터리얼에 대한 텍스처 컨테이너이다. 
*/
class UModelMaterial final : public UComponent {
public:
	struct DESC {
		MODELMATERIALINFO			MaterialInfo;
		MATERIALTEXCONTAINER		MaterialTexContainer;
	};
public:
	UModelMaterial(CSHPTRREF<UDevice> _spDevice, const DESC& _MaterialDesc);
	UModelMaterial(const UModelMaterial& _rhs);
	DESTRUCTOR(UModelMaterial)
public:
	// UObject을(를) 통해 상속됨
	CLONE_MACRO(UModelMaterial, "Material::Clone To Failed")
	virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _stDatas) override;

	void BindTexture(CSHPTRREF<UShader> _spShader, const SRV_REGISTER _srvRegister, TEXTYPE _TexType, _int _TextureOrder = 0);
	void BindTexture(CSHPTRREF<UShader> _spShader, const SRV_REGISTER _srvRegister, TEXTYPE _TexType, const _wstring& _wstrTextureName);

	void RegisterBindTextureContainer(const SRV_REGISTER _srvRegister, TEXTYPE _TexType, _int _TextureOrder = 0);
	void RegisterBindTextureContainer(const SRV_REGISTER _srvRegister, TEXTYPE _TexType, const _wstring& _wstrTextureName);
public: /* get set */
	const MODELMATERIALINFO& GetMaterialInfo() const  { return m_MaterialInfo; }
	const _int GetMaterialIndex() const { return m_iMaterialIndex; }
	_bool IsEmpty(TEXTYPE _TexType) const { return m_MaterialTexContainer[_TexType] == nullptr; }
private:
	// Material Draw Number 
	static _int									s_AllMaterialDrawLength;
	// 넘버링을 주기 위한 컨테이너
	static VECTOR<_int>				s_RemainMaterialDrawNumber;
	// 머터리얼 
	_int												m_iMaterialIndex;

	BINDTEXTURECONTAINER		m_BindTextureContainer;
	MATERIALTEXCONTAINER		m_MaterialTexContainer;
	MODELMATERIALINFO			m_MaterialInfo;
};


END
