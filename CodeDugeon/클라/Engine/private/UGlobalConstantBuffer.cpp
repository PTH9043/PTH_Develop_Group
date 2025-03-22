#include "EngineDefine.h"
#include "UGlobalConstantBuffer.h"
#include "UDevice.h"
#include "UGpuCommand.h"
#include "UMethod.h"

UGlobalConstantBuffer::UGlobalConstantBuffer() :
	m_cpUploadBuffer{ NULL },
	m_cpCpuDescriptorHeap{ NULL },
	m_cpCpuDescriptorHandle{ NULL },
	m_iElementSize{ 0 },
	m_iCbvRegisterNumber{ 0 }
{
}

void UGlobalConstantBuffer::Free()
{
}

HRESULT UGlobalConstantBuffer::NativeConstruct(CSHPTRREF<UDevice> _spDevice, const CBV_REGISTER& _eReg, const _uint& _iSize)
{
	RETURN_CHECK(nullptr == _spDevice, E_FAIL);
	m_iElementSize = UMethod::CalcConstantBufferByteSize(_iSize);
	m_iCbvRegisterNumber = (_uint)(_eReg);
	// Create Buffer
	{
		D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC Desc = CD3DX12_RESOURCE_DESC::Buffer(m_iElementSize);
		RETURN_CHECK_DXOBJECT(_spDevice->GetDV()->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE,
			&Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_cpUploadBuffer)), E_FAIL);
		// UploadBuffer
		m_cpUploadBuffer->Map(0, nullptr, (void**)&m_pMapBuffer);
	}
	// CreateView 
	{
		D3D12_DESCRIPTOR_HEAP_DESC cbvDesc{};
		ZeroMemory(&cbvDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
		cbvDesc.NumDescriptors = 1;
		cbvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		cbvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		RETURN_CHECK_DXOBJECT(_spDevice->GetDV()->CreateDescriptorHeap(&cbvDesc, IID_PPV_ARGS(&m_cpCpuDescriptorHeap)), E_FAIL);
		// CpuStart
		m_cpCpuDescriptorHandle = m_cpCpuDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		D3D12_CPU_DESCRIPTOR_HANDLE cbvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_cpCpuDescriptorHandle, 0);
		D3D12_CONSTANT_BUFFER_VIEW_DESC bufferViewDesc{};
		bufferViewDesc.BufferLocation = m_cpUploadBuffer->GetGPUVirtualAddress();
		bufferViewDesc.SizeInBytes = m_iElementSize;
		_spDevice->GetDV()->CreateConstantBufferView(&bufferViewDesc, cbvHandle);
	}
	return S_OK;
}

void UGlobalConstantBuffer::SettingGlobalData(CSHPTRREF<UCommand> _spCommand, const void* _pBuffer, const _uint _iSize)
{
	RETURN_CHECK(nullptr == _spCommand, ;);
	RETURN_CHECK(m_iElementSize != ((_iSize + 255) & ~255), ;);
	::memcpy(&m_pMapBuffer[0], _pBuffer, _iSize);
	_spCommand->GetGpuCmdList()->SetGraphicsRootConstantBufferView(m_iCbvRegisterNumber,
		m_cpUploadBuffer->GetGPUVirtualAddress());
}