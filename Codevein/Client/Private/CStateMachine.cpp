#include "CStateMachine.h"
#include "CGameInstance.h"
#include "CGameObject.h"

CStateMachine::CStateMachine(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : m_pDevice(_pDevice), m_pContext(_pContext)
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CStateMachine::NativeConstruct(CGameObject* _pOwnerObject)
{
    if (nullptr == _pOwnerObject)
        return E_FAIL;
    m_pTransform = static_cast<CTransform*>(_pOwnerObject->Get_Component(COM_TRANSFORM));
    m_pModel = static_cast<CModel*>(_pOwnerObject->Get_Component(COM_ANIMMODEL));


    if (nullptr == m_pTransform || nullptr == m_pModel)
        return E_FAIL;

    Safe_AddRef(m_pTransform);
    Safe_AddRef(m_pModel);
    return S_OK;
}


void CStateMachine::Free()
{
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pModel);
    Safe_Release(m_pTransform);
}
