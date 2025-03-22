#include "FrustCulling.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CFrustCulling)

CFrustCulling::CFrustCulling()
{
}

HRESULT CFrustCulling::NativeConstruct_FrustCull(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
    const CFrustCulling::FRUSTDESC& _tFrustDesc)
{
    if (nullptr == _pDevice || nullptr == _pContext)
        return E_FAIL;

    this->m_pDevice = _pDevice;
    this->m_pContext = _pContext;

    Safe_AddRef_DV(m_pDevice);
    Safe_AddRef_DV(m_pContext);

    // 가까운 면
    m_FrustPosition[0] = { -1.f , -1.f  , 0.f };
    m_FrustPosition[1] = { -1.f , 1.f  , 0.f };
    m_FrustPosition[2] = { 1.f , 1.f  , 0.f };
    m_FrustPosition[3] = { 1.f , -1.f, 0.f };

    m_tFrustDesc = _tFrustDesc;
    m_FrustPosition[4] = { -1.f , -1.f  , 1.f };
    m_FrustPosition[5] = { -1.f , 1.f, 1.f };
    m_FrustPosition[6] = { 1.f , 1.f, 1.f };
    m_FrustPosition[7] = { 1.f , -1.f, 1.f };

    _matrix		ProjMatrixInv = XMMatrixPerspectiveFovLH(_tFrustDesc.fFovy, _tFrustDesc.fAspect, _tFrustDesc.fNear, _tFrustDesc.fFar);
    ProjMatrixInv = XMMatrixInverse(nullptr, ProjMatrixInv);
    XMStoreFloat4x4(&m_ProjMatrixInv, ProjMatrixInv);
    return S_OK;
}

void CFrustCulling::Update_Matrix()
{
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        _matrix View = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
        _matrix ProjInv = XMLoadFloat4x4(&m_ProjMatrixInv);
        _matrix ViewInv = XMMatrixInverse(nullptr, View);
        _vector vPoints[8] = {};

        for (_uint i = 0; i < 8; ++i)
        {
            vPoints[i] = XMLoadFloat3(&m_FrustPosition[i]);
            vPoints[i] = XMVector3TransformCoord(vPoints[i], ProjInv);
            vPoints[i] = XMVector3TransformCoord(vPoints[i], ViewInv);
            XMStoreFloat3(&m_WorldPosition[i], vPoints[i]);
        }
        // 위
        XMStoreFloat4(&m_vPlaneInWorld[0], XMPlaneFromPoints(vPoints[3], vPoints[2], vPoints[1]));
        // 아래
        XMStoreFloat4(&m_vPlaneInWorld[1], XMPlaneFromPoints(vPoints[7], vPoints[5], vPoints[6]));
        // 오른쪽
        XMStoreFloat4(&m_vPlaneInWorld[2], XMPlaneFromPoints(vPoints[4], vPoints[1], vPoints[5]));
        // 왼쪽
        XMStoreFloat4(&m_vPlaneInWorld[3], XMPlaneFromPoints(vPoints[6], vPoints[1], vPoints[2]));
        // 앞쪽
        XMStoreFloat4(&m_vPlaneInWorld[4], XMPlaneFromPoints(vPoints[6], vPoints[2], vPoints[7]));
        // 오른쪽
        XMStoreFloat4(&m_vPlaneInWorld[5], XMPlaneFromPoints(vPoints[3], vPoints[0], vPoints[7]));
    }
    RELEASE_INSTANCE(CGameInstance);
}

void CFrustCulling::Update_LocalMatrix(_fmatrix _WorldMatrix)
{
    _vector		vPoint[8];

    for (_uint i = 0; i < 8; ++i)
        vPoint[i] = XMVector3TransformCoord(XMLoadFloat3(&m_WorldPosition[i]), _WorldMatrix);

    XMStoreFloat4(&m_vPlaneInLocal[0], XMPlaneFromPoints(vPoint[1], vPoint[5], vPoint[6]));
    XMStoreFloat4(&m_vPlaneInLocal[1], XMPlaneFromPoints(vPoint[4], vPoint[0], vPoint[3]));
    XMStoreFloat4(&m_vPlaneInLocal[2], XMPlaneFromPoints(vPoint[4], vPoint[5], vPoint[1]));
    XMStoreFloat4(&m_vPlaneInLocal[3], XMPlaneFromPoints(vPoint[3], vPoint[2], vPoint[6]));
    XMStoreFloat4(&m_vPlaneInLocal[4], XMPlaneFromPoints(vPoint[5], vPoint[4], vPoint[7]));
    XMStoreFloat4(&m_vPlaneInLocal[5], XMPlaneFromPoints(vPoint[0], vPoint[1], vPoint[2]));
}

_bool CFrustCulling::Is_RenderingSphere(CTransform* _pTransform, const _float& _fRadius)
{
    if (nullptr == _pTransform)
        return false;

    for (_uint i = 0; i < 6; ++i)
    {
        _float fDist = XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vPlaneInWorld[i]), _pTransform->Get_Pos()));
        if (true == Is_JudgeColl(fDist, _fRadius))
            return true;

        if (fDist < 0)
            return false;
    }
    return true;
}

_bool CFrustCulling::Is_RenderingSphere(const _vector& _vPos, const _float& _fRadius)
{
    for (_uint i = 0; i < 6; ++i)
    {
        _float fDist = XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vPlaneInWorld[i]), _vPos));
        if (true == Is_JudgeColl(fDist, _fRadius))
            return true;

        if (fDist < 0)
            return false;
    }
    return true;
}

_bool CFrustCulling::Is_Rendering_InLocal(const _vector& _vPos, const _float& _fRange)
{
    for (_uint i = 0; i < 6; ++i)
    {
        if (_fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vPlaneInLocal[i]), _vPos)))
            return false;
    }

    return true;
}

_bool CFrustCulling::Is_JudgeColl(const _float& _fDistnace, const _float& _fRadius)
{
    if (fabs(_fDistnace) <= _fRadius)
        return true;

    return false;
}

void CFrustCulling::Free()
{
    Safe_Release_DV(m_pDevice);
    Safe_Release_DV(m_pContext);
}
