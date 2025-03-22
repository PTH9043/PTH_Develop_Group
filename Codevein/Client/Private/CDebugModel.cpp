#include "CDebugModel.h"
#include "CGameInstance.h"

CDebugModel::CDebugModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CGameObject(_pDevice, _pContext)
{
}

CDebugModel::CDebugModel(const CDebugModel& _rhs)
    : CGameObject(_rhs)
{
}

HRESULT CDebugModel::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

HRESULT CDebugModel::NativeConstruct_Clone(void* _pArg)
{
    if (__super::NativeConstruct_Clone(_pArg))
        return E_FAIL;

    return S_OK;
}

void CDebugModel::Tick(const _double& _dTimeDelta)
{
    __super::Tick(_dTimeDelta);
}

void CDebugModel::Late_Tick(const _double& _dTimeDelta)
{
    __super::Late_Tick(_dTimeDelta);

    if (nullptr != m_pRendererCom)
        m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CDebugModel::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    if (nullptr == m_pModelCom)
        return E_FAIL;

    if (FAILED(SetUp_ConstantTable()))
        return E_FAIL;

    _uint iNumMeshContainers = m_pModelCom->Get_NumMeshContainers();

    for (_uint i = 0; i < iNumMeshContainers; ++i)
    {
        if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TEXTYPE::TextureType_DIFFUSE)))
            return E_FAIL;

        //if (FAILED(m_pVIBufferCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
        //	return E_FAIL;
        //if (FAILED(m_pVIBufferCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
        //	return E_FAIL;

        m_pModelCom->Render_NonAnim(i, m_pShaderCom, 0);
    }

    return S_OK;
}

void CDebugModel::Set_Model(CModel* _pModel)
{
    if (nullptr == _pModel)
        return;
    
    m_pModelCom = _pModel;
}

HRESULT CDebugModel::Ready_Components()
{
    if (FAILED(Ready_Transform(COM_TRANSFORM)))
        return E_FAIL;

    /* For.Com_Renderer */
    if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, (CComponent**)&m_pRendererCom)))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXMODEL, (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CDebugModel::SetUp_ConstantTable()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
            return E_FAIL;
    }
    END_SINGLETON(pGameInstance);
    return S_OK;
}

CDebugModel* CDebugModel::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CDebugModel* pInstance = new CDebugModel(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CDebugModel::Create To Failed");
    }
    return pInstance;
}

CGameObject* CDebugModel::Clone(void* pArg)
{
    AddRef();
     
    return this;
} 

void CDebugModel::Free()
{
    __super::Free();

    Safe_Release(m_pRendererCom);
    Safe_Release(m_pShaderCom);
}
