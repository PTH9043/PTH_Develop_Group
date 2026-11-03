#include "CFloorBuffer.h"
#include "CPolygon.h"
#include "CMethod.h"

CFloorBuffer::CFloorBuffer(const CFloorBuffer& _rhs) : CBuffer(_rhs)
{
}

shared_ptr<CComponent> CFloorBuffer::Clone(const VOIDDATAS& _tDatas)
{
	shared_ptr<CFloorBuffer> pInstance = CSmartPointerFactory<CFloorBuffer>::CreateShared(*this);
	if (FAILED(pInstance->NativeConstruct(_tDatas)))
	{
		Safe_Free(pInstance);
		DEBUG_MSG("CFloorMesh::Create To Failed");
	}
	return pInstance;
}

void CFloorBuffer::Free()
{
	__super::Free();
}

HRESULT CFloorBuffer::NativeConstruct()
{
	return S_OK;
}

HRESULT CFloorBuffer::NativeConstruct(VOIDDATAS _tDatas)
{
	if (FAILED(__super::NativeConstruct(_tDatas)))
		return E_FAIL;

	if (_tDatas.size() > 1) {
		FLOORBUFFERDESC tBufferDesc = CMethod::ConvertTemplate_Index<FLOORBUFFERDESC>(&_tDatas, 1);
		MakeFloorBuffer(tBufferDesc.fWidth, tBufferDesc.fDepth, tBufferDesc.iSubjects);
	}
	return  S_OK;
}

HRESULT CFloorBuffer::MakeFloorBuffer(const _float& _fWidth, const _float& _fDepth, const _int& _iSubRects)
{
	_float fHalfWidth = _fWidth * 0.5f;
	_float fHalfDepth = _fDepth * 0.5f;
	_float fCellWidth = _fWidth * (1.f / _iSubRects);
	_float fCellDepth = _fDepth * (1.f / _iSubRects);

	_int k = 0;
	vector<shared_ptr<CPolygon>> Polygons{};
	Polygons.reserve(_iSubRects * _iSubRects);
	for (_int i = 0; i < _iSubRects; ++i) {
		for (_int j = 0; j < _iSubRects; ++j) {
			shared_ptr<CPolygon> pPoly = CreateNew<CPolygon>(4);
			pPoly->BindVertex(0, VERTEX(-fHalfWidth + (i * fCellWidth), 0.0f, -fHalfDepth + (j * fCellDepth)));
			pPoly->BindVertex(1, VERTEX(-fHalfWidth + ((i + 1) * fCellWidth), 0.0f, -fHalfDepth + (j * fCellDepth)));
			pPoly->BindVertex(2, VERTEX(-fHalfWidth + ((i + 1) * fCellWidth), 0.0f, -fHalfDepth + ((j + 1) * fCellDepth)));
			pPoly->BindVertex(3, VERTEX(-fHalfWidth + (i * fCellWidth), 0.0f, -fHalfDepth + ((j + 1) * fCellDepth)));
			Polygons.push_back(pPoly);
		}
	}
	Bind_Polygon(Polygons);
	return S_OK;
}