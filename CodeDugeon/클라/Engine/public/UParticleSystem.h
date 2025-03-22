#pragma once

#include "UResource.h"

BEGIN(Engine)
class UShaderStructedBuffer;
class UShaderConstantBuffer;
class UComputeShader;
class UShader;
class UComputeCommand;
class UTableDescriptor;
class UCommand;

class UParticleSystem : public UResource{
public:
	UParticleSystem(CSHPTRREF<UDevice> _spDevice);
	UParticleSystem(const UParticleSystem& _rhs);
	DESTRUCTOR(UParticleSystem)
public:
	 const _uint GetMaxParticleCnt() { return m_iMaxParitcleCnt; }
public:
	CLONE_MACRO(UParticleSystem, "UParticleSystem::Clone To Failed")
	virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _convecDatas) override;
public:
	void Update(const _double& _dTimeDelta);
	void Render(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor);
	void BindShaderParams(CSHPTRREF<UShader> _spShader);
	// Bind Compute shader
	void SettingComputeShader(const _wstring& _wstrProtoName);

public:
	PARTICLEPARAM* GetParticleParam() {
		return &m_stParticleParam;
	}
	ComputeParticleType* GetParticleTypeParam() {
		return &m_stParticleType;
	}
	_uint* GetAddParticleAmount() {
		return &m_iParticleAddAmount;
	}
	_float* GetCreateInterval() {
		return &m_fCreateInterval;
	}
	

	_uint GetParticleType() {
		return m_stParticleParam.stGlobalParticleInfo.fParticleKind;
	}
	

private:
	// static 
	static constexpr _uint										PARTICLEPARAM_SIZE{sizeof(PARTICLEPARAM)};
	static constexpr _uint										PARTICLETYPEPARAM_SIZE{sizeof(ComputeParticleType)};
	//static constexpr _float									CREATE_INTERVAL{2.5f};
	static constexpr _ushort								COMPUTE_MAX_INDEX{3};
	using COMPUTECOMMANDCONTAINER = ARRAY<SHPTR<UComputeCommand>, COMPUTE_MAX_INDEX>;

	
	PARTICLEPARAM								m_stParticleParam;
	ComputeParticleType							m_stParticleType;
	SHPTR< UShaderConstantBuffer>				m_spComputeShaderParticleConstantBuffer;
	SHPTR< UShaderConstantBuffer>				m_spComputeShaderTypeConstantBuffer;
	SHPTR< UShaderConstantBuffer>				m_spGraphicsShaderParticleConstantBuffer;
	SHPTR<UShaderStructedBuffer>				m_spParticleStructedBuffer;
	SHPTR<UShaderStructedBuffer>				m_spParticleStructedBufferPlus;
	SHPTR<UShaderStructedBuffer>				m_spComputeShaderStructedBuffer;
	// ComputeShader
	SHPTR<UComputeShader>						m_spComputeShader;
//	COMPUTECOMMANDCONTAINER				m_arrComputeCommands;
	SHPTR<UComputeCommand>				m_spComputeCommand;
	SHPTR< UTableDescriptor>						m_spComputeTableDescriptor;
	
	_uint										m_iMaxParitcleCnt;
	_ushort										m_sComputeIndex;
	_uint										m_iParticleAddAmount;
	_float										m_fCreateInterval;
	_bool			m_fOffsetFactor;
	
};

END

