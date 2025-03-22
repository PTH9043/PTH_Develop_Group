#include "Navigation.h"
#include "Shader.h"
#include "PipeLine.h"
#include "Collider.h"
#include <direct.h>

CNavigation::CNavigation(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CComponent(_pDevice, _pContext)
{
}

CNavigation::CNavigation(const CNavigation& _rhs)
    : CComponent(_rhs)
    , m_Cells(_rhs.m_Cells)
#ifdef _DEBUG
    , m_pShader(_rhs.m_pShader)
#endif // _DEBUG
{
#ifdef _DEBUG
    Safe_AddRef(m_pShader);
#endif // _DEBUG

    for (auto& pCell : m_Cells)
        Safe_AddRef(pCell);
}

void CNavigation::Add_Cells(CCell* _pCells)
{
    if (nullptr == _pCells)
        return;

    m_Cells.push_back(_pCells);
}

HRESULT CNavigation::NativeConstruct_Prototype(const _tchar* _pPath)
{
    if (nullptr == _pPath)
        return E_FAIL;

    Load_File(_pPath);

#ifdef _DEBUG
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("Shader_Cell.hlsl"), VTXCOL_DECLARATION::Element, VTXCOL_DECLARATION::iNumElement);
    if (nullptr == m_pShader)
        return E_FAIL;
#endif // _DEBUG

    return S_OK;
}

HRESULT CNavigation::NativeConstruct_Prototype_Init()
{
#ifdef _DEBUG
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("Shader_Cell.hlsl"), VTXCOL_DECLARATION::Element, VTXCOL_DECLARATION::iNumElement);
    if (nullptr == m_pShader)
        return E_FAIL;
#endif // _DEBUG

    CCollider::COLLIDERDESC tDesc;

    tDesc.vTranslation = _float3(0.f, 0.f, 0.f);
    tDesc.vScale = _float3(0.f, 0.f, 0.f);
    m_pCollider = CCollider::Create_NotClone(m_pDevice, m_pContext, CCollider::TYPE_AABB, &tDesc);
#ifdef _DEBUG
    m_pCollider->Set_Color(XMVectorSet(0.f, 1.f, 0.f, 0.f));
#endif // _DEBUG
    if (FAILED(Ready_Neighbor()))
        return E_FAIL;

    return S_OK;
}

HRESULT CNavigation::NativeConstruct_Clone(void* _pArg)
{
    if (nullptr != _pArg)
    {
        m_iCurrentIndex = ((NAVIDESC*)_pArg)->iCurIndex;
        m_iCurOhterIndex = m_iCurrentIndex;
    }
    return S_OK;
}

_float CNavigation::Compute_Height(_vector vPosition)
{
    if (nullptr != m_pCurCell)
    {
        return m_pCurCell->Compute_Height(vPosition);
    }
    else
    {
        for (auto& iter : m_Cells)
        {
            if (true == iter->IsIn(vPosition, &m_iCurrentIndex))
            {
                m_pCurCell = iter;
                break;
            }
        }
        if (nullptr == m_pCurCell)
            return XMVectorGetY(vPosition);

        return m_pCurCell->Compute_Height(vPosition);
    }
    return XMVectorGetY(vPosition);
}

_float3 CNavigation::Compute_SlideVector(const _vector& vCurPos, const _vector& vTargetPos)
{
    _int    iNeighborIndex = -1;
    _float3 vAdjLine = _float3(0.f, 0.f, 0.f);

    // 목표위치가 현재 셀에서 이동할 수 없을 때만 검사한다.
    if (false == m_Cells[m_iCurrentIndex]->IsIn(vTargetPos, &iNeighborIndex, &vAdjLine))
    {
        if (0 <= iNeighborIndex)
        {
            _uint iValue = 0;
            while (true)
            {
                // 목표 위치가 네비게이션 안에 존재하지 않을때
                if (-1 == iNeighborIndex)
                {
                    // 각도가 직각에 가까우면 슬라이딩 처리 안함
                    _float fAngle = XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenVectors(vAdjLine.Get_Vector(), vTargetPos - vCurPos)));
                    if (70.f < fAngle && fAngle < 110.f)
                        return _float3(0.f, 0.f, 0.f);

                    vAdjLine = vAdjLine.Normalize();
                    _float fLength = XMVectorGetX(XMVector3Dot(vAdjLine.Get_Vector(), vTargetPos - vCurPos));
                    _float3 vSlideDir = vAdjLine * fLength;
                    return vSlideDir.Normalize();
                }

                // true가 반환되면 이웃한 셀로 이동할 수 있다. 슬라이딩 벡터를 구할 필요없음
                if (true == m_Cells[iNeighborIndex]->IsIn(vTargetPos, &iNeighborIndex, &vAdjLine))
                {
                    return _float3(0.f, 0.f, 0.f);
                }
                if (iValue++ > 10)
                    return _float3(0.f, 0.f, 0.f);
            }
        }
        // 이웃이 없으면 현재 셀에서 닿은 라인이 슬라이드 방향이 된다.
        else
        {
            // 각도가 직각에 가까우면 슬라이딩 처리 안함
            _float fAngle = XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenVectors(vAdjLine.Get_Vector(), vTargetPos - vCurPos)));
            if (70.f < fAngle && fAngle < 110.f)
                return _float3(0.f, 0.f, 0.f);

            vAdjLine = vAdjLine.Normalize();
            _float fLength = XMVectorGetX(XMVector3Dot(vAdjLine.Get_Vector(), vTargetPos - vCurPos));
            _float3 vSlideDir = vAdjLine * fLength;
            return vSlideDir.Normalize();
        }
    }

    return _float3(0.f, 0.f, 0.f);
}

