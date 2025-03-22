#pragma once
#include "UResource.h"

BEGIN(Engine)
class UTableDescriptor;
class UTexture;
class UActor;
class UShaderConstantBuffer;
class URootSignature;
class UShaderStructedBuffer;
/*
모든 쉐이더 Param들을 바인드할 때는 무조건 Shader 클래스를 거쳐서 바인드한다.
반드시 따로 값을 전달하는 경우는 없도록 한다.

쉐이더 파일은 복제 되는 일이 없고 원복 객체를 그대로 리턴하도록 되어 있다.
*/
class UShader : public UResource{
	// Shader Container
	using SHADERCONTAINER = UNORMAP<SHADERMAININFO, ComPtr<DxBlob>>;
public:
	UShader(CSHPTRREF<UDevice> _pDevice, CSHPTRREF<URootSignature> _spRootSignature, const SHADERDESC& _tDesc);
	UShader(const UShader& _rhs);
	DESTRUCTOR(UShader)
public:
	const SHADERDESC& GetShaderDesc() const { return m_stShaderDesc; }
	CSHPTRREF<UCommand> GetCommand() const { return m_spCommand; }
	CSHPTRREF<UTableDescriptor> GetTableDescriptor() const { return m_spTableDescriptor; }
	CSHPTRREF<URootSignature> GetRootSignature() const { return m_spRootSignature; }
	const SHADERCONTAINER& GetShaderContainer() const { return m_ShaderBlopContainer; }
public:
	virtual void SetTableDescriptor(CSHPTRREF<UTableDescriptor> _spTableDescriptor) { this->m_spTableDescriptor = _spTableDescriptor; }
public:
	CLONE_MACRO(UShader, "ShaderCloneToFailed")
	virtual void				Free() override;
	// Native
	virtual HRESULT	NativeConstruct() override;
	virtual HRESULT	NativeConstructClone(const VOIDDATAS& _stDatas) override;
	// Bind Buffer
	HRESULT				BindSRVBuffer(const SRV_REGISTER _eReg, CSHPTRREF<UTexture> _spTexture);
	HRESULT				BindCBVBuffer(CSHPTRREF<UShaderConstantBuffer> _spShaderConstantBuffer,
		const void* _pBuffer, const _uint _iSize);
	HRESULT				BindSRVBuffer(const SRV_REGISTER _eReg, CSHPTRREF<UShaderStructedBuffer> _spStructedBuffer);

	 void						BeginSettingDatas(CSHPTRREF<UCommand> _spCommand);
	 void						SettingPipeLineState(CSHPTRREF<UCommand> _spCommand);
	 void						CommitLocalShaderDatas(CSHPTRREF<UCommand> _spCommand);
protected:
	virtual HRESULT CreateShader();
	void InsertShaderBlopContainer(const SHADERMAININFO _eInfo, const ComPtr<DxBlob>& _blob);
	const _wstring GetShaderType(const SHADERMAININFO& _eShaderType);

	D3D12_PRIMITIVE_TOPOLOGY_TYPE GetToplologyType(const D3D12_PRIMITIVE_TOPOLOGY _eTopology);

private:
	using PIPELINECONTAINER = UNORMAP<_wstring, ComPtr<Dx12PipelineState>>;
	static PIPELINECONTAINER					s_m_PipeLineContainer;

	SHADERCONTAINER								m_ShaderBlopContainer;
	SHPTR<UCommand>								m_spCommand;
	SHPTR<UTableDescriptor>						m_spTableDescriptor;
	SHPTR<URootSignature>						m_spRootSignature;
	SHADERDESC									m_stShaderDesc;
};

END