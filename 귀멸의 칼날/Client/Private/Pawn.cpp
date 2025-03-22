#include "Pawn.h"
#include "GameInstance.h"

CPawn::CPawn(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CGameObject(_pDevice, _pContext)
{
    m_PrevWorldMatrix = CMethod::XMMatrixIdentity_P();
}

CPawn::CPawn(const CPawn& _rhs)
    : CGameObject(_rhs), 
    m_ePawnType(_rhs.m_ePawnType), 
    m_PrevWorldMatrix(_rhs.m_PrevWorldMatrix),
    m_bWrapSampler(_rhs.m_bWrapSampler)
{
}

HRESULT CPawn::NativeConstruct_Prototype()
{
    return S_OK;
}

HRESULT CPawn::NativeConstruct_Clone(void* pArg)
{
    if (FAILED(SetUp_Components()))
        return E_FAIL;

    return S_OK;
}

void CPawn::Tick(const _double& _dTimeDelta)
{
}

void CPawn::Late_Tick(const _double& _dTimeDelta)
{
#ifdef _DEBUG
     //Coll_Renderer();
#endif
  
}

HRESULT CPawn::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    if (FAILED(SetUp_ConstantTable()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPawn::RenderLight()
{
    if (FAILED(__super::RenderLight()))
        return E_FAIL;
    
    return S_OK;
}

_bool CPawn::Hit_Pushed(CPawn* _pDamageOwner, VOIDDATAS* _pOtherDatas)
{
    RETURN_IFNOT(_pDamageOwner == nullptr, false);

    return true;
}

HRESULT CPawn::Collision_Situation(const _double& _dTimeDelta)
{
    return S_OK;
}

HRESULT CPawn::SetUp_ConstantTable()
{
    if (nullptr == m_pTransformCom || nullptr == m_pShaderCom)
        return E_FAIL;

    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        _float4x4 WorldMatrix, ViewMatrix, ProjMatrix, PrevViewMatrix;
        WorldMatrix = m_pTransformCom->Get_WorldFloat4x4_TP();
        ViewMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW);
        ProjMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ);
        PrevViewMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PREVVIEW);
        _float camFar = pGameInstance->Get_CamFar();

        if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
        {
            RELEASE_INSTANCE(CGameInstance);
            return E_FAIL;
        }
        if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
        {
            RELEASE_INSTANCE(CGameInstance);
            return E_FAIL;
        }
        if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
        {
            RELEASE_INSTANCE(CGameInstance);
            return E_FAIL;
        }
        if (FAILED(m_pShaderCom->Set_RawValue("g_CamFar", &camFar, sizeof(_float))))
        {
            RELEASE_INSTANCE(CGameInstance);
            return E_FAIL;
        }
        if (FAILED(m_pShaderCom->Set_RawValue("g_PrevWorldMatrix", &m_PrevWorldMatrix, sizeof(_float4x4))))
        {      
            RELEASE_INSTANCE(CGameInstance);
            return E_FAIL;
        }
        if (FAILED(m_pShaderCom->Set_RawValue("g_PrevViewMatrix", &PrevViewMatrix, sizeof(_float4x4))))
        {
            RELEASE_INSTANCE(CGameInstance);
            return E_FAIL;
        }
        if (FAILED(m_pShaderCom->Set_RawValue("g_bWrapSampler", &m_bWrapSampler, sizeof(_bool))))
        {
            RELEASE_INSTANCE(CGameInstance);
            return E_FAIL;
        }
        m_PrevWorldMatrix = WorldMatrix;
    }
    RELEASE_INSTANCE(CGameInstance);

    return S_OK;
}

HRESULT CPawn::SetUp_Components()
{
    if (FAILED(Ready_Transform(COM_TRANSFORM)))
        return E_FAIL;

    /* For.Com_Renderer */
    if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, (CComponent**)&m_pRendererCom)))
        return E_FAIL;

    return S_OK;
}

void CPawn::Coll_Init()
{
    if (nullptr != m_pColliderCom)
        m_pColliderCom->Init_Collision();
}

void CPawn::Coll_Tick(_matrix _WorldMatrix)
{
    if (nullptr != m_pColliderCom)
        m_pColliderCom->Tick(_WorldMatrix);

}
#ifdef _DEBUG
void CPawn::Coll_Render()
{
    if (nullptr != m_pColliderCom)
        m_pColliderCom->Render_Debug();
}
void CPawn::Coll_Renderer()
{
    if (nullptr != m_pColliderCom && nullptr != m_pRendererCom)
        m_pRendererCom->Add_Components(CRenderer::DEBUG_PLAYER, m_pColliderCom);
}
#endif
CGameObject* CPawn::Clone(void* pArg)
{
    return nullptr;
}

void CPawn::Free()
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pShaderCom);
    __super::Free();
}
