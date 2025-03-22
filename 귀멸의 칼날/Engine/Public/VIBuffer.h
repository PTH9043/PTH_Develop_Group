#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	explicit CVIBuffer(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext);
	explicit CVIBuffer(const CVIBuffer& _rhs);
	virtual ~CVIBuffer() = default;
public:
	ID3D11Buffer* Get_VB() { return m_pVB; }
	const _uint& Get_NumBuffer() const { return m_iNumBuffers; }
	const _uint& Get_NumVertices() const { return m_iNumVertices; }
	const _uint& Get_Stride() const { return m_iStride; }
public:
	ID3D11Buffer* Get_IB() { return m_pIB; }
	const _uint& Get_NumPrimitive() const { return m_iNumPrimitive; }
	const _uint& Get_NumIndicesPerPrimitive() const { return m_iNumIndicesPerPrimitive; }
	const _uint& Get_BytePerPrimitive() const { return m_iBytePerPrimitive; }
	const DXGI_FORMAT& Get_DXGI_Format() const { return m_eFormat; }
public:
	 void* Get_Indices() { return m_pIndices; }
	 _float3* Get_VerticesPos() { return m_pVerticesPos; }
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	virtual HRESULT Render();
	// ReCalculate Value
	virtual void* ReCalculate_Value(void* _pData);
protected:
	HRESULT Create_VertexBuffer(const D3D11_BUFFER_DESC& _tBufferDesc, const D3D11_SUBRESOURCE_DATA& _tBufferSubResourceData);
	HRESULT Create_IndexBuffer(const D3D11_BUFFER_DESC& _tBufferDesc, const D3D11_SUBRESOURCE_DATA& _tBufferSubResourceData);

protected:
	ID3D11Buffer* m_pVB = nullptr;
	_uint													m_iNumBuffers = 0;
	_uint													m_iStride = 0;
	_uint													m_iNumVertices = 0;

protected:
	ID3D11Buffer* m_pIB = nullptr;
	_uint													m_iNumPrimitive = 0;
	_uint													m_iNumIndicesPerPrimitive = 0;
	_uint													m_iBytePerPrimitive = 0;
	DXGI_FORMAT									m_eFormat;
	D3D11_PRIMITIVE_TOPOLOGY	m_eTopology;
	// Indices
	void* m_pIndices = nullptr;
	// Num Vertices Pos
	_float3* m_pVerticesPos = nullptr;
public:
	virtual CComponent* Clone(void* _pArg) = 0;
	virtual void Free() override;
};

END