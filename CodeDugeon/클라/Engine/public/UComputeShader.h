#pragma once
#include "UShader.h"

BEGIN(Engine)
class UTableDescriptor;

class UComputeShader : public UShader{
public:
	UComputeShader(CSHPTRREF<UDevice> _pDevice,  CSHPTRREF<URootSignature> _spRootSignature, const SHADERDESC & _stDesc);
	UComputeShader(const UComputeShader& _rhs);
	DESTRUCTOR(UComputeShader)
public:
	virtual void SetTableDescriptor(CSHPTRREF<UTableDescriptor> _spTableDescriptor);
public:
	CLONE_MACRO(UComputeShader, "ComputeShaderCloneToFailed")
	virtual void				Free() override;
	// Native
	virtual HRESULT	NativeConstruct() override;
	virtual HRESULT	NativeConstructClone(const VOIDDATAS& _stDatas) override;

	HRESULT				BindUAVBuffer(const UAV_REGISTER _eReg, CSHPTRREF<UTexture> _spTexture);
	HRESULT				BindUAVBuffer(const UAV_REGISTER _eReg, CSHPTRREF<UShaderStructedBuffer> _spStructedBuffer);

	void							CommitLocalShaderDatas(CSHPTRREF<UCommand> _spCommand, const _uint& _iPosX, const _uint& _iPosY, const _uint& _iPosZ);
protected:
	virtual HRESULT CreateShader() override;
};

END