#pragma once
#include "CBase.h"

typedef vector<VERTEX>						VERTECIES;
typedef vector<_uint>							INDEXBUFFERS;

class CPolygon : public CBase {
protected:
	explicit CPolygon() = default;
	virtual ~CPolygon() = default;

public:
	virtual void Free() override;
public:
	// NativeConstruct
	HRESULT NativeConstruct(const _uint& _iVertexCnt = 0);
private:
	void Draw2DLine(const HDC& _hDC, _float3 _vPrev, _float3 _vNext,
		const shared_ptr<class CGameInstance>& _pGameInstance);
public:
	// Render 
	void Render(const HDC& _hDC, const _float4x4& _Matrix, const shared_ptr<class CGameInstance>& _pGameInstance);

	// Bind Vertex
	void BindVertex(const _uint& _iIndex, const VERTEX& _tVertex);

	void BindVertex(const VERTECIES& _VertecBuffers) { m_VertexBuffers = _VertecBuffers; }
public:
	const VERTECIES& Get_VertexPos() const { return m_VertexBuffers; }
private:
	VERTECIES						m_VertexBuffers{};
};

