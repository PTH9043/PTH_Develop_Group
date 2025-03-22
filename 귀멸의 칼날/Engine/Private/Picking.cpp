#include "Picking.h"
#include "GameInstance.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CPicking)

CPicking::CPicking()
{
}

HRESULT CPicking::NativeConstruct(const HWND& _hWnd, ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    m_hWnd = _hWnd;

    m_pDevice = _pDevice;
    m_pContext = _pContext;

    Safe_AddRef_DV(m_pDevice);
    Safe_AddRef_DV(m_pContext);
    Compute_RayInWorldSpace();
    return S_OK;
}

void CPicking::Compute_RayInWorldSpace()
{
    CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
    {
        _float2 v2MousePos = pInstance->Get_MousePosition();
        RECT rc;
        GetClientRect(m_hWnd, &rc);

        if ((rc.left > v2MousePos.x || v2MousePos.x > rc.right) && (rc.top > v2MousePos.y || v2MousePos.y > rc.bottom))
        {
            m_bMousePicking = false;
            RELEASE_INSTANCE(CGameInstance);
            return;
        }
        // Mouse Picking
        m_bMousePicking = true;

        UINT iNumViewPort = 0;
        m_pContext->RSGetViewports(&iNumViewPort, nullptr);

        D3D11_VIEWPORT* pViewPorts = new D3D11_VIEWPORT[iNumViewPort];
        m_pContext->RSGetViewports(&iNumViewPort, pViewPorts);

        _float4x4 ProjMatrix;
        XMStoreFloat4x4(&ProjMatrix, pInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

        _float4 v4Positon;
        v4Positon.x = (v2MousePos.x * 2.f / (pViewPorts[0].Width) - 1.f) / ProjMatrix._11;
        v4Positon.y = (v2MousePos.y * -2.f / (pViewPorts[0].Height) + 1.f) / ProjMatrix._22;
        v4Positon.z = 1.f;
        v4Positon.w = 1.f;

        Safe_Delete_Array(pViewPorts);

        _vector vPos = XMLoadFloat4(&v4Positon);
        /*  View CalCulate */
        //_matrix ProjMatrix;
        //ProjMatrix = *pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);
        //ProjMatrix = XMMatrixInverse(nullptr, ProjMatrix);
        //vPos = XMVector4Transform(vPos, ProjMatrix);
        // Ray Dir, Pos
        m_v3RayDir = _float3(v4Positon.x, v4Positon.y, v4Positon.z);
        m_v3RayPos = _float3(0.f, 0.f, 0.f);

        /*  World CalCulate */
        _matrix ViewMatrix;
        ViewMatrix = pInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
        ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);
        /* Ray */
        XMStoreFloat3(&m_v3RayDir, XMVector3TransformNormal(XMLoadFloat3(&m_v3RayDir), ViewMatrix));
        XMStoreFloat3(&m_v3RayPos, XMVector3TransformCoord(XMLoadFloat3(&m_v3RayPos), ViewMatrix));
    }
    RELEASE_INSTANCE(CGameInstance);
}

