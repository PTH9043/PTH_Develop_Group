#pragma once
#include "UResource.h"

BEGIN(Engine)
class UShader;
class UGpuCommand;

enum class VISPACE_TYPE {
	SPACE_2D, SPACE_3D, SPACE_END
};
enum class VIINSTANCE_TYPE {
	SINGLE, INSTANCEING, INSTANCING_END
};


class UVIBuffer abstract : public UResource {
public:
	using POSVECTOR = VECTOR< _float3>;
	using CPOSVECTOR = const POSVECTOR;
public:
	UVIBuffer(CSHPTRREF <UDevice> _spDevice, const VISPACE_TYPE _eSpace,
		const VIINSTANCE_TYPE _eInstance = VIINSTANCE_TYPE::SINGLE, const VIBUFFERTYPE& _eBufferType = VIBUFFERTYPE::GENERIC);
	UVIBuffer(const UVIBuffer& _rhs);
	DESTRUCTOR(UVIBuffer)
public:
	/* Get Set */
	const _uint& GetVertexCnt() const { return m_iVertexCnt; }
	const DXGI_FORMAT& GetIndexFormat() { return m_stD3DIndexBufferView.Format; }
	const D3D_PRIMITIVE_TOPOLOGY& GetPrimitiveTopology() const { return m_ePrimitiveTopology; }
	const std::shared_ptr<CPOSVECTOR>& GetVertexPos() const { return m_spPosVector; }
	const void* GetIndices() const { return m_pIndices; }
	const _uint& GetIndexCnt() const { return m_iIndexCnt; }
	const VISPACE_TYPE& GetSpaceType() const { return m_eVISpaceType; }
	const VIINSTANCE_TYPE& GetInstanceType() const { return m_eVIInstanceType; }

	const _float3& GetMaxVertexPos() const { return m_vMaxVertex; }
	const _float3& GetMinVertexPos() const { return m_vMinVertex; }

	const VIBUFFERTYPE& GetBufferType() const { return m_eVIBufferType; }
public:
	virtual void Free() override;
	virtual HRESULT NativeConstruct() override PURE;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _stDatas) override PURE;
	virtual HRESULT Render(CSHPTRREF<UShader> _spShader, CSHPTRREF<UCommand> _spCommands, const _uint& _iInstanceCnt = 1);
protected:
	/* Get Set */
	const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const { return m_stD3DVertexBufferView; }
	const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const { return m_stD3DIndexBufferView; }
	const ComPtr<Dx12Resource>& GetVertexGpuBuffer() const { return m_cpVertexGpuBuffer; }
	const ComPtr<Dx12Resource>& GetIndexGpuBuffer() const { return m_cpIndexGpuBuffer; }


	void SetInstanceType(const VIINSTANCE_TYPE& _eInstanceType) { this->m_eVIInstanceType = _eInstanceType; }
	void SetMinPosition(const _float3& _vMinPosition) { this->m_vMinVertex = _vMinPosition; }
	void SetMaxPosition(const _float3& _vMaxPosition) { this->m_vMaxVertex = _vMaxPosition; }
	void SetPrimitiveTopology(const D3D_PRIMITIVE_TOPOLOGY _eTopology) { this->m_ePrimitiveTopology = _eTopology; }

	HRESULT CreateVtxBuffer(const _uint& _iVertexCnt, const _uint& _iBufferSize, const void* _pVertexData,
		const D3D_PRIMITIVE_TOPOLOGY& _eTopology, const POSVECTOR& _vecPosVector, const _bool _isComputeMinMaxPositon = true);
	HRESULT CreateVtxBuffer(const _uint& _iVertexCnt, const _uint& _iBufferSize, const void* _pVertexData,
		const D3D_PRIMITIVE_TOPOLOGY& _eTopology);

	HRESULT CreateVtxBufferWithNoData(const _uint& _iVertexCnt, const _uint& _iBufferSize, const void* _pVertexData,
		const D3D_PRIMITIVE_TOPOLOGY& _eTopology, const  D3D12_HEAP_TYPE& d3dHeapType, const D3D12_RESOURCE_STATES& d3dResourceStates);

	HRESULT CreateIndexBuffer(const _uint& _iIndexCnt, const _uint& _iBufferSize,
		const void* _pIndexData, const DXGI_FORMAT& _eIndexFormat, const _uint _iIndexMultiple = 3);

	virtual void BindVertexAndIndex(const ComPtr<Dx12GraphicsCommandList>& _cpGraphicCmdList, const _uint& _iInstanceCnt);
private:
	void ComputeMinMaxPosition();
protected:
	// Pos Indecies
	std::shared_ptr<CPOSVECTOR>	m_spPosVector;
	const void* m_pIndices;
	// GpuBuffer
	ComPtr<Dx12Resource>				m_cpVertexGpuBuffer;
	ComPtr<Dx12Resource>				m_cpIndexGpuBuffer;

	ComPtr<Dx12Resource>				m_cpVertexUploadBuffer;
	ComPtr<Dx12Resource>				m_cpIndexUploadBuffer;
	// VertexView
	D3D12_VERTEX_BUFFER_VIEW	m_stD3DVertexBufferView;
	// Vertex Data
	_uint													m_iVertexCnt;
	// Index View
	D3D12_INDEX_BUFFER_VIEW		m_stD3DIndexBufferView;
	// Index Data
	_uint													m_iIndexCnt;
	_uint													m_iIndexSize;
	// Enums
	D3D_PRIMITIVE_TOPOLOGY			m_ePrimitiveTopology;

	VISPACE_TYPE									m_eVISpaceType;
	VIINSTANCE_TYPE							m_eVIInstanceType;
	// Min MaxVertex
	_float3												m_vMinVertex;
	_float3												m_vMaxVertex;
	// CubeBufferType
	VIBUFFERTYPE									m_eVIBufferType;
};

END