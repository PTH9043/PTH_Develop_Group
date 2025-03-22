#include "QuadTree.h"
#include "FrustCulling.h"
#include "PipeLine.h"

CQuadTree::CQuadTree() 
{
}

HRESULT CQuadTree::Initialize(const _uint& _iLT, const _uint& _iRT, const _uint& _iRB, const _uint& _iLB)
{
	m_iCorner[CORNER_LT] = _iLT;
	m_iCorner[CORNER_RT] = _iRT;
	m_iCorner[CORNER_RB] = _iRB;
	m_iCorner[CORNER_LB] = _iLB;

	if (1 == _iRT - _iLT)
		return S_OK;

	m_iCenter = (_iLT + _iRB) >> 1;

	if (FAILED(Make_Child()))
		return E_FAIL;

	if (FAILED(Make_Neighbor()))
		return E_FAIL;

	return S_OK;
}

void CQuadTree::Culling(CFrustCulling* _pFrustCulling, const _float3* _pVerticesPos, FACEINDICES32* _pIndices,
	_uint* _pNumFaces, _fmatrix _WorldMatrixInv)
{
	if (nullptr == m_pChild[CORNER_LT] ||
		true == LevelOfDetail(_pVerticesPos, _WorldMatrixInv))
	{
		_bool		isDraw[4] = { true, true, true, true };

		if (nullptr != m_pNeighbor[NEIGHBOR_LEFT])
			isDraw[NEIGHBOR_LEFT] = m_pNeighbor[NEIGHBOR_LEFT]->LevelOfDetail(_pVerticesPos, _WorldMatrixInv);
		if (nullptr != m_pNeighbor[NEIGHBOR_TOP])
			isDraw[NEIGHBOR_TOP] = m_pNeighbor[NEIGHBOR_TOP]->LevelOfDetail(_pVerticesPos, _WorldMatrixInv);
		if (nullptr != m_pNeighbor[NEIGHBOR_RIGHT])
			isDraw[NEIGHBOR_RIGHT] = m_pNeighbor[NEIGHBOR_RIGHT]->LevelOfDetail(_pVerticesPos, _WorldMatrixInv);
		if (nullptr != m_pNeighbor[NEIGHBOR_BOTTOM])
			isDraw[NEIGHBOR_BOTTOM] = m_pNeighbor[NEIGHBOR_BOTTOM]->LevelOfDetail(_pVerticesPos, _WorldMatrixInv);

		_bool		isIn[] = {
			_pFrustCulling->Is_Rendering_InLocal(XMLoadFloat3(&_pVerticesPos[m_iCorner[CORNER_LT]])),
			_pFrustCulling->Is_Rendering_InLocal(XMLoadFloat3(&_pVerticesPos[m_iCorner[CORNER_RT]])),
			_pFrustCulling->Is_Rendering_InLocal(XMLoadFloat3(&_pVerticesPos[m_iCorner[CORNER_RB]])),
			_pFrustCulling->Is_Rendering_InLocal(XMLoadFloat3(&_pVerticesPos[m_iCorner[CORNER_LB]])),
		};

		if (true == isDraw[NEIGHBOR_LEFT] &&
			true == isDraw[NEIGHBOR_TOP] &&
			true == isDraw[NEIGHBOR_RIGHT] &&
			true == isDraw[NEIGHBOR_BOTTOM])
		{
			if (true == isIn[0] ||
				true == isIn[1] ||
				true == isIn[2])
			{
				_pIndices[*_pNumFaces]._1 = m_iCorner[CORNER_LT];
				_pIndices[*_pNumFaces]._2 = m_iCorner[CORNER_RT];
				_pIndices[*_pNumFaces]._3 = m_iCorner[CORNER_RB];
				++* _pNumFaces;
			}

			if (true == isIn[0] ||
				true == isIn[2] ||
				true == isIn[3])
			{
				_pIndices[*_pNumFaces]._1 = m_iCorner[CORNER_LT];
				_pIndices[*_pNumFaces]._2 = m_iCorner[CORNER_RB];
				_pIndices[*_pNumFaces]._3 = m_iCorner[CORNER_LB];
				++* _pNumFaces;
			}
			return;
		}

		_uint			iLC, iTC, iRC, iBC;

		iLC = (m_iCorner[CORNER_LT] + m_iCorner[CORNER_LB]) >> 1;
		iTC = (m_iCorner[CORNER_LT] + m_iCorner[CORNER_RT]) >> 1;
		iRC = (m_iCorner[CORNER_RT] + m_iCorner[CORNER_RB]) >> 1;
		iBC = (m_iCorner[CORNER_LB] + m_iCorner[CORNER_RB]) >> 1;

		if (true == isIn[0] ||
			true == isIn[2] ||
			true == isIn[3])
		{
			if (false == isDraw[NEIGHBOR_LEFT])
			{
				_pIndices[*_pNumFaces]._1 = m_iCorner[CORNER_LT];
				_pIndices[*_pNumFaces]._2 = m_iCenter;
				_pIndices[*_pNumFaces]._3 = iLC;
				++* _pNumFaces;

				_pIndices[*_pNumFaces]._1 = iLC;
				_pIndices[*_pNumFaces]._2 = m_iCenter;
				_pIndices[*_pNumFaces]._3 = m_iCorner[CORNER_LB];
				++* _pNumFaces;
			}

			else
			{
				_pIndices[*_pNumFaces]._1 = m_iCorner[CORNER_LT];
				_pIndices[*_pNumFaces]._2 = m_iCenter;
				_pIndices[*_pNumFaces]._3 = m_iCorner[CORNER_LB];
				++* _pNumFaces;
			}

			if (false == isDraw[NEIGHBOR_BOTTOM])
			{
				_pIndices[*_pNumFaces]._1 = m_iCorner[CORNER_LB];
				_pIndices[*_pNumFaces]._2 = m_iCenter;
				_pIndices[*_pNumFaces]._3 = iBC;
				++* _pNumFaces;

				_pIndices[*_pNumFaces]._1 = iBC;
				_pIndices[*_pNumFaces]._2 = m_iCenter;
				_pIndices[*_pNumFaces]._3 = m_iCorner[CORNER_RB];
				++* _pNumFaces;
			}

			else
			{
				_pIndices[*_pNumFaces]._1 = m_iCorner[CORNER_LB];
				_pIndices[*_pNumFaces]._2 = m_iCenter;
				_pIndices[*_pNumFaces]._3 = m_iCorner[CORNER_RB];
				++* _pNumFaces;
			}
		}

		if (true == isIn[0] ||
			true == isIn[1] ||
			true == isIn[2])
		{
			if (false == isDraw[NEIGHBOR_TOP])
			{
				_pIndices[*_pNumFaces]._1 = m_iCorner[CORNER_LT];
				_pIndices[*_pNumFaces]._2 = iTC;
				_pIndices[*_pNumFaces]._3 = m_iCenter;
				++* _pNumFaces;

				_pIndices[*_pNumFaces]._1 = m_iCenter;
				_pIndices[*_pNumFaces]._2 = iTC;
				_pIndices[*_pNumFaces]._3 = m_iCorner[CORNER_RT];
				++* _pNumFaces;
			}

			else
			{
				_pIndices[*_pNumFaces]._1 = m_iCorner[CORNER_LT];
				_pIndices[*_pNumFaces]._2 = m_iCorner[CORNER_RT];
				_pIndices[*_pNumFaces]._3 = m_iCenter;
				++* _pNumFaces;
			}

			if (false == isDraw[NEIGHBOR_RIGHT])
			{
				_pIndices[*_pNumFaces]._1 = m_iCorner[CORNER_RT];
				_pIndices[*_pNumFaces]._2 = iRC;
				_pIndices[*_pNumFaces]._3 = m_iCenter;
				++* _pNumFaces;

				_pIndices[*_pNumFaces]._1 = m_iCenter;
				_pIndices[*_pNumFaces]._2 = iRC;
				_pIndices[*_pNumFaces]._3 = m_iCorner[CORNER_RB];
				++* _pNumFaces;
			}

			else
			{
				_pIndices[*_pNumFaces]._1 = m_iCorner[CORNER_RT];
				_pIndices[*_pNumFaces]._2 = m_iCorner[CORNER_RB];
				_pIndices[*_pNumFaces]._3 = m_iCenter;
				++* _pNumFaces;
			}
		}

		return;
	}

	m_fRadius = XMVectorGetX(XMVector3Length(XMLoadFloat3(&_pVerticesPos[m_iCorner[CORNER_LT]]) - XMLoadFloat3(&_pVerticesPos[m_iCenter])));

	if (true == _pFrustCulling->Is_Rendering_InLocal(XMLoadFloat3(&_pVerticesPos[m_iCenter]), m_fRadius))
	{
		for (_uint i = 0; i < CORNER_END; ++i)
		{
			if (nullptr != m_pChild[i])
				m_pChild[i]->Culling(_pFrustCulling, _pVerticesPos, _pIndices, _pNumFaces, _WorldMatrixInv);
		}

	}
}

