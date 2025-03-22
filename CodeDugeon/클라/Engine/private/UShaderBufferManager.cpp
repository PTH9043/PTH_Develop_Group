#include "EngineDefine.h"
#include "UShaderBufferManager.h"
#include "UGlobalConstantBuffer.h"
#include "UShaderConstantBuffer.h"

UShaderBufferManager::UShaderBufferManager()
{
}

void UShaderBufferManager::Free()
{
}

HRESULT UShaderBufferManager::ReadyShaderBufferManager(CSHPTRREF<UDevice> _spDevice)
{
	{
		RETURN_CHECK_FAILED(CreateGlobalConstantBuffer(_spDevice, GLOBAL_VIEWPROJ, CBV_REGISTER::VIEWPROJ, sizeof(VIEWPROJPARAM)), E_FAIL);
		RETURN_CHECK_FAILED(CreateGlobalConstantBuffer(_spDevice, GLOBAL_LIGHTSTATIC, CBV_REGISTER::STATICLIGHT, sizeof(LIGHTPARAM)), E_FAIL);
	}
	{
		RETURN_CHECK_FAILED(CreateShaderConstantBuffer(_spDevice, PREALLOCATED_TRANSFORM, CBV_REGISTER::TRANSFORM, sizeof(TRANSFORMPARAM), 50000), E_FAIL);
	}
	//RETURN_CHECK_FAILED(CreateGlobalConstantBuffer(_spDevice, GLOBAL_GLOBALDATA, CBV_REGISTER::GLOBALDATA, sizeof(GLOBALPARAM)), E_FAIL);
	return S_OK;
}

void UShaderBufferManager::BindGlobalBuffer(const GLOBAL_CBUFFERTYPE _eGlobalCBuffer, CSHPTRREF<UCommand> _spCommand, const void* _pBuffer, const _uint _iSize)
{
	RETURN_CHECK(m_arrGlobalConstantBufferContainer.size() <= _eGlobalCBuffer, );
	m_arrGlobalConstantBufferContainer[_eGlobalCBuffer]->SettingGlobalData(_spCommand, _pBuffer, _iSize);
}

HRESULT UShaderBufferManager::GetGlobalConstantBuffer(const GLOBAL_CBUFFERTYPE _eGlobalCBuffer, SHPTRREF<UGlobalConstantBuffer> _spGlobalConstantBuffer)
{
	RETURN_CHECK(m_arrGlobalConstantBufferContainer.size() <= _eGlobalCBuffer, E_FAIL);
	_spGlobalConstantBuffer = m_arrGlobalConstantBufferContainer[_eGlobalCBuffer];
	return S_OK;
}

HRESULT UShaderBufferManager::GetPreAllocatedConstantBuffer(const PREALLOCATED_CBUFFERTYPE _ePreAllocatedCBufferType, SHPTRREF<UShaderConstantBuffer> _spShaderConstantBuffer)
{
	RETURN_CHECK(m_arrPreAllocatedCBufferContainer.size() <= _ePreAllocatedCBufferType, E_FAIL);
	_spShaderConstantBuffer = m_arrPreAllocatedCBufferContainer[_ePreAllocatedCBufferType];
	return S_OK;
}

HRESULT UShaderBufferManager::CreateGlobalConstantBuffer(CSHPTRREF<UDevice> _spDevice, const GLOBAL_CBUFFERTYPE _eType, const CBV_REGISTER& _eReg, const _uint& _iBufferSize)
{
	_ubyte ByteInt = static_cast<_ubyte>(_eType);
	RETURN_CHECK(m_arrGlobalConstantBufferContainer.size() <= ByteInt || nullptr != m_arrGlobalConstantBufferContainer[ByteInt], E_FAIL);
	// Create GlobalBuffer 
	m_arrGlobalConstantBufferContainer[ByteInt] = CreateNative<UGlobalConstantBuffer>(_spDevice, _eReg, _iBufferSize);
	return S_OK;
}

HRESULT UShaderBufferManager::CreateShaderConstantBuffer(CSHPTRREF<UDevice> _spDevice, const PREALLOCATED_CBUFFERTYPE _eType, const CBV_REGISTER& _eReg, const _uint& _iBufferSize, const _uint _iCnt)
{
	_ubyte ByteInt = static_cast<_ubyte>(_eType);
	RETURN_CHECK(m_arrPreAllocatedCBufferContainer.size() <= ByteInt || nullptr != m_arrPreAllocatedCBufferContainer[ByteInt], E_FAIL);
	// Create GlobalBuffer 
	m_arrPreAllocatedCBufferContainer[ByteInt] = CreateNative<UShaderConstantBuffer>(_spDevice, _eReg, _iBufferSize, _iCnt);
	return S_OK;
}
