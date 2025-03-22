#include "EngineDefine.h"
#include "UShaderConstantBuffer.h"
#include "UDevice.h"
#include "UTableDescriptor.h"
#include "UMethod.h"
#include "UGameInstance.h"

UShaderConstantBuffer::UShaderConstantBuffer() :
	m_cpUploadBuffer{ NULL },
	m_cpCpuDescriptorHeap{ NULL },
	m_stD3DCpuDescriptorHandle{ NULL },
	m_iHandleIncrementSize{ 0 },
	m_pMapBuffer{ nullptr },
	m_iElementSize{ 0 },
	m_eCbvRegisterNumber{ CBV_REGISTER::CBV_END },
	m_iShaderConstantBufferIndex{ 0 },
	m_iShaderConstantBufferCpuLocation{ 0 },
	m_iElementNum{ 1 }
{
}

void UShaderConstantBuffer::Free()
{
}

HRESULT UShaderConstantBuffer::NativeConstruct(CSHPTRREF<UDevice> _spDevice, const CBV_REGISTER& _eReg, const _uint& _iSize, const _uint& _iNum)
{
	RETURN_CHECK(nullptr == _spDevice, E_FAIL);
	m_iElementSize = UMethod::CalcConstantBufferByteSize(_iSize);
	m_eCbvRegisterNumber = _eReg;
	m_iElementNum = _iNum;
	// Create Buffer
	{
		D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC Desc = CD3DX12_RESOURCE_DESC::Buffer(m_iElementSize * m_iElementNum);
		RETURN_CHECK_DXOBJECT(_spDevice->GetDV()->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE,
			&Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_cpUploadBuffer)), E_FAIL);
		// UploadBuffer
		m_cpUploadBuffer->Map(0, nullptr, (void**)&m_pMapBuffer);
	}
	// CreateView 
	{
		D3D12_DESCRIPTOR_HEAP_DESC cbvDesc{};
		ZeroMemory(&cbvDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
		cbvDesc.NumDescriptors = _iNum;
		cbvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		cbvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		RETURN_CHECK_DXOBJECT(_spDevice->GetDV()->CreateDescriptorHeap(&cbvDesc, IID_PPV_ARGS(&m_cpCpuDescriptorHeap)), E_FAIL);
		// CpuStart
		m_stD3DCpuDescriptorHandle = m_cpCpuDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		// IncreamentSize
		m_iHandleIncrementSize = _spDevice->GetDV()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		for (_uint i = 0; i < m_iElementNum; ++i)
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC bufferViewDesc{};
			// 버퍼가 할당된 공간을 지정한다. 
			bufferViewDesc.BufferLocation = m_cpUploadBuffer->GetGPUVirtualAddress() +
				UINT64(i * m_iElementSize);
			bufferViewDesc.SizeInBytes = m_iElementSize;
			D3D12_CPU_DESCRIPTOR_HANDLE cbvHandle = GetCpuDescriptorHandle(i);
			_spDevice->GetDV()->CreateConstantBufferView(&bufferViewDesc, cbvHandle);
		}
	}
	return S_OK;
}

// CPU 공간에 할당되어 있는 DescriptorHeap에 Buffer를 Upload 한다. 
HRESULT UShaderConstantBuffer::PushData(CSHPTRREF< UTableDescriptor> _spTableDescriptor, const void* _pBuffer, const _uint& _iSize)
{
	UMethod::CheckOverNumbers(m_iShaderConstantBufferIndex, m_iElementNum);
	// 현재 인덱싱 된 사이즈 만큼 
	_uint iIndex = m_iShaderConstantBufferIndex * m_iElementSize;
	::memcpy(&m_pMapBuffer[iIndex], _pBuffer, _iSize);
	_spTableDescriptor->BindCBV(GetCpuDescriptorHandle(m_iShaderConstantBufferIndex), m_eCbvRegisterNumber);
	++m_iShaderConstantBufferIndex;

	return S_OK;
}

D3D12_CPU_DESCRIPTOR_HANDLE UShaderConstantBuffer::GetCpuDescriptorHandle(const _uint& _iIndex)
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_stD3DCpuDescriptorHandle, m_iHandleIncrementSize * _iIndex);
}