_bool CPicking::Picking_Mesh(CGameObject* _pGameObject, CTransform* _pTransform, CVIBuffer* _pVIBuffer, _float3* _pOut)
{
    if (false == m_bMousePicking)
        return false;

    if (nullptr == _pTransform || nullptr == _pVIBuffer)
        return false;

    _vector v3RayPos = XMLoadFloat3(&m_v3RayPos), v3RayDir = XMLoadFloat3(&m_v3RayDir);
    // Ray Dir In Local
    _vector v3RayDirInLocal, v3RayPosInLocal;
    // 월드의 역행렬
    _matrix WorldMatrixInv = _pTransform->Get_WorldMatrixInv();
    v3RayPosInLocal = XMVector3TransformCoord(v3RayPos, WorldMatrixInv);
    v3RayDirInLocal = XMVector3TransformNormal(v3RayDir, WorldMatrixInv);
    v3RayDirInLocal = XMVector3Normalize(v3RayDirInLocal);

    _uint iNumFaces = _pVIBuffer->Get_NumPrimitive();
    const _float3* pVerticesPos = _pVIBuffer->Get_VerticesPos();

    const void* pIndices = _pVIBuffer->Get_Indices();
    DXGI_FORMAT eFormat = _pVIBuffer->Get_DXGI_Format();

    _uint iSize = 0;
    if (eFormat == DXGI_FORMAT_R16_UINT)
        iSize = sizeof(FACEINDICES16);
    else
        iSize = sizeof(FACEINDICES32);

    _float	fDist = 0.f;
    if (eFormat == DXGI_FORMAT_R16_UINT)
    {
        for (_uint i = 0; i < iNumFaces; ++i)
        {
            _ushort iIndices[3];
            memcpy(&iIndices, (_byte*)pIndices + iSize * i, iSize);

            _vector v1 = XMLoadFloat3(&pVerticesPos[iIndices[0]]);
            _vector v2 = XMLoadFloat3(&pVerticesPos[iIndices[1]]);
            _vector v3 = XMLoadFloat3(&pVerticesPos[iIndices[2]]);

            if (TriangleTests::Intersects(v3RayPosInLocal, v3RayDirInLocal, v1, v2, v3, fDist))
            {
                _float3 v3Pos = _float3(0.f, 0.f, 0.f);
                _vector vPos = v3RayPosInLocal + v3RayDirInLocal * fDist;
                vPos = XMVector3TransformCoord(vPos, _pTransform->Get_WorldMatrix());
                XMStoreFloat3(&v3Pos, vPos);
                if (nullptr != _pOut)
                    *_pOut = v3Pos;
                if (nullptr != _pGameObject)
                {
                    // Add
                    Add_GameObject(PICKINGDESC(_pGameObject, _pTransform, v3Pos, fDist));
                }
                return true;
            }
        }
    }
    else if (eFormat == DXGI_FORMAT_R32_UINT)
    {
        for (_uint i = 0; i < iNumFaces; ++i)
        {
            _uint iIndices[3];
            memcpy(&iIndices, (_byte*)pIndices + iSize * i, iSize);

            _vector v1 = XMLoadFloat3(&pVerticesPos[iIndices[0]]);
            _vector v2 = XMLoadFloat3(&pVerticesPos[iIndices[1]]);
            _vector v3 = XMLoadFloat3(&pVerticesPos[iIndices[2]]);

            if (TriangleTests::Intersects(v3RayPosInLocal, v3RayDirInLocal, v1, v2, v3, fDist))
            {
                _float3 v3Pos = _float3(0.f, 0.f, 0.f);
                _vector vPos = v3RayPosInLocal + v3RayDirInLocal * fDist;
                vPos = XMVector3TransformCoord(vPos, _pTransform->Get_WorldMatrix());
                XMStoreFloat3(&v3Pos, vPos);
                if (nullptr != _pOut)
                    *_pOut = v3Pos;
                // Add
                Add_GameObject(PICKINGDESC(_pGameObject, _pTransform, v3Pos, fDist));
                return true;
            }
        }
    }
    return false;
}

