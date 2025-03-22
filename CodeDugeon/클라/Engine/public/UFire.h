#pragma once
#include "UPawn.h"

BEGIN(Engine)

class UTexGroup;
class UVIBufferRect;
class UShaderConstantBuffer;
class UFire : public UPawn {
public:
	struct FIREDESC
	{
		_wstring						 wstrFireShader{ L"" };
	};
public:
	using TEXTUREINDEXCONTAINER = UNORMAP<_uint, SRV_REGISTER>;
public:
	UFire(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	UFire(const UFire& _rhs);
	DESTRUCTOR(UFire)
public:
	CLONE_MACRO(UFire, "UFire::Clone To Failed")
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
	CSHPTRREF<UTexGroup> GetColorTextureGroup() const { return m_spFireColorTexGroup; }
	CSHPTRREF<UTexGroup> GetNoiseTextureGroup() const { return m_spFireNoiseTexGroup; }
	CSHPTRREF<UTexGroup> GetAlphaTextureGroup() const { return m_spFireAlphaTexGroup; }

	void SetColorTexture(const _wstring& TexName);
	void SetColorTexture(_uint _index);
	void SetNoiseTexture(const _wstring& TexName);
	void SetNoiseTexture(_uint _index);
	void SetAlphaTexture(const _wstring& TexName);
	void SetAlphaTexture(_uint _index);

	FIRENOISEBUFFER* GetFireNoiseBuffer() { return &m_stFireNoiseBuffer; }
	FIREDISTORTIONBUFFER* GetFireDistortionBuffer() { return &m_stFireDistortionBuffer; }
#ifdef _USE_IMGUI
public:
	virtual void ShowObjectInfo() override;
#endif
protected:
	SHPTR<UTexGroup>				m_spFireColorTexGroup;
	SHPTR<UTexGroup>				m_spFireNoiseTexGroup;
	SHPTR<UTexGroup>				m_spFireAlphaTexGroup;
	SHPTR<UVIBufferRect>		m_spVIBufferRect;
	
	SHPTR< UShaderConstantBuffer>				m_spShaderFireNoiseBuffer;
	SHPTR< UShaderConstantBuffer>				m_spShaderDistortionBuffer;

	FIRENOISEBUFFER					m_stFireNoiseBuffer;
	FIREDISTORTIONBUFFER		m_stFireDistortionBuffer;
	
	// 세 가지 다른 노이즈 텍스처에 대해 세 가지 스크롤 속도를 설정합니다.
	_float3 scrollSpeeds = _float3(1.3f, 2.1f, 2.3f);

	// 세 개의 서로 다른 노이즈 옥타브 텍스처를 만드는 데 사용할 세 개의 스케일을 설정합니다.
	_float3 scales = _float3(1.0f, 2.0f, 3.0f);

	// 세 가지 다른 노이즈 텍스처에 대해 세 가지 다른 x 및 y 왜곡 인수를 설정합니다.
	_float2 distortion1 = _float2(0.1f, 0.2f);
	_float2 distortion2 = _float2(0.1f, 0.3f);
	_float2 distortion3 = _float2(0.1f, 0.1f);

	// 텍스처 좌표 샘플링 섭동의 스케일과 바이어스.
	float distortionScale = 0.8f;
	float distortionBias = 0.5f;



	_uint ColorTextureIndex = 0; 
	_uint NoiseTextureIndex = 0;
	_uint AlphaTextureIndex = 0;
};

END

