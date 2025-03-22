#include "CWall.h"

CWall::CWall(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CGameObject_Client(_pDevice, _pContext)
{
}

CWall::CWall(const CWall& _rhs)
    : CGameObject_Client(_rhs)
{
}

HRESULT CWall::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;
    return S_OK;
}

HRESULT CWall::NativeConstruct_Clone(void* pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(pArg)))
        return E_FAIL;
    return S_OK;
}

void CWall::Tick(const _double& _dTimeDelta)
{
    __super::Tick(_dTimeDelta);
}

void CWall::Late_Tick(const _double& _dTimeDelta)
{
    __super::Late_Tick(_dTimeDelta);

    
}

HRESULT CWall::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;
    return S_OK;
}

_bool CWall::Extern_EditValue()
{
    return _bool();
}

_bool CWall::Extern_HierakeyView(const string& _str)
{
    return _bool();
}

HRESULT CWall::Create_Self(CGameObject** _ppOut)
{
    return E_NOTIMPL;
}

HRESULT CWall::Delete_Self()
{
    return E_NOTIMPL;
}

HRESULT CWall::Save_Data()
{
    return E_NOTIMPL;
}

HRESULT CWall::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
    return E_NOTIMPL;
}

_bool CWall::Is_Picking(_float3* _pOut)
{
    return _bool();
}

void CWall::Reset_Data()
{
}

CWall* CWall::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    return nullptr;
}

CGameObject* CWall::Clone(void* _pArg)
{
    return nullptr;
}

void CWall::Free()
{
    __super::Free();
}