_bool CPicking::Picking_Mesh(CGameObject* _pGameObject, CTransform* _pTransform, CVIBuffer* _pVIBuffer, _matrix _BoneMatrix)
{
    if (false == m_bMousePicking)
        return false;

    if (nullptr == _pTransform || nullptr == _pVIBuffer)
        return false;

    _vector v3RayPos = XMLoadFloat3(&m_v3RayPos), v3RayDir = XMLoadFloat3(&m_v3RayDir);
    // Ray Dir In Local
    _vector v3RayDirInLocal, v3RayPosInLocal;
    // 월드의 역행렬
    _matrix WorldMatrix = _pTransform->Get_WorldMatrix() * _BoneMatrix;
    _matrix WorldMatrixInv = XMMatrixInverse(nullptr, WorldMatrix);
    v3RayPosInLocal = XMVector3TransformCoord(v3RayPos, WorldMatrixInv);
    v3RayDirInLocal = XMVector3TransformNormal(v3RayDir, WorldMatrixInv);
    v3RayDirInLocal = XMVector3Normalize(v3RayDirInLocal);

    _uint iNumFaces = _pVIBuffer->Get_NumPrimitive();
    const _float3* pVerticesPos = _pVIBuffer->Get_VerticesPos();

    const void* pIndices = _pVIBuffer->Get_Indices();
    DXGI_FORMAT eFormat = _pVIBuffer->Get_DXGI_Format();

    _uint iSize = 0;
    if (eFormat == DXGI_FORMAT_R16_UINT)
        iSize = sizeof(FACEINDICES16);
    else
        iSize = sizeof(FACEINDICES32);

    _float	fDist = 0.f;
    if (eFormat == DXGI_FORMAT_R16_UINT)
    {
        for (_uint i = 0; i < iNumFaces; ++i)
        {
            _ushort iIndices[3];
            memcpy(&iIndices, (_byte*)pIndices + iSize * i, iSize);

            _vector v1 = XMLoadFloat3(&pVerticesPos[iIndices[0]]);
            _vector v2 = XMLoadFloat3(&pVerticesPos[iIndices[1]]);
            _vector v3 = XMLoadFloat3(&pVerticesPos[iIndices[2]]);

            if (TriangleTests::Intersects(v3RayPosInLocal, v3RayDirInLocal, v1, v2, v3, fDist))
            {
                _float3 v3Pos = _float3(0.f, 0.f, 0.f);
                _vector vPos = v3RayPosInLocal + v3RayDirInLocal * fDist;
                vPos = XMVector3TransformCoord(vPos, _pTransform->Get_WorldMatrix());
                XMStoreFloat3(&v3Pos, vPos);
                if (nullptr != _pGameObject)
                {
                    // Add
                    Add_GameObject(PICKINGDESC(_pGameObject, _pTransform, v3Pos, fDist));
                }
                return true;
            }
        }
    }
    else if (eFormat == DXGI_FORMAT_R32_UINT)
    {
        for (_uint i = 0; i < iNumFaces; ++i)
        {
            _uint iIndices[3];
            memcpy(&iIndices, (_byte*)pIndices + iSize * i, iSize);

            _vector v1 = XMLoadFloat3(&pVerticesPos[iIndices[0]]);
            _vector v2 = XMLoadFloat3(&pVerticesPos[iIndices[1]]);
            _vector v3 = XMLoadFloat3(&pVerticesPos[iIndices[2]]);

            if (TriangleTests::Intersects(v3RayPosInLocal, v3RayDirInLocal, v1, v2, v3, fDist))
            {
                _float3 v3Pos = _float3(0.f, 0.f, 0.f);
                _vector vPos = v3RayPosInLocal + v3RayDirInLocal * fDist;
                vPos = XMVector3TransformCoord(vPos, _pTransform->Get_WorldMatrix());
                XMStoreFloat3(&v3Pos, vPos);
                // Add
                Add_GameObject(PICKINGDESC(_pGameObject, _pTransform, v3Pos, fDist));
                return true;
            }
        }
    }
    return false;
}

