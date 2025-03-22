#include "CCubeObj.h"
#include "CGameInstance.h"

CCubeObj::CCubeObj(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice(_pDevice), m_pContext(_pContext)
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}
#ifdef _DEBUG
void CCubeObj::Set_Color(const _vector& _v4Color)
{
    if (nullptr == m_pCollider)
        return;

    m_pCollider->Set_Color(_v4Color);
}
#endif
void CCubeObj::Set_Position(const _vector& _vPos)
{
    if (nullptr == m_pTransform)
        return;

    m_pTransform->Set_Pos(_vPos);
}

HRESULT CCubeObj::NativeConstruct(const _vector& _vPosition, const _uint& _iIndex)
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        CCollider::COLLIDERDESC tCollDesc;
        tCollDesc.vScale = _float3(0.5f, 0.5f, 0.5f);
        tCollDesc.vTranslation = _float3(0.f, 0.f, 0.f);
#ifdef _DEBUG
        m_pCollider = static_cast<CCollider*>(pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_AABBCOLLIDER, &tCollDesc));
#endif
        m_pVIBufferCube = static_cast<CVIBuffer_Cube*>(pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_VIBUFFER_CUBE));
        m_pTransform = CTransform::Create(m_pDevice, m_pContext);
        m_pTransform->Set_Pos(_vPosition);
        m_pTransform->Set_Scale(_float3(0.5f, 0.5f, 0.5f));
        m_iIndex = _iIndex;
    }
    END_SINGLETON(pGameInstance);
    return S_OK;
}
#ifdef _DEBUG
HRESULT CCubeObj::Render()
{
#ifdef _DEBUG
        m_pCollider->Render();
#endif // DEBUG
    return S_OK;
}
#endif
#ifdef _DEBUG
HRESULT CCubeObj::Add_Render(CRenderer* _pRenderer)
{
    if (nullptr == _pRenderer)
        return E_FAIL;

    m_pCollider->Tick(m_pTransform->Get_WorldMatrix());
    _pRenderer->Add_Components(m_pCollider);
    return S_OK;
}
#endif // DEBUG

_bool CCubeObj::Is_Picking(_float3* _v3Pos)
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance)
    {
        if (pGameInstance->Picking_Mesh(nullptr, m_pTransform, m_pVIBufferCube, _v3Pos))
        {
            END_SINGLETON(pGameInstance);
            return true;
        }
    }
    END_SINGLETON(pGameInstance);
    return false;
}

CCubeObj* CCubeObj::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _vector& _vPosition, const _uint& _iIndex)
{     
    CCubeObj* pInstance = new CCubeObj(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct(_vPosition, _iIndex)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CCubeObj::Create To Failed");
    }
    return pInstance;
}

void CCubeObj::Free()
{
#ifdef _DEBUG
    Safe_Release(m_pCollider);
#endif
    Safe_Release(m_pVIBufferCube);
    Safe_Release(m_pTransform);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    __super::Free();
}