_bool CQuadTree::LevelOfDetail(const _float3* _pVerticesPos, _fmatrix _WorldMatrixInv)
{
	_vector vCamPos;
	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);
	{
		vCamPos = pPipeLine->Get_CamPosition();
		vCamPos = XMVector3TransformCoord(vCamPos, _WorldMatrixInv);
	}
	RELEASE_INSTANCE(CPipeLine);

	_float fDistance = XMVectorGetX(XMVector3Length(vCamPos - XMLoadFloat3(&_pVerticesPos[m_iCenter])));
	_float fWidth = XMVectorGetX(XMVector3Length(XMLoadFloat3(&_pVerticesPos[m_iCenter])));

	if (fDistance * 0.1f > fWidth)
		return true;
	return false;
}

HRESULT CQuadTree::Make_Child()
{
	_uint			iLC, iTC, iRC, iBC;

	iLC = (m_iCorner[CORNER_LT] + m_iCorner[CORNER_LB]) >> 1;
	iTC = (m_iCorner[CORNER_LT] + m_iCorner[CORNER_RT]) >> 1;
	iRC = (m_iCorner[CORNER_RT] + m_iCorner[CORNER_RB]) >> 1;
	iBC = (m_iCorner[CORNER_LB] + m_iCorner[CORNER_RB]) >> 1;

	m_pChild[CORNER_LT] = CQuadTree::Create(m_iCorner[CORNER_LT], iTC, m_iCenter, iLC);
	m_pChild[CORNER_RT] = CQuadTree::Create(iTC, m_iCorner[CORNER_RT], iRC, m_iCenter);
	m_pChild[CORNER_RB] = CQuadTree::Create(m_iCenter, iRC, m_iCorner[CORNER_RB], iBC);
	m_pChild[CORNER_LB] = CQuadTree::Create(iLC, m_iCenter, iBC, m_iCorner[CORNER_LB]);

	return S_OK;
}