// Picking
_bool CPicking::Picking_Mesh_Nav(CGameObject* _pGameObject, CTransform* _pTransform, CVIBuffer* _pVIBuffer)
{
    if (false == m_bMousePicking)
        return false;

    if (nullptr == _pTransform || nullptr == _pVIBuffer)
        return false;

    _vector v3RayPos = XMLoadFloat3(&m_v3RayPos), v3RayDir = XMLoadFloat3(&m_v3RayDir);
    // Ray Dir In Local
    _vector v3RayDirInLocal, v3RayPosInLocal;
    // 월드의 역행렬
    _matrix WorldMatrixInv = _pTransform->Get_WorldMatrixInv();
    v3RayPosInLocal = XMVector3TransformCoord(v3RayPos, WorldMatrixInv);
    v3RayDirInLocal = XMVector3TransformNormal(v3RayDir, WorldMatrixInv);
    v3RayDirInLocal = XMVector3Normalize(v3RayDirInLocal);

    _uint iNumFaces = _pVIBuffer->Get_NumPrimitive();
    const _float3* pVerticesPos = _pVIBuffer->Get_VerticesPos();

    const void* pIndices = _pVIBuffer->Get_Indices();
    DXGI_FORMAT eFormat = _pVIBuffer->Get_DXGI_Format();

    _uint iSize = 0;
    if (eFormat == DXGI_FORMAT_R16_UINT)
        iSize = sizeof(FACEINDICES16);
    else
        iSize = sizeof(FACEINDICES32);

    _float	fDist = 0.f;
    list<_float3> vDirList;
    if (eFormat == DXGI_FORMAT_R16_UINT)
    {
        for (_uint i = 0; i < iNumFaces; ++i)
        {
            _ushort iIndices[3];
            memcpy(&iIndices, (_byte*)pIndices + iSize * i, iSize);

            _vector v1 = XMLoadFloat3(&pVerticesPos[iIndices[0]]);
            _vector v2 = XMLoadFloat3(&pVerticesPos[iIndices[1]]);
            _vector v3 = XMLoadFloat3(&pVerticesPos[iIndices[2]]);

            if (TriangleTests::Intersects(v3RayPosInLocal, v3RayDirInLocal, v1, v2, v3, fDist))
            {
                _float3 v3Pos = _float3(0.f, 0.f, 0.f);
                _vector vPos = v3RayPosInLocal + v3RayDirInLocal * fDist;
                vPos = XMVector3TransformCoord(vPos, _pTransform->Get_WorldMatrix());
                XMStoreFloat3(&v3Pos, vPos);
                vDirList.push_back(v3Pos);
                // Add
                Add_GameObject(PICKINGDESC(_pGameObject, _pTransform, v3Pos, fDist), true);
            }
        }
    }
    else if (eFormat == DXGI_FORMAT_R32_UINT)
    {
        for (_uint i = 0; i < iNumFaces; ++i)
        {
            _uint iIndices[3];
            memcpy(&iIndices, (_byte*)pIndices + iSize * i, iSize);

            _vector v1 = XMLoadFloat3(&pVerticesPos[iIndices[0]]);
            _vector v2 = XMLoadFloat3(&pVerticesPos[iIndices[1]]);
            _vector v3 = XMLoadFloat3(&pVerticesPos[iIndices[2]]);

            if (TriangleTests::Intersects(v3RayPosInLocal, v3RayDirInLocal, v1, v2, v3, fDist))
            {
                _float3 v3Pos = _float3(0.f, 0.f, 0.f);
                _vector vPos = v3RayPosInLocal + v3RayDirInLocal * fDist;
                vPos = XMVector3TransformCoord(vPos, _pTransform->Get_WorldMatrix());
                XMStoreFloat3(&v3Pos, vPos);
                vDirList.push_back(v3Pos);
                // Add
                Add_GameObject(PICKINGDESC(_pGameObject, _pTransform, v3Pos, fDist), true);
            }
        }
    }

    if (vDirList.size() > 0)
    {
        //BEGIN_SINGLETON(CGameInstance, pGameInstance);
        //_vector vCamPos = *pGameInstance->Get_CamPosition();
        //END_SINGLETON(pGameInstance);
        //vDirList.sort([&](const _float3& _vP1, const _float3& _vP2)->bool
        //{
        //    _vector v1 = vCamPos - XMLoadFloat3(&_vP1);
        //    _vector v2 = vCamPos - XMLoadFloat3(&_vP2);

        //    float fLen1 = XMVectorGetX(XMVector3LengthSq(v1));
        //    float fLen2 = XMVectorGetX(XMVector3LengthSq(v2));

        //    return fLen1 < fLen2;
        //});
        //// Add
        //Add_GameObject(PICKINGDESC(_pGameObject, _pTransform, vDirList.front()));
        return true;
    }

    return false;
}

CPicking::PICKINGDESC* CPicking::Get_Picking_GOB()
{
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        _vector vCamPos = pGameInstance->Get_CamPosition();
        m_PickList.sort([&](PICKINGDESC t1, PICKINGDESC t2)
        {
            _vector pos1 = XMLoadFloat3(&t1.v3PickingPos);
            _vector pos2 = XMLoadFloat3(&t2.v3PickingPos);

            float fLen1 = XMVectorGetX(XMVector3LengthSq(pos1 - vCamPos));
            float fLen2 = XMVectorGetX(XMVector3LengthSq(pos2 - vCamPos));

            return fLen1 < fLen2;
        });

        if (m_PickList.size() > 0)
        {
            m_tPickInfo = m_PickList.front();
            m_PickList.clear();
            return &m_tPickInfo;
        }
    }
    RELEASE_INSTANCE(CGameInstance);
    return nullptr;
}

void CPicking::Release_Pickings()
{
    m_PickList.clear();
}

_bool CPicking::Add_GameObject(const PICKINGDESC& _tPickDesc, const _bool& _bIsPosPick)
{
    auto iter = find_if(m_PickList.begin(), m_PickList.end(), [&](auto& pair)->bool
    {
        if (_tPickDesc.pGameObject == pair.pGameObject)
            return true;

        return false;
    });

    if (false == _bIsPosPick)
    {
        if (m_PickList.end() == iter)
        {
            m_PickList.push_back(_tPickDesc);
            return false;
        }
    }
    else
    {
        m_PickList.push_back(_tPickDesc);
    }

    return true;
}

void CPicking::Free()
{
    Safe_Release_DV(m_pContext);
    Safe_Release_DV(m_pDevice);
}
