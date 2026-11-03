#include "CBuffer.h"
#include "CBuffer.h"
#include "CGameInstance.h"
#include "CActor.h"
#include "CPolygon.h"

CBuffer::CBuffer(const CBuffer& _rhs) : CComponent(_rhs),
m_PolygonDatas(_rhs.m_PolygonDatas), m_PolygonCnt(_rhs.m_PolygonCnt)
{
}

shared_ptr<CComponent> CBuffer::Clone(const VOIDDATAS& _tDatas)
{
	return shared_ptr<CComponent>();
}

void CBuffer::Free()
{
	m_PolygonDatas.reset();
}

HRESULT CBuffer::NativeConstruct()
{
	return S_OK;
}

HRESULT CBuffer::NativeConstruct(VOIDDATAS _tDatas)
{
	return __super::NativeConstruct(_tDatas);
}

HRESULT CBuffer::Render(const HDC& _hDC)
{
	// Get Instance
	shared_ptr<CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);
	// Screen Matrix
	_float4x4 vWVP = pGameInstance->CombineMatrix(Get_Actor().lock());

	POLYGONS Polygons = *m_PolygonDatas.get();
	const _uint POLYGON_SIZE = static_cast<_uint>(Polygons.size());

	_float3 v3PrevPos, v3InitialPos;
	_bool bPreviousInside = false, bInitialInside = false, bCurrentInside = false, bIntersectInside = false;

	HPEN hPen = ::CreatePen(PS_SOLID, 0, RGB(m_vColor.x, m_vColor.y, m_vColor.z));
	HPEN hOldPen = (HPEN)::SelectObject(_hDC, hPen);
	for (_uint j = 0; j < POLYGON_SIZE; ++j) {

		(*m_PolygonDatas)[j]->Render(_hDC, vWVP, pGameInstance);
	}
	::SelectObject(_hDC, hOldPen);
	::DeleteObject(hPen);
	return S_OK;
}

void CBuffer::Draw2DLine(const HDC& _hDC,  _float3 _vPrev,  _float3 _vNext, 
	const shared_ptr<class CGameInstance>& _pGameInstance)
{
	_pGameInstance->ChangeScreenToPos(_vPrev);
	_pGameInstance->ChangeScreenToPos( _vNext);

	::MoveToEx(_hDC, (long)_vPrev.x, (long)_vPrev.y, NULL);
	::LineTo(_hDC, (long)_vNext.x, (long)_vNext.y);
}

void CBuffer::Bind_Polygon(const POLYGONS& _value)
{
	if (nullptr == m_PolygonDatas)
		m_PolygonDatas = std::make_shared<POLYGONS>();

	*m_PolygonDatas = _value;

	m_PolygonCnt = std::make_shared<_uint>((_uint)m_PolygonDatas->size());
}
