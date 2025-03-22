#include "Sword_Tool.h"

CSword_Tool::CSword_Tool(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CItem_Tool(_pDevice, _pContext)
{
}

CSword_Tool::CSword_Tool(const CSword_Tool& _rhs)
    : CItem_Tool(_rhs)
{
}

HRESULT CSword_Tool::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSword_Tool::NativeConstruct_Clone(void* pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(pArg)))
        return E_FAIL;

    
    return S_OK;
}

void CSword_Tool::Tick(const _double& _dTimeDelta)
{
    __super::Tick(_dTimeDelta);
}

void CSword_Tool::Late_Tick(const _double& _dTimeDelta)
{
    __super::Late_Tick(_dTimeDelta);
}

HRESULT CSword_Tool::Render()
{
    return S_OK;
}

CSword_Tool* CSword_Tool::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
    CSword_Tool* pInstance = new CSword_Tool(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
#ifdef _DEBUG
        MSG_BOX("CSword_Tool::Create To Failed");
#endif
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CSword_Tool::Clone(void* pArg)
{
    CSword_Tool* pInstance = new CSword_Tool(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
    {
#ifdef _DEBUG
        MSG_BOX("CSword_Tool::Create To Failed");
#endif
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CSword_Tool::Free()
{
    __super::Free();
}
