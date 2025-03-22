#pragma once
#include "UBase.h"

BEGIN(Engine)
class UDevice;
class UGpuCommand;
class UTableDescriptor;

/*
각 쉐이더마다 값이 달라야 하는 Buffer들을 ShaderFile에 Bind하기 위한 클래스로
각 쉐이더에 전달하기 위해서는 반드시 해당 객체를 한 번 생성하거나 생성해 놓은 값을 이용한다.

자주 사용하는 값들은 _iNum 값을 1이상 주어 미리 할당 해 놓은 Buffer를 사용하고
자주 사용하지 않고 많이 할당 되지도 않는 것들은 그때 그때마다 ShaderConstantBuffer를 할당한다.
*/
class UShaderConstantBuffer final : public UBase{
public:
	UShaderConstantBuffer();
	NO_COPY(UShaderConstantBuffer)
	DESTRUCTOR(UShaderConstantBuffer)
public:
	virtual void Free() override;
	HRESULT NativeConstruct(CSHPTRREF<UDevice> _spDevice, 	const CBV_REGISTER& _eReg,   const _uint& _iSize, const _uint& _iNum = 1);
	// Shader에 값을 밀어 넣는 함수이다. 
	HRESULT PushData(CSHPTRREF< UTableDescriptor> _spTableDescriptor, const void* _pBuffer, const _uint& _iSize);
private:
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuDescriptorHandle(const _uint& _iIndex);
private:
	// Use Upload
	ComPtr<Dx12Resource>						m_cpUploadBuffer;
	// Descriptor Heap
	ComPtr<Dx12DescriptorHeap>			m_cpCpuDescriptorHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE	m_stD3DCpuDescriptorHandle;
	_int																m_iHandleIncrementSize;

	BYTE* m_pMapBuffer;
	_uint															m_iElementSize;
	// CBV_REGISTER
	CBV_REGISTER										m_eCbvRegisterNumber;
	// elementIndex
	_uint															m_iShaderConstantBufferIndex;
	_int																m_iShaderConstantBufferCpuLocation;
	_uint															m_iElementNum;
};

END