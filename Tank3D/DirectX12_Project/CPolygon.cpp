#include "CPolygon.h"
#include "CGameInstance.h"

void CPolygon::Free()
{
}

HRESULT CPolygon::NativeConstruct(const _uint& _iVertexCnt)
{
    m_VertexBuffers.resize(_iVertexCnt);
    return S_OK;
}

void CPolygon::Draw2DLine(const HDC& _hDC, _float3 _vPrev, _float3 _vNext, const shared_ptr<class CGameInstance>& _pGameInstance)
{
	_pGameInstance->ChangeScreenToPos(_vPrev);
	_pGameInstance->ChangeScreenToPos(_vNext);
	::MoveToEx(_hDC, (long)_vPrev.x, (long)_vPrev.y, NULL);
	::LineTo(_hDC, (long)_vNext.x, (long)_vNext.y);
}

void CPolygon::Render(const HDC& _hDC, const _float4x4& _Matrix, const shared_ptr<class CGameInstance>& _pGameInstance)
{
	_float3 v3PrevPos, v3InitialPos;
	_bool bPreviousInside = false, bInitialInside = false, bCurrentInside = false, bIntersectInside = false;

	v3PrevPos = v3InitialPos = m_VertexBuffers[0].vPosition.Coord4x4(_Matrix);
	bPreviousInside = bInitialInside = (-1.0f <= v3InitialPos.x) && (v3InitialPos.x <= 1.0f)
		&& (-1.0f <= v3InitialPos.y) && (v3InitialPos.y <= 1.0f);

	for (_uint i = 1; i < m_VertexBuffers.size(); ++i) {
		_float3 v3CurrentPos = m_VertexBuffers[i].vPosition.Coord4x4(_Matrix);
		bCurrentInside = (-1.0f <= v3CurrentPos.x) && (v3CurrentPos.x <= 1.0f) && (-1.0f <= v3CurrentPos.y) && (v3CurrentPos.y <= 1.0f);
		if (((0.0f <= v3CurrentPos.z) && (v3CurrentPos.z <= 1.0f)) && ((bCurrentInside || bPreviousInside)))
			Draw2DLine(_hDC, v3PrevPos, v3CurrentPos, _pGameInstance);
		v3PrevPos = v3CurrentPos;
	}
	if (((0.0f <= v3InitialPos.z) && (v3InitialPos.z <= 1.0f)) && ((bInitialInside || bPreviousInside)))
		Draw2DLine(_hDC, v3PrevPos, v3InitialPos, _pGameInstance);
}

void CPolygon::BindVertex(const _uint& _iIndex, const VERTEX& _tVertex)
{
	if ((0 <= _iIndex) && (_iIndex <= m_VertexBuffers.size()))
	{
		m_VertexBuffers[_iIndex] = _tVertex;
	}
}