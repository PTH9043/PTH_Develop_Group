#include "CItem_Manager.h"

CItem_Manager::CItem_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CManager(_pDevice, _pContext)
{
}

HRESULT CItem_Manager::NativeConstruct()
{
    return S_OK;
}

HRESULT CItem_Manager::Save_Data(const _string& _strFolderPath)
{
    return E_NOTIMPL;
}

HRESULT CItem_Manager::Load_Data(const _string& _strFolderPath)
{
    return E_NOTIMPL;
}

CItem_Manager* CItem_Manager::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CItem_Manager* pInstnace = new CItem_Manager(_pDevice, _pContext);
    if (FAILED(pInstnace->NativeConstruct()))
    {
        Safe_Release(pInstnace);
        MSG_BOX("CItem_Manager::Create To Failed");
    }
    return pInstnace;
}

void CItem_Manager::Free()
{
    __super::Free();
}
