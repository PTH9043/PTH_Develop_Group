#include "EngineDefine.h"
#include "UShaderStructedBuffer.h"
#include "UGameInstance.h"
#include "UDevice.h"
#include "UTableDescriptor.h"


UShaderStructedBuffer::UShaderStructedBuffer() :
	m_iElementSize{ 0 },
	m_iElementCount{ 0 },
	m_stD3DResourceState{},
	m_cpBufferResource{ nullptr },
	m_cpSrvDescriptorHeap{ nullptr },
	m_cpUavDescriptorHeap{ nullptr },
	m_stD3DSrvHeapBegin{},
	m_stD3DUavHeapBegin{}
{
}

void UShaderStructedBuffer::Free()
{
}

HRESULT UShaderStructedBuffer::NativeConstruct(CSHPTRREF<UDevice> _spDevice, const _uint _iElementSize, const _uint _iElementCnt)
{
	RETURN_CHECK(nullptr == _spDevice, E_FAIL);
	m_iElementSize = _iElementSize;
	m_iElementCount = _iElementCnt;

	m_stD3DResourceState = D3D12_RESOURCE_STATE_COMMON;
	// UAV ¸¸µê 
	{
		_ulong BufferSize{ static_cast<_ulong>(_iElementCnt * _iElementSize) };
		D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(BufferSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
		D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		RETURN_CHECK_DXOBJECT(_spDevice->GetDV()->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
			&desc, m_stD3DResourceState, nullptr, IID_PPV_ARGS(&m_cpBufferResource)), E_FAIL);
	}

	// SRV
	{
		D3D12_DESCRIPTOR_HEAP_DESC SrvDesc;
		ZeroMemory(&SrvDesc, sizeof(SrvDesc));
		SrvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		SrvDesc.NumDescriptors = 1;
		SrvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		RETURN_CHECK_DXOBJECT(_spDevice->GetDV()->CreateDescriptorHeap(&SrvDesc, IID_PPV_ARGS(&m_cpSrvDescriptorHeap)), E_FAIL);

		D3D12_SHADER_RESOURCE_VIEW_DESC SrvHeapDesc;
		ZeroMemory(&SrvHeapDesc, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
		SrvHeapDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SrvHeapDesc.Format = DXGI_FORMAT_UNKNOWN;
		SrvHeapDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		SrvHeapDesc.Buffer.FirstElement = 0;
		SrvHeapDesc.Buffer.NumElements = m_iElementCount;
		SrvHeapDesc.Buffer.StructureByteStride = _iElementSize;
		SrvHeapDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		m_stD3DSrvHeapBegin = m_cpSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		_spDevice->GetDV()->CreateShaderResourceView(m_cpBufferResource.Get(), &SrvHeapDesc, m_stD3DSrvHeapBegin);
	}
	// UAV
	{
		D3D12_DESCRIPTOR_HEAP_DESC UavDesc;
		ZeroMemory(&UavDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
		UavDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		UavDesc.NumDescriptors = 1;
		UavDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		RETURN_CHECK_DXOBJECT(_spDevice->GetDV()->CreateDescriptorHeap(&UavDesc, IID_PPV_ARGS(&m_cpUavDescriptorHeap)), E_FAIL);

		D3D12_UNORDERED_ACCESS_VIEW_DESC UavHeapDesc;
		ZeroMemory(&UavHeapDesc, sizeof(D3D12_UNORDERED_ACCESS_VIEW_DESC));
		UavHeapDesc.Format = DXGI_FORMAT_UNKNOWN;
		UavHeapDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		UavHeapDesc.Buffer.FirstElement = 0;
		UavHeapDesc.Buffer.NumElements = m_iElementCount;
		UavHeapDesc.Buffer.StructureByteStride = _iElementSize;
		UavHeapDesc.Buffer.CounterOffsetInBytes = 0;
		UavHeapDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		m_stD3DUavHeapBegin = m_cpUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		_spDevice->GetDV()->CreateUnorderedAccessView(m_cpBufferResource.Get(), nullptr, &UavHeapDesc, m_stD3DUavHeapBegin);
	}

	return S_OK;
}

HRESULT UShaderStructedBuffer::NativeConstruct(CSHPTRREF<UDevice> _spDevice, const size_t _iElementSize, const _uint _iElementCnt)
{
	return NativeConstruct(_spDevice, static_cast<_uint>(_iElementSize), _iElementCnt);
}

void UShaderStructedBuffer::PushSRVData(CSHPTRREF<UTableDescriptor> _spTableDescriptor, const SRV_REGISTER& _eReg)
{
	RETURN_CHECK(nullptr == _spTableDescriptor, ;);
	_spTableDescriptor->BindSRV(m_stD3DSrvHeapBegin, _eReg);
}

void UShaderStructedBuffer::PushUAVData(CSHPTRREF<UTableDescriptor> _spTableDescriptor, const UAV_REGISTER& _eReg)
{
	RETURN_CHECK(nullptr == _spTableDescriptor, ;);
	_spTableDescriptor->BindUAV(m_stD3DUavHeapBegin, _eReg);
}
