#include "Cell.h"

CCell::CCell(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice(_pDevice), m_pContext(_pContext)
{
    ZeroMemory(m_vPoints, sizeof(_float3) * 3);
    ZeroMemory(m_vNormal, sizeof(_float3) * 3);
    ZeroMemory(m_vLine, sizeof(_float3) * 3);

    Safe_AddRef_DV(m_pDevice);
    Safe_AddRef_DV(m_pContext);
}

_bool CCell::IsSame(_float3* _pPoints)
{
    if (nullptr == _pPoints)
        return false;

    for (_uint i = 0; i < POINT_END; ++i)
    {
        _vector v1, v2;
        v1 = XMLoadFloat3(&m_vPoints[i]);
        v2 = XMLoadFloat3(&_pPoints[i]);

        if (m_vPoints[i].x != _pPoints[i].x
            || m_vPoints[i].y != _pPoints[i].y ||
            m_vPoints[i].z != _pPoints[i].z)
        {
            return false;
        }
    }
    return true;
}

HRESULT CCell::NativeConstruct(const _float3* _pPoints, const _uint& _iIndex)
{
    m_vPoints[POINT_A] = _pPoints[POINT_A];
    m_vPoints[POINT_B] = _pPoints[POINT_B];
    m_vPoints[POINT_C] = _pPoints[POINT_C];

    m_iIndex = _iIndex;
    Resort_Points();
#ifdef _DEBUG
    m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, _pPoints);
    if (nullptr == m_pVIBuffer)
        return E_FAIL;
#endif // _DEBUG

    _vector vPos = XMLoadFloat3(&m_vPoints[POINT_A]) +
        XMLoadFloat3(&m_vPoints[POINT_B]) + XMLoadFloat3(&m_vPoints[POINT_C]);
    vPos = vPos / 3.f;
    XMStoreFloat3(&m_vCenterPos, vPos);

    return S_OK;
}

HRESULT CCell::NativeConstruct(const CELLDESC& _tCellDesc)
{
    for (_uint i = 0; i < POINT_END; ++i)
    {
        m_vPoints[i] = _tCellDesc.vPoints[i];
        m_vNormal[i] = _tCellDesc.vNormal[i];
    }
    for (_uint i = 0; i < LINE_END; ++i)
    {
        m_vLine[i] = _tCellDesc.vLine[i];
        m_iNeighbor[i] = _tCellDesc.iNeighbor[i];
    }
    m_iIndex = _tCellDesc.iIndex;

    Resort_Points();
#ifdef _DEBUG
    m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
    if (nullptr == m_pVIBuffer)
        return E_FAIL;
#endif // _DEBUG

    _vector vPos = XMLoadFloat3(&m_vPoints[POINT_A]) +
        XMLoadFloat3(&m_vPoints[POINT_B]) + XMLoadFloat3(&m_vPoints[POINT_C]);
    vPos = vPos / 3.f;
    XMStoreFloat3(&m_vCenterPos, vPos);
    return S_OK;
}
#ifdef _DEBUG
void CCell::ReBuffer()
{
    Safe_Release(m_pVIBuffer);
    m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
}
#endif // _DEBUG
_bool CCell::ComparePoints(const _fvector& _PointA, const _fvector& _PointB)
{
    if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), _PointA))
    {
        if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), _PointB))
            return true;
        if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), _PointB))
            return true;
    }

    if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), _PointA))
    {
        if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), _PointB))
            return true;
        if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), _PointB))
            return true;
    }

    if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), _PointA))
    {
        if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), _PointB))
            return true;
        if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), _PointB))
            return true;
    }

    return false;
}

_bool CCell::ComparePoints(CCell* _pCell)
{
    if (nullptr == _pCell)
        return false;

    for (_uint k = 0; k < POINT_END; ++k)
    {
        if (XMVector3Equal(XMLoadFloat3(&m_vPoints[k]), _pCell->Get_Point(POINT_A)))
        {
            for (_uint i = 0; i < POINT_END; ++i)
            {
                if (i == k)
                    continue;

                if (XMVector3Equal(XMLoadFloat3(&m_vPoints[i]), _pCell->Get_Point(POINT_B)))
                    return true;

                if (XMVector3Equal(XMLoadFloat3(&m_vPoints[i]), _pCell->Get_Point(POINT_C)))
                    return true;
            }
        }

        if (XMVector3Equal(XMLoadFloat3(&m_vPoints[k]), _pCell->Get_Point(POINT_B)))
        {
            for (_uint i = 0; i < POINT_END; ++i)
            {
                if (i == k)
                    continue;

                if (XMVector3Equal(XMLoadFloat3(&m_vPoints[i]), _pCell->Get_Point(POINT_A)))
                    return true;

                if (XMVector3Equal(XMLoadFloat3(&m_vPoints[i]), _pCell->Get_Point(POINT_C)))
                    return true;
            }
        }

        if (XMVector3Equal(XMLoadFloat3(&m_vPoints[k]), _pCell->Get_Point(POINT_C)))
        {
            for (_uint i = 0; i < POINT_END; ++i)
            {
                if (i == k)
                    continue;

                if (XMVector3Equal(XMLoadFloat3(&m_vPoints[i]), _pCell->Get_Point(POINT_B)))
                    return true;

                if (XMVector3Equal(XMLoadFloat3(&m_vPoints[i]), _pCell->Get_Point(POINT_C)))
                    return true;
            }
        }
    }

    return false;
}