void CNavigation::Load_File(const _tchar* _pPath)
{
    _ulong		dwByte = 0;
    HANDLE		hFile = CreateFile(_pPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (0 == hFile)
        return;

    while (true)
    {
        _float3		vPoints[3];

        ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
        if (0 == dwByte)
            break;

        CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, (_uint)m_Cells.size());
        if (nullptr == pCell)
            return;

        m_Cells.push_back(pCell);
    }

    if (FAILED(Ready_Neighbor()))
        return;

    CloseHandle(hFile);
}

void CNavigation::Save_File(const _tchar* _pPath, const _uint& _iIndex)
{
    _wstring strPath = _pPath;
    _tchar NAME[100];
    wsprintf(NAME, L"Navigation%d", _iIndex);
    strPath.append(L"\\");
    strPath.append(NAME);
    strPath.append(L".dat");

    HANDLE  hInstnace = CreateFile(strPath.c_str(), GENERIC_WRITE, 0, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == hInstnace)
        return;

    DWORD dwByte = 0;
    _uint iCellSize = (_uint)m_Cells.size();
    WriteFile(hInstnace, &iCellSize, sizeof(_uint), &dwByte, nullptr);
    for (auto& iter : m_Cells)
    {
        for (_uint i = 0; i < CCell::POINT_END; ++i)
        {
            WriteFile(hInstnace, &iter->Get_Points()[i], sizeof(_float3), &dwByte, nullptr);
        }
        for (_uint i = 0; i < CCell::POINT_END; ++i)
        {
            WriteFile(hInstnace, &iter->Get_Normal()[i], sizeof(_float3), &dwByte, nullptr);
        }
        for (_uint i = 0; i < CCell::LINE_END; ++i)
        {
            WriteFile(hInstnace, &iter->Get_Lines()[i], sizeof(_float3), &dwByte, nullptr);
        }
        for (_uint i = 0; i < CCell::LINE_END; ++i)
        {
            WriteFile(hInstnace, &iter->Get_Neighbor()[i], sizeof(_int), &dwByte, nullptr);
        }
        _int iIndex = iter->Get_Index();
        WriteFile(hInstnace, &iIndex, sizeof(_int), &dwByte, nullptr);
    }
    if (nullptr != m_pCollider)
    {
        WriteFile(hInstnace, &m_pCollider->Get_Scale(), sizeof(_float3), &dwByte, nullptr);
        WriteFile(hInstnace, &m_pCollider->Get_Translate(), sizeof(_float3), &dwByte, nullptr);
    }

    CloseHandle(hInstnace);
}

_bool CNavigation::Is_Move(const _fvector& _vPosition, CCell** _ppOut)
{
    if (-1 == m_iCurrentIndex ||
        m_iCurrentIndex >= m_Cells.size())
    {
        return false;
    }

    _int		iNeighborIndex = -1;

    if (true == m_Cells[m_iCurrentIndex]->IsIn(_vPosition, &iNeighborIndex))
    {
        m_pCurCell = m_Cells[m_iCurrentIndex];
        if (nullptr != _ppOut)
            *_ppOut = m_pCurCell;
        return true;
    }
    else
    {
        /* 나간방향에 이웃이 있었다라면? */
        if (0 <= iNeighborIndex)
        {
            _uint iValue = 0;
            while (true)
            {
                if (-1 == iNeighborIndex)
                    return false;

                if (true == m_Cells[iNeighborIndex]->IsIn(_vPosition, &iNeighborIndex))
                {
                    m_iCurrentIndex = iNeighborIndex;
                    break;
                }
                ++iValue;
                if (iValue > 10)
                {
                    Find_Cell(_vPosition);
                    return false;
                }
            }

            m_pCurCell = m_Cells[m_iCurrentIndex];
            if (nullptr != _ppOut)
                *_ppOut = m_pCurCell;
            return true;
        }
        else /* 이웃이 없었다라면. */
        {
            return false;
        }
    }

    return false;
}
_bool CNavigation::Is_MoveOther(const _fvector& _vPosition, CCell** _pNode)
{
    if (-1 == m_iCurOhterIndex ||
        m_iCurOhterIndex >= m_Cells.size())
    {
        return false;
    }

    _int		iNeighborIndex = -1;

    if (true == m_Cells[m_iCurOhterIndex]->IsIn(_vPosition, &iNeighborIndex))
    {
        if (nullptr != _pNode)
            *_pNode = m_Cells[m_iCurOhterIndex];
        return true;
    }
    else
    {
        /* 나간방향에 이웃이 있었다라면? */
        _uint iValue = 0;
        if (0 <= iNeighborIndex)
        {
            while (true)
            {
                if (-1 == iNeighborIndex)
                    return false;

                if (true == m_Cells[iNeighborIndex]->IsIn(_vPosition, &iNeighborIndex))
                {
                    m_iCurOhterIndex = iNeighborIndex;
                    break;
                }
                ++iValue;
                if (iValue >30)
                {
                    Find_Cell(_vPosition);
                    return false;
                }
            }

            return true;
        }
        else /* 이웃이 없었다라면. */
        {
            return false;
        }
    }
    return false;
}
_bool CNavigation::Is_Collision(CCollider* _pCollider)
{
    if (nullptr == _pCollider || nullptr == m_pCollider)
        return false;

    if (true == m_pCollider->Is_Collision(_pCollider))
        return true;

    return false;
}

