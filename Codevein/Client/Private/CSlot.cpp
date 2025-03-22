#include "CSlot.h"
#include "CGameInstance.h"

CSlot::CSlot(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CUserInterface(_pDevice, _pContext)
{
}

CSlot::CSlot(const CSlot& _rhs)
    : CUserInterface(_rhs)
{
}

HRESULT CSlot::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;
    return S_OK;
}

HRESULT CSlot::NativeConstruct_Clone(void* pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    return S_OK;
}

void CSlot::Tick(const _double& _dTimeDelta)
{
    __super::Tick(_dTimeDelta);
}

void CSlot::Late_Tick(const _double& _dTimeDelta)
{
    __super::Late_Tick(_dTimeDelta);
}

HRESULT CSlot::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    if (FAILED(SetUp_ConstantTable()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSlot::Ready_Component()
{
    if (FAILED(Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXTEX, (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    if (FAILED(Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, (CComponent**)&m_pRendererCom)))
        return E_FAIL;

    if (FAILED(Add_Component(COM_TEXTURE, LEVEL_STATIC, PROTO_COMP_TEXTURE_UIGROUP, (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    if (FAILED(Add_Component(COM_VIBUFFER, LEVEL_STATIC, PROTO_COMP_VIBUFFER_RECT, (CComponent**)&m_pVIBufferRect)))
        return E_FAIL;

    return S_OK;
}

HRESULT CSlot::SetUp_ConstantTable()
{


    return S_OK;
}

CSlot* CSlot::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
    CSlot* pInstance = new CSlot(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CInventory::Create To Failed");
    }
    return pInstance;
}

CGameObject* CSlot::Clone(void* _pArg)
{
    CSlot* pInstance = new CSlot(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CSlot::Create To Failed");
    }
    return pInstance;
}

void CSlot::Free()
{
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pVIBufferRect);
    __super::Free();
}
