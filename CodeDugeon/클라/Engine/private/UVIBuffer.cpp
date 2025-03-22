#include "EngineDefine.h"
#include "UVIBuffer.h"
#include "UShader.h"
#include "UMethod.h"
#include "UDevice.h"
#include "UGpuCommand.h"
#include "UGameInstance.h"
#include <DirectXTK/ResourceUploadBatch.h>

UVIBuffer::UVIBuffer(CSHPTRREF<UDevice> _spDevice, const VISPACE_TYPE _eSpace, const VIINSTANCE_TYPE _eInstance,
	const VIBUFFERTYPE& _eBufferType) :
	UResource(_spDevice),
	m_spPosVector{ nullptr },
	m_pIndices{ nullptr },
	m_cpVertexGpuBuffer{ nullptr },
	m_cpIndexGpuBuffer{ nullptr },
	m_cpVertexUploadBuffer{ nullptr },
	m_cpIndexUploadBuffer{ nullptr },
	m_stD3DVertexBufferView{ NULL },
	m_iVertexCnt{ 0 },
	m_stD3DIndexBufferView{ NULL },
	m_iIndexCnt{ 0 },
	m_iIndexSize{ 0 },
	m_ePrimitiveTopology{ D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_UNDEFINED },
	m_eVISpaceType{ _eSpace },
	m_eVIInstanceType{ _eInstance },
	m_vMinVertex{ 0.f, 0.f, 0.f },
	m_vMaxVertex{ 0.f, 0.f, 0.f },
	m_eVIBufferType{ _eBufferType }
{
}

UVIBuffer::UVIBuffer(const UVIBuffer& _rhs) :
	UResource(_rhs),
	m_spPosVector{ _rhs.m_spPosVector },
	m_pIndices{ _rhs.m_pIndices },
	m_cpVertexGpuBuffer{ _rhs.m_cpVertexGpuBuffer },
	m_cpIndexGpuBuffer{ _rhs.m_cpIndexGpuBuffer },
	m_cpVertexUploadBuffer{ nullptr },
	m_cpIndexUploadBuffer{ nullptr },
	m_stD3DVertexBufferView{ _rhs.m_stD3DVertexBufferView },
	m_iVertexCnt{ _rhs.m_iVertexCnt },
	m_stD3DIndexBufferView{ _rhs.m_stD3DIndexBufferView },
	m_iIndexCnt{ _rhs.m_iIndexCnt },
	m_iIndexSize{ _rhs.m_iIndexSize },
	m_ePrimitiveTopology{ _rhs.m_ePrimitiveTopology },
	m_eVISpaceType{ _rhs.m_eVISpaceType },
	m_eVIInstanceType{ _rhs.m_eVIInstanceType },
	m_vMinVertex{ _rhs.m_vMinVertex },
	m_vMaxVertex{ _rhs.m_vMaxVertex },
	m_eVIBufferType{ _rhs.m_eVIBufferType }
{
}

void UVIBuffer::Free()
{
	if (false == IsClone())
	{
		Make::ReleaseBuffer(m_pIndices);
	}
}

HRESULT UVIBuffer::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	return S_OK;
}

HRESULT UVIBuffer::NativeConstructClone(const VOIDDATAS& _stDatas)
{
	return __super::NativeConstructClone(_stDatas);
}

HRESULT UVIBuffer::Render(CSHPTRREF<UShader> _spShader, CSHPTRREF<UCommand> _spCommand, const _uint& _iInstanceCnt)
{
	RETURN_CHECK(nullptr == _spShader || nullptr == _spCommand, E_FAIL);
	_spShader->CommitLocalShaderDatas(_spCommand);
	BindVertexAndIndex(_spCommand->GetGpuCmdList(), _iInstanceCnt);
	return S_OK;
}

