#include "CActor.h"
#include "CTransform.h"
#include "CGameInstance.h"

shared_ptr<CActor> CActor::Clone(const VOIDDATAS& _tDatas)
{
    return shared_ptr<CActor>();
}

void CActor::Free()
{
    Safe_Free(m_pTransform);
}

HRESULT CActor::NativeConstruct()
{
    return S_OK;
}

HRESULT CActor::NativeConstruct(VOIDDATAS _tDatas)
{
    CreateComponent();
    return S_OK;
}

shared_ptr<CComponent> CActor::CloneComp(const _wstring& _strPrototypes, const VOIDDATAS& _tDatas)
{
    shared_ptr<CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);
    shared_ptr<CComponent> pComponent = pGameInstance->Clone(_strPrototypes, _tDatas);
    return pComponent;
}

HRESULT CActor::CreateComponent()
{
    m_pTransform = CTransform::Create();
    Add_Component(COM_TRANSFORM, m_pTransform);
    return S_OK;
}

HRESULT CActor::Tick_Awake(const _double& _dTimeDelta)
{
    if (false == m_bActive)
        return E_FAIL;

    for (auto& iter : m_Components) {
        iter.second.lock()->Tick(_dTimeDelta);
    }

    return S_OK;
}

HRESULT CActor::Tick(const _double& _dTimeDelta)
{
    if (false == m_bActive)
        return E_FAIL;

    return S_OK;
}

HRESULT CActor::Late_Tick(const _double& _dTimeDelta)
{
    if (false == m_bActive)
        return E_FAIL;

    return S_OK;
}

HRESULT CActor::Render(const HDC& _hDC)
{
    if (false == m_bActive)
        return E_FAIL;

    return S_OK;
}

void CActor::Add_Component(const _wstring& _str, const shared_ptr<CComponent>& _pCompontens) {
    
    if (nullptr == _pCompontens)
        return;
    // Components
    m_Components.emplace(_str, _pCompontens);
}
