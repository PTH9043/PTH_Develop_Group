#include "Camera_CutScene.h"
#include "GameInstance.h"

CCamera_CutScene::CCamera_CutScene(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CCamera(_pDevice, _pContext)
{
}

CCamera_CutScene::CCamera_CutScene(const CCamera_CutScene& _rhs)
    : CCamera(_rhs)
{
}

HRESULT CCamera_CutScene::NativeConstruct_Prototype()
{
    return S_OK;
}

HRESULT CCamera_CutScene::NativeConstruct_Clone(void* _pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(_pArg)))
        return E_FAIL;

    m_bActive = false;
    return S_OK;
}

void CCamera_CutScene::Tick(const _double& _dTimeDelta) {

    if (false == m_bActive)
        return;

    __super::Tick(_dTimeDelta);
}

void CCamera_CutScene::Late_Tick(const _double& _dTimeDelta) {
    if (false == m_bActive)
        return;
    __super::Late_Tick(_dTimeDelta);
}

HRESULT CCamera_CutScene::Render()
{
    return S_OK;
}

CCamera_CutScene* CCamera_CutScene::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CCamera_CutScene* pInstance = new CCamera_CutScene(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        Safe_Release(pInstance);
#ifdef _DEBUG
        MSG_BOX("CCamera_CutScene::Create To Failed");
#endif
    }
    return pInstance;
}

CGameObject* CCamera_CutScene::Clone(void* pArg)
{
    CCamera_CutScene* pInstance = new CCamera_CutScene(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
    {
        Safe_Release(pInstance);
#ifdef _DEBUG
        MSG_BOX("CCamera_CutScene::Clone To Failed");
#endif
    }
    return pInstance;
}

void CCamera_CutScene::Free()
{
    __super::Free();
}
