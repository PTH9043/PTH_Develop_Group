#include "CItemSlot.h"
#include "CItem.h"
#include "CGameInstance.h"

CItemSlot::CItemSlot(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CSlot(_pDevice, _pContext)
{
}

CItemSlot::CItemSlot(const CItemSlot& _rhs)
    : CSlot(_rhs)
{
}

HRESULT CItemSlot::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CItemSlot::NativeConstruct_Clone(void* pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(pArg)))
        return E_FAIL;

    return S_OK;
}

void CItemSlot::Tick(const _double& _dTimeDelta)
{
    __super::Tick(_dTimeDelta);
}

void CItemSlot::Late_Tick(const _double& _dTimeDelta)
{
    __super::Late_Tick(_dTimeDelta);
}

HRESULT CItemSlot::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;
    return S_OK;
}

HRESULT CItemSlot::Ready_Component()
{
    if (FAILED(__super::Ready_Component()))
        return E_FAIL;

    return S_OK;
}

HRESULT CItemSlot::SetUp_ConstantTable()
{
    if (FAILED(__super::SetUp_ConstantTable()))
        return E_FAIL;

    return S_OK;
}

CItemSlot* CItemSlot::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
    CItemSlot* pInstance = new CItemSlot(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CItemSlot::Create To Failed");
    }
    return pInstance;
}

CGameObject* CItemSlot::Clone(void* _pArg)
{
    CItemSlot* pInstance = new CItemSlot(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CItemSlot::Clone To Failed");
    }
    return pInstance;
}

void CItemSlot::Free()
{
    __super::Free();
}