HRESULT UVIBuffer::CreateVtxBuffer(const _uint& _iVertexCnt, const _uint& _iBufferSize, const void* _pVertexData,
	const D3D_PRIMITIVE_TOPOLOGY& _eTopology, const POSVECTOR& _vecPosVector, const _bool _isComputeMinMaxPositon)
{
	const _uint BUFFER_SIZE = _iVertexCnt * _iBufferSize;
	//const _uint BUFFER_SIZE = ((_iBufferSize * _iVertexCnt + 255) & ~255);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	SHPTR<UGpuCommand> spGpuCommand = spGameInstance->GetGpuCommand();

	RETURN_CHECK_FAILED(UMethod::CreateBufferResource(GetDevice()->GetDV(),
		spGpuCommand->GetResCmdList().Get(), BUFFER_SIZE, _pVertexData,
		m_cpVertexGpuBuffer, m_cpVertexUploadBuffer, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER), E_FAIL);

	// 자원 동기화
	spGpuCommand->WaitForGpuResourceUpload();

	m_stD3DVertexBufferView.BufferLocation = m_cpVertexGpuBuffer->GetGPUVirtualAddress();
	m_stD3DVertexBufferView.SizeInBytes = BUFFER_SIZE;
	m_stD3DVertexBufferView.StrideInBytes = _iBufferSize;
	m_ePrimitiveTopology = _eTopology;

	
	m_spPosVector = std::make_shared<CPOSVECTOR>(_vecPosVector);
	m_iVertexCnt = _iVertexCnt;

	if (true == _isComputeMinMaxPositon)
	{
		ComputeMinMaxPosition();
	}

	return S_OK;
}
HRESULT UVIBuffer::CreateVtxBuffer(const _uint& _iVertexCnt, const _uint& _iBufferSize, const void* _pVertexData,
	const D3D_PRIMITIVE_TOPOLOGY& _eTopology)
{
	const _uint BUFFER_SIZE = _iVertexCnt * _iBufferSize;
	//const _uint BUFFER_SIZE = ((_iBufferSize * _iVertexCnt + 255) & ~255);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	SHPTR<UGpuCommand> spGpuCommand = spGameInstance->GetGpuCommand();

	RETURN_CHECK_FAILED(UMethod::CreateBufferResource(GetDevice()->GetDV(),
		spGpuCommand->GetResCmdList().Get(), BUFFER_SIZE, _pVertexData,
		m_cpVertexGpuBuffer, m_cpVertexUploadBuffer, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER), E_FAIL);

	// 자원 동기화
	spGpuCommand->WaitForGpuResourceUpload();

	m_stD3DVertexBufferView.BufferLocation = m_cpVertexGpuBuffer->GetGPUVirtualAddress();
	m_stD3DVertexBufferView.SizeInBytes = BUFFER_SIZE;
	m_stD3DVertexBufferView.StrideInBytes = _iBufferSize;
	m_ePrimitiveTopology = _eTopology;




	return S_OK;
}
HRESULT UVIBuffer::CreateVtxBufferWithNoData(const _uint& _iVertexCnt, const _uint& _iBufferSize, const void* _pVertexData,
	const D3D_PRIMITIVE_TOPOLOGY& _eTopology, const  D3D12_HEAP_TYPE& d3dHeapType, const D3D12_RESOURCE_STATES& d3dResourceStates)
{
	//const _uint BUFFER_SIZE = _iVertexCnt * _iBufferSize;
	const _uint BUFFER_SIZE = ((_iBufferSize * _iVertexCnt + 255) & ~255);

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	SHPTR<UGpuCommand> spGpuCommand = spGameInstance->GetGpuCommand();

	RETURN_CHECK_FAILED(UMethod::CreateTextureResource(GetDevice()->GetDV(),
		spGpuCommand->GetResCmdList().Get(), BUFFER_SIZE, _pVertexData,
		m_cpVertexGpuBuffer, nullptr, d3dHeapType, d3dResourceStates), E_FAIL);

	// 자원 동기화
	spGpuCommand->WaitForGpuResourceUpload();


	m_ePrimitiveTopology = _eTopology;

	return S_OK;
}

HRESULT UVIBuffer::CreateIndexBuffer(const _uint& _iIndexCnt, const _uint& _iBufferSize, const void* _pIndexData,
	const DXGI_FORMAT& _eIndexFormat, const _uint _iIndexMultiple)
{
	const _uint BUFFER_SIZE = _iBufferSize * _iIndexCnt;

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	SHPTR<UGpuCommand> spGpuCommand = spGameInstance->GetGpuCommand();

	RETURN_CHECK_FAILED(UMethod::CreateBufferResource(GetDevice()->GetDV(),
		spGpuCommand->GetResCmdList().Get(), BUFFER_SIZE, _pIndexData,
		m_cpIndexGpuBuffer, m_cpIndexUploadBuffer, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER), E_FAIL);

	spGpuCommand->WaitForGpuResourceUpload();

	m_stD3DIndexBufferView.BufferLocation = m_cpIndexGpuBuffer->GetGPUVirtualAddress();
	m_stD3DIndexBufferView.SizeInBytes = BUFFER_SIZE;
	m_stD3DIndexBufferView.Format = _eIndexFormat;

	m_iIndexCnt = _iIndexCnt;
	m_iIndexSize = m_iIndexCnt * _iIndexMultiple;
	m_pIndices = _pIndexData;

	return S_OK;
}

void UVIBuffer::BindVertexAndIndex(const ComPtr<Dx12GraphicsCommandList>& _cpGraphicCmdList, const _uint& _iInstanceCnt)
{
	// TODO
	// Buffer에다 세팅
	// Buffer의 주소를 Register에 전송 
	const ComPtr<Dx12GraphicsCommandList>& pGraphicCmdList = _cpGraphicCmdList;
	pGraphicCmdList->IASetPrimitiveTopology(m_ePrimitiveTopology);
	// Vertex
	{
		pGraphicCmdList->IASetVertexBuffers(0, 1, &m_stD3DVertexBufferView);
	}
	if (m_cpIndexGpuBuffer != nullptr)
	{
		// Index
		{
			pGraphicCmdList->IASetIndexBuffer(&m_stD3DIndexBufferView);
		}
		pGraphicCmdList->DrawIndexedInstanced(m_iIndexSize, _iInstanceCnt, 0, 0, 0);
	}
	else
	{
		pGraphicCmdList->DrawInstanced(m_iVertexCnt, 1, 0, 0);
	}
}

void UVIBuffer::ComputeMinMaxPosition()
{
	if (nullptr != m_spPosVector)
	{
		for (auto& iter : *m_spPosVector)
		{
			m_vMinVertex = _float3{ min(m_vMinVertex.x, iter.x), min(m_vMinVertex.y, iter.y), min(m_vMinVertex.z, iter.z) };
			m_vMaxVertex = _float3{ max(m_vMaxVertex.x, iter.x), max(m_vMaxVertex.y, iter.y), max(m_vMaxVertex.z, iter.z) };
		}
	}
}
