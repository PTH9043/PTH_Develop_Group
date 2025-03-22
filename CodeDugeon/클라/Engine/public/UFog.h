#pragma once
#include "UPawn.h"

BEGIN(Engine)

class UTexGroup;
class UVIBufferCube;
class UShaderConstantBuffer;
class UFog : public UPawn {
public:
	struct FOGDESC
	{
		_wstring						 wstrFogShader{ L"" };
	};
public:
	using TEXTUREINDEXCONTAINER = UNORMAP<_uint, SRV_REGISTER>;
public:
	UFog(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	UFog(const UFog& _rhs);
	DESTRUCTOR(UFog)
public:
	CLONE_MACRO(UFog, "UFog::Clone To Failed")
		virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _convecDatas) override;
	void Update(const _double& _dTimeDelta);
protected:
	// Tick, LateTick, Render
	virtual void TickActive(const _double& _dTimeDelta) override;
	virtual void LateTickActive(const _double& _dTimeDelta) override;
	virtual HRESULT RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass = true) override;
	virtual void BindShaderBuffer();
	// Damaged
	virtual void Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta) override;
	// Save Load
	virtual _bool Save(const _wstring& _wstrPath);
	virtual _bool Load(const _wstring& _wstrPath);
public:
	CSHPTRREF<UTexGroup> GetColorTextureGroup() const { return m_spFogTexGroup; }
	FOGBUFFER* GetFogBuffer() {
		return &m_stFogBuffer;
	}
#ifdef _USE_IMGUI
public:
	void SetColorTexture(const _wstring& TexName);
	void SetColorTexture(_uint _index);
	virtual void ShowObjectInfo() override;

#endif
protected:
	SHPTR<UTexGroup>				m_spFogTexGroup;

	SHPTR<UVIBufferCube>		m_spVIBufferCube;

	SHPTR< UShaderConstantBuffer>				m_spShaderFogBuffer;
	

	FOGBUFFER					m_stFogBuffer;
	

	_float fogStart = -50.0f;
	_float fogEnd = 50.0f;
	_float4 fogColor = _float4(0.5,0.5,0.5,1.f);
	_float fogDensity = 0.5f;
	
	_uint ColorTextureIndex = 0;
};

END