CCell* CNavigation::Find_Cell(const _vector& _vPos)
{
    int _iNehgiborIndex = 0;
    for (auto& iter : m_Cells)
    {
        if (true == iter->IsIn(_vPos, &_iNehgiborIndex))
        {
            m_iCurrentIndex = iter->Get_Index();
            return iter;
        }
    }
    return nullptr;
}

#ifdef  _DEBUG

HRESULT CNavigation::Render_Debug()
{
    if (nullptr != m_pCollider)
    {
        m_pCollider->Render_Debug();

    }
    if (nullptr == m_pShader || 0 >= m_Cells.size())
        return E_FAIL;


    _float4x4		WorldMatrix;
    XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

    CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);
    {
        if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
            return E_FAIL;
        if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
            return E_FAIL;
    }
    RELEASE_INSTANCE(CPipeLine);



        if (-1 == m_iCurrentIndex)
        {
            if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
                return E_FAIL;

            _float4 vColor = _float4(0.f, 1.f, 0.f, 1.f);
            if (FAILED(m_pShader->Set_RawValue("g_vColor", &vColor, sizeof(_float4))))
                return E_FAIL;

            m_pShader->Begin(0);

            for (auto& pCell : m_Cells)
            {
                if (nullptr != pCell)
                {
                    pCell->Render_Debug();
                }
            }
        }
        else
        {
            WorldMatrix._42 = 0.1f;

            XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
            if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
                return E_FAIL;
            {
                _float4 vColor = _float4(1.f, 0.f, 0.f, 1.f);
                if (FAILED(m_pShader->Set_RawValue("g_vColor", &vColor, sizeof(_float4))))
                    return E_FAIL;
                m_pShader->Begin(0);
                for (auto& pCell : m_Cells)
                {
                    if (nullptr != pCell)
                    {
                        if (pCell != m_pCurCell)
                            pCell->Render_Debug();
                    }
                }
            }

            _float4 vColor = _float4(1.f, 0.f, 0.f, 1.f);
            if (FAILED(m_pShader->Set_RawValue("g_vColor", &vColor, sizeof(_float4))))
                return E_FAIL;

            m_pShader->Begin(0);
            m_Cells[m_iCurrentIndex]->Render_Debug();
        }

    return S_OK;
}
#endif //  _DEBUG

HRESULT CNavigation::Ready_Neighbor()
{
    for (auto& pSourCell : m_Cells)
    {
        for (auto& pDestCell : m_Cells)
        {
            if (pSourCell == pDestCell)
                continue;

            if (true == pDestCell->ComparePoints(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
                pSourCell->Set_Neighbor(CCell::LINE_AB, pDestCell->Get_Index());

            if (true == pDestCell->ComparePoints(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
                pSourCell->Set_Neighbor(CCell::LINE_BC, pDestCell->Get_Index());

            if (true == pDestCell->ComparePoints(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
                pSourCell->Set_Neighbor(CCell::LINE_CA, pDestCell->Get_Index());

        }
    }

    return S_OK;
}

CNavigation* CNavigation::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pPath)
{
    CNavigation* pInstance = new CNavigation(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype(_pPath)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CNavigation::Create to Failed");
    }
    return pInstance;
}

CNavigation* CNavigation::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CNavigation* pInstance = new CNavigation(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype_Init()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CNavigation::Create to Failed");
    }
    return pInstance;
}

CComponent* CNavigation::Clone(void* _pArg)
{
    CNavigation* pInstance = new CNavigation(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CNavigation::Create to Failed");
    }
    return pInstance;
}

void CNavigation::Free()
{
    for (auto& iter : m_Cells)
        Safe_Release(iter);
#ifdef _DEBUG
    Safe_Release(m_pShader);
#endif

    Safe_Release(m_pCollider);
    __super::Free();
}
