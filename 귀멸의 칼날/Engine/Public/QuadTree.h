#pragma once
#include "Base.h"
#include "QuadTree.h"

BEGIN(Engine)

class CQuadTree final : public CBase
{
public:
	enum CORNER { CORNER_LT, CORNER_RT, CORNER_RB, CORNER_LB, CORNER_END };
	enum NEIGHBOR { NEIGHBOR_LEFT, NEIGHBOR_TOP, NEIGHBOR_RIGHT, NEIGHBOR_BOTTOM, NEIGHBOR_END };
private:
	explicit CQuadTree();
	virtual ~CQuadTree() = default;

public:
	HRESULT Initialize(const _uint& _iLT, const _uint& _iRT, const _uint& _iRB, const _uint& _iLB);
	void Culling(class CFrustCulling* _pFrustCulling, const _float3* _pVerticesPos,
		FACEINDICES32* _pIndices, _uint* _pNumFaces, _fmatrix _WorldMatrixInv);
	_bool LevelOfDetail(const _float3* _pVerticesPos, _fmatrix _WorldMatrixInv);

private:
	HRESULT Make_Child();
	HRESULT Make_Neighbor();
private:
	_uint	m_iCorner[CORNER_END] = {};
	_uint	m_iCenter = 0;
	_float	m_fRadius = 0.f;

	CQuadTree* m_pChild[CORNER_END] = { nullptr };
	CQuadTree* m_pNeighbor[NEIGHBOR_END] = { nullptr };
public:
	static CQuadTree* Create(const _uint& _iLT, const _uint& _iRT, const _uint& _iRB, const _uint& _iLB);
	virtual void Free() override;
};

END