HRESULT CQuadTree::Make_Neighbor()
{
	if (nullptr == m_pChild[CORNER_LT]->m_pChild[CORNER_LT])
		return S_OK;

	m_pChild[CORNER_LT]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pChild[CORNER_RT];
	m_pChild[CORNER_LT]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pChild[CORNER_LB];

	m_pChild[CORNER_RT]->m_pNeighbor[NEIGHBOR_LEFT] = m_pChild[CORNER_LT];
	m_pChild[CORNER_RT]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pChild[CORNER_RB];

	m_pChild[CORNER_RB]->m_pNeighbor[NEIGHBOR_LEFT] = m_pChild[CORNER_LB];
	m_pChild[CORNER_RB]->m_pNeighbor[NEIGHBOR_TOP] = m_pChild[CORNER_RT];

	m_pChild[CORNER_LB]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pChild[CORNER_RB];
	m_pChild[CORNER_LB]->m_pNeighbor[NEIGHBOR_TOP] = m_pChild[CORNER_LT];

	if (nullptr != m_pNeighbor[NEIGHBOR_RIGHT])
	{
		m_pChild[CORNER_RT]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pNeighbor[NEIGHBOR_RIGHT]->m_pChild[CORNER_LT];
		m_pChild[CORNER_RB]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pNeighbor[NEIGHBOR_RIGHT]->m_pChild[CORNER_LB];
	}

	if (nullptr != m_pNeighbor[NEIGHBOR_BOTTOM])
	{
		m_pChild[CORNER_LB]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pNeighbor[NEIGHBOR_BOTTOM]->m_pChild[CORNER_LT];
		m_pChild[CORNER_RB]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pNeighbor[NEIGHBOR_BOTTOM]->m_pChild[CORNER_RT];
	}

	if (nullptr != m_pNeighbor[NEIGHBOR_LEFT])
	{
		m_pChild[CORNER_LT]->m_pNeighbor[NEIGHBOR_LEFT] = m_pNeighbor[NEIGHBOR_LEFT]->m_pChild[CORNER_RT];
		m_pChild[CORNER_LB]->m_pNeighbor[NEIGHBOR_LEFT] = m_pNeighbor[NEIGHBOR_LEFT]->m_pChild[CORNER_RB];
	}

	if (nullptr != m_pNeighbor[NEIGHBOR_TOP])
	{
		m_pChild[CORNER_LT]->m_pNeighbor[NEIGHBOR_TOP] = m_pNeighbor[NEIGHBOR_TOP]->m_pChild[CORNER_LB];
		m_pChild[CORNER_RT]->m_pNeighbor[NEIGHBOR_TOP] = m_pNeighbor[NEIGHBOR_TOP]->m_pChild[CORNER_RB];
	}

	for (_uint i = 0; i < CORNER_END; ++i)
	{
		if (nullptr != m_pChild[i])
			m_pChild[i]->Make_Neighbor();
	}

	return S_OK;
}

CQuadTree* CQuadTree::Create(const _uint& _iLT, const _uint& _iRT, const _uint& _iRB, const _uint& _iLB)
{
	CQuadTree* pInstance = new CQuadTree();

	if (FAILED(pInstance->Initialize(_iLT, _iRT, _iRB, _iLB)))
	{
		MSG_BOX("Failed to Created : CQuadTree");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CQuadTree::Free()
{
	for (_uint i = 0; i < CORNER_END; ++i)
		Safe_Release(m_pChild[i]);
}
