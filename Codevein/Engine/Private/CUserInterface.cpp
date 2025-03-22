#include "CUserInterface.h"
#include "CGameInstance.h"

CUserInterface::CUserInterface(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CGameObject(_pDevice, _pContext)
{
}

CUserInterface::CUserInterface(const CGameObject& _rhs)
    : CGameObject(_rhs)
{
}

HRESULT CUserInterface::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;
    return S_OK;
}

HRESULT CUserInterface::NativeConstruct_Clone(void* pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(pArg)))
        return E_FAIL;

    return S_OK;
}

void CUserInterface::Tick(const _double& _dTimeDelta)
{
    __super::Tick(_dTimeDelta);
}

void CUserInterface::Late_Tick(const _double& _dTimeDelta)
{
    __super::Late_Tick(_dTimeDelta);
}

HRESULT CUserInterface::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;
    return S_OK;
}

HRESULT CUserInterface::Ready_RectTransform(const _tchar* _pTag, CRectTransform::RECTDESC* _pDesc)
{
    m_pRectTransformCom = CRectTransform::Create(m_pDevice, m_pContext);
    if (nullptr == m_pRectTransformCom)
        return E_FAIL;

    m_Components.emplace(_pTag, m_pRectTransformCom);
    Safe_AddRef(m_pRectTransformCom);

    if (nullptr != _pDesc)
    {
        CRectTransform::RECTDESC tDesc = *static_cast<CRectTransform::RECTDESC*>(_pDesc);
        m_pRectTransformCom->Set_RectDesc(tDesc);
    }
    return S_OK;
}

HRESULT CUserInterface::Ready_RectTransform(const _tchar* _pTag, CRectTransform::RECTDESC* _pDesc,
    CRectTransform** _pRectTransform)
{
   * _pRectTransform = CRectTransform::Create(m_pDevice, m_pContext);
    if (nullptr == _pRectTransform)
        return E_FAIL;

    m_Components.emplace(_pTag, *_pRectTransform);
    Safe_AddRef(*_pRectTransform);

    if (nullptr != _pDesc)
    {
        CRectTransform::RECTDESC tDesc = *static_cast<CRectTransform::RECTDESC*>(_pDesc);
        (* _pRectTransform)->Set_RectDesc(tDesc);
    }
    return S_OK;
}

_bool CUserInterface::Is_MouseOnRect()
{
    if (nullptr == m_pRectTransformCom)
        return false;

    BEGIN_SINGLETON(CGameInstance, pGameInstance)

    _float2 v2MousePos = pGameInstance->Get_MousePosition();

    END_SINGLETON(pGameInstance);

    if (true == m_pRectTransformCom->Is_MouseOnRect(v2MousePos))
        return true;

    return false;
}

CGameObject* CUserInterface::Clone(void* _pArg)
{
    return nullptr;
}

void CUserInterface::Free()
{
    __super::Free();

    Safe_Release(m_pRectTransformCom);
}
