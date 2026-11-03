#include "CCristalBuffer.h"
#include "CPolygon.h"

CCristalBuffer::CCristalBuffer(const CCristalBuffer& _rhs) : CBuffer(_rhs)
{
}

shared_ptr<CComponent> CCristalBuffer::Clone(const VOIDDATAS& _tDatas)
{
	shared_ptr<CCristalBuffer> pInstance = CSmartPointerFactory<CCristalBuffer>::CreateShared(*this);
	if (FAILED(pInstance->NativeConstruct(_tDatas)))
	{
		Safe_Free(pInstance);
		DEBUG_MSG("CCristalBuffer::Create To Failed");
	}
	return pInstance;
}

void CCristalBuffer::Free()
{
	__super::Free();
}

HRESULT CCristalBuffer::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	const _uint SIZE = 24;
	vector<shared_ptr<CPolygon>> Polygons{};

	float fHalfWidth = 1.f;
	float fHalfHeight = 1.f;
	float fHalfDepth = 0.5f;

	{
		shared_ptr<CPolygon> pFrontFace = CreateNew<CPolygon>();
		VERTECIES vertecies{ VERTEX(-fHalfWidth, +fHalfHeight, -fHalfDepth) ,
												  VERTEX(+fHalfWidth, +fHalfHeight, -fHalfDepth),
												  VERTEX(+fHalfWidth, -fHalfHeight, -fHalfDepth),
												  VERTEX(-fHalfWidth, -fHalfHeight, -fHalfDepth) };

		pFrontFace->BindVertex(vertecies);
		Polygons.push_back(pFrontFace);
	}
	{
		shared_ptr<CPolygon> pTopFace = CreateNew<CPolygon>();
		VERTECIES vertecies{ VERTEX(-fHalfWidth, +fHalfHeight, +fHalfDepth) ,
												  VERTEX(+fHalfWidth, +fHalfHeight, +fHalfDepth),
												  VERTEX(+fHalfWidth, +fHalfHeight, -fHalfDepth),
												  VERTEX(-fHalfWidth, +fHalfHeight, -fHalfDepth) };

		pTopFace->BindVertex(vertecies);
		Polygons.push_back(pTopFace);
	}
	{
		shared_ptr<CPolygon> pBackFace = CreateNew<CPolygon>();
		VERTECIES vertecies{ VERTEX(-fHalfWidth, -fHalfHeight, +fHalfDepth) ,
										  VERTEX(+fHalfWidth, -fHalfHeight, +fHalfDepth),
										  VERTEX(+fHalfWidth, +fHalfHeight, +fHalfDepth),
										  VERTEX(-fHalfWidth, +fHalfHeight, +fHalfDepth) };

		pBackFace->BindVertex(vertecies);
		Polygons.push_back(pBackFace);
	}
	{
		shared_ptr<CPolygon> pBottomFace = CreateNew<CPolygon>();
		VERTECIES vertecies{ VERTEX(-fHalfWidth, -fHalfHeight, -fHalfDepth) ,
									  VERTEX(+fHalfWidth, -fHalfHeight, -fHalfDepth),
									  VERTEX(+fHalfWidth, -fHalfHeight, +fHalfDepth),
									  VERTEX(-fHalfWidth, -fHalfHeight, +fHalfDepth) };

		pBottomFace->BindVertex(vertecies);
		Polygons.push_back(pBottomFace);
	}
	{
		shared_ptr<CPolygon> pLeftFace = CreateNew<CPolygon>();
		VERTECIES vertecies{ VERTEX(-fHalfWidth, +fHalfHeight, +fHalfDepth) ,
							  VERTEX(-fHalfWidth, +fHalfHeight, -fHalfDepth),
							  VERTEX(-fHalfWidth, -fHalfHeight, -fHalfDepth),
							  VERTEX(-fHalfWidth, -fHalfHeight, +fHalfDepth) };

		pLeftFace->BindVertex(vertecies);
		Polygons.push_back(pLeftFace);
	}
	{
		shared_ptr<CPolygon> pRightFace = CreateNew<CPolygon>();
		VERTECIES vertecies{ VERTEX(+fHalfWidth, +fHalfHeight, -fHalfDepth) ,
					  VERTEX(+fHalfWidth, +fHalfHeight, +fHalfDepth),
					  VERTEX(+fHalfWidth, -fHalfHeight, +fHalfDepth),
					  VERTEX(+fHalfWidth, -fHalfHeight, -fHalfDepth) };

		pRightFace->BindVertex(vertecies);
		Polygons.push_back(pRightFace);
	}
	Bind_Polygon(Polygons);
	return S_OK;
}

HRESULT CCristalBuffer::NativeConstruct(VOIDDATAS _tDatas)
{
	return __super::NativeConstruct(_tDatas);
}
