#pragma once
#include "UBase.h"

BEGIN(Engine)
class UGlobalConstantBuffer;
class UShaderConstantBuffer;
class UDevice;
class UCommand;

class UShaderBufferManager : public UBase {
	using GLOBALBUFFERCONTAINER = ARRAY<SHPTR<UGlobalConstantBuffer>, GLOBAL_BUFFER_END>;
	using PREALLOCATEDCBUFFERCONTANER = ARRAY<SHPTR<UShaderConstantBuffer>, PREALLOCATED_BUFFER_END>;
public:
	UShaderBufferManager();
	NO_COPY(UShaderBufferManager)
	DESTRUCTOR(UShaderBufferManager)
public:
	virtual void Free() override;
public:
	HRESULT ReadyShaderBufferManager(CSHPTRREF<UDevice> _spDevice);

	void	 BindGlobalBuffer(const GLOBAL_CBUFFERTYPE _eGlobalCBuffer, CSHPTRREF<UCommand> _spCommand, const void* _pBuffer, const _uint _iSize);
	HRESULT GetGlobalConstantBuffer(const GLOBAL_CBUFFERTYPE _eGlobalCBuffer, SHPTRREF<UGlobalConstantBuffer> _spGlobalConstantBuffer);
	HRESULT GetPreAllocatedConstantBuffer(const PREALLOCATED_CBUFFERTYPE _ePreAllocatedCBufferType, SHPTRREF<UShaderConstantBuffer> _spShaderConstantBuffer);
private:
	// CreateGlobalConstantBuffer
	HRESULT CreateGlobalConstantBuffer(CSHPTRREF<UDevice> _spDevice, const GLOBAL_CBUFFERTYPE _eType,
		const CBV_REGISTER& _eReg, const _uint& _iBufferSize);
	// CreateGlobalConstantBuffer
	HRESULT CreateShaderConstantBuffer(CSHPTRREF<UDevice> _spDevice, const PREALLOCATED_CBUFFERTYPE _eType,
		const CBV_REGISTER& _eReg, const _uint& _iBufferSize, const _uint _iCnt);
private:
	// GlobalBufferContainer
	GLOBALBUFFERCONTAINER						m_arrGlobalConstantBufferContainer;
	PREALLOCATEDCBUFFERCONTANER		m_arrPreAllocatedCBufferContainer;
};

END

