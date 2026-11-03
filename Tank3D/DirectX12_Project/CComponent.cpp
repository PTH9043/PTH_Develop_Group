#include "CComponent.h"
#include "CMethod.h"

shared_ptr<CComponent> CComponent::Clone(const VOIDDATAS& _tDatas)
{
    return shared_ptr<CComponent>();
}

void CComponent::Free()
{
}

HRESULT CComponent::NativeConstruct()
{
    return S_OK;
}

HRESULT CComponent::NativeConstruct(VOIDDATAS _tDatas)
{
    if (_tDatas.size() < 0)
        return E_FAIL;

    COMPDESC tDesc = CMethod::ConvertTemplate_Index< COMPDESC>(&_tDatas, 0);
    m_pActor = tDesc.pActor;
    return S_OK;
}