_float CCell::Compute_Height(_vector vPosition)
{
    _vector			vPlane;

    vPlane = XMPlaneFromPoints(XMLoadFloat3(&m_vPoints[POINT_B]),
        XMLoadFloat3(&m_vPoints[POINT_A]),
        XMLoadFloat3(&m_vPoints[POINT_C]));

    _float fY = XMVectorGetX(XMPlaneDotCoord(vPlane, vPosition)) + XMVectorGetY(vPosition);

    return fY;
}

_bool CCell::IsIn(const _fvector& _vPos, _int* _pNeightborIndex, _float3* _pLine)
{
    for (_uint i = 0; i < LINE_END; ++i)
    {
        _vector	vDir = XMVector3Normalize(XMVectorSetY(_vPos - XMLoadFloat3(&m_vPoints[i]), 0.f));

        if (0 < XMVectorGetX(XMVector3Dot(vDir, XMLoadFloat3(&m_vNormal[i]))))
        {
            if (nullptr != _pNeightborIndex)
                *_pNeightborIndex = m_iNeighbor[i];
            if (nullptr != _pLine)
                *_pLine = m_vLine[i];
            return false;
        }
    }
    return true;
}

#ifdef _DEBUG
HRESULT CCell::Render_Debug()
{
    if (nullptr == m_pVIBuffer)
        return E_FAIL;

    m_pVIBuffer->Render();
    return S_OK;
}
#endif
HRESULT CCell::Get_CrossResult(_float3* _pCrossResult)
{
    if (nullptr == _pCrossResult)
        return E_FAIL;

    _vector p1 = XMLoadFloat3(&_float3(m_vPoints[POINT_A].x, 0.f, m_vPoints[POINT_A].z));
    _vector p2 = XMLoadFloat3(&_float3(m_vPoints[POINT_B].x, 0.f, m_vPoints[POINT_B].z));
    _vector p3 = XMLoadFloat3(&_float3(m_vPoints[POINT_C].x, 0.f, m_vPoints[POINT_C].z));

    _vector L1 = XMVector3Normalize(p2 - p1);
    _vector L2 = XMVector3Normalize(p3 - p2);
    _vector L3 = XMVector3Normalize(p1 - p3);

    _vector vCross1 = XMVector3Cross(L1, L2);
    _vector vCross2 = XMVector3Cross(L2, L3);
    _vector vCross3 = XMVector3Cross(L3, L1);

    XMStoreFloat3(&_pCrossResult[0], vCross1);
    XMStoreFloat3(&_pCrossResult[1], vCross2);
    XMStoreFloat3(&_pCrossResult[2], vCross3);
    return S_OK;
}

HRESULT CCell::Resort_Points()
{
    _bool bIsTrue = true;
    while (bIsTrue)
    {
        _float3 v3PosList[3] = {};
        Get_CrossResult(v3PosList);

        if (v3PosList[0].y < 0)
        {
            Swap(m_vPoints[POINT_A], m_vPoints[POINT_B]);
            continue;
        }
        if (v3PosList[1].y < 0)
        {
            Swap(m_vPoints[POINT_B], m_vPoints[POINT_C]);
            continue;
        }
        if (v3PosList[2].y < 0)
        {
            Swap(m_vPoints[POINT_C], m_vPoints[POINT_A]);
            continue;
        }

        bIsTrue = false;
    }
    Make_NormalAndLine();
    return S_OK;
}

void CCell::Make_NormalAndLine()
{
    XMStoreFloat3(&m_vLine[LINE_AB], XMVector3Normalize(XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(&m_vPoints[POINT_A])));
    XMStoreFloat3(&m_vLine[LINE_BC], XMVector3Normalize(XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(&m_vPoints[POINT_B])));
    XMStoreFloat3(&m_vLine[LINE_CA], XMVector3Normalize(XMLoadFloat3(&m_vPoints[POINT_A]) - XMLoadFloat3(&m_vPoints[POINT_C])));

    m_vNormal[LINE_AB] = _float3(m_vLine[LINE_AB].z * -1.f, 0.f, m_vLine[LINE_AB].x);
    m_vNormal[LINE_BC] = _float3(m_vLine[LINE_BC].z * -1.f, 0.f, m_vLine[LINE_BC].x);
    m_vNormal[LINE_CA] = _float3(m_vLine[LINE_CA].z * -1.f, 0.f, m_vLine[LINE_CA].x);
}


CCell* CCell::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _float3* _pPoints, const _uint& _iIndex)
{
    CCell* pInstance = new CCell(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct(_pPoints, _iIndex)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CCell::Create To Failed");
    }
    return pInstance;
}

CCell* CCell::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const CELLDESC& _tCellDesc)
{
    CCell* pInstance = new CCell(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct(_tCellDesc)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CCell::Create To Failed");
    }
    return pInstance;
}

void CCell::Free()
{
#ifdef _DEBUG
    Safe_Release(m_pVIBuffer);
#endif
    Safe_Release_DV(m_pDevice);
    Safe_Release_DV(m_pContext);
}
