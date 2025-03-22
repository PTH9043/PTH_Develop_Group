#include "CModel_FBX.h"
#include "CShowObject.h"
#include "CGameInstance.h"

CShowObject::CShowObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CGameObject(_pDevice, _pContext)
{
}

CShowObject::CShowObject(const CShowObject& _rhs)
    : CGameObject(_rhs)
{
}

void CShowObject::Set_Model_FBX(CModel_FBX* _pModelFBX)
{
    Safe_Release(m_pModelFBXCom);
    {
        m_pModelFBXCom = _pModelFBX;
    }
    Safe_AddRef(m_pModelFBXCom);
}

void CShowObject::Set_Model(CModel* _pModel)
{
    Safe_Release(m_pModelCom);
    {
        m_pModelCom = _pModel;
    }
    Safe_AddRef(m_pModelCom);
}

HRESULT CShowObject::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;
    return S_OK;
}

HRESULT CShowObject::NativeConstruct_Clone(void* _pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CShowObject::Tick(const _double& _dTimeDelta)
{
    __super::Tick(_dTimeDelta);
}

void CShowObject::Late_Tick(const _double& _dTimeDelta)
{
    __super::Late_Tick(_dTimeDelta);

    if (false == m_bActive)
        return;

    if (nullptr != m_pRendererCom)
        m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CShowObject::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;


    if(FAILED(SetUp_ConstantTable()))
        return E_FAIL;

    if (nullptr != m_pModelFBXCom && nullptr == m_pModelCom)
    {
        _uint iNumMeshContainers = m_pModelFBXCom->Get_NumMeshContainers();

        for (_uint i = 0; i < iNumMeshContainers; ++i)
        {
            if (FAILED(m_pModelFBXCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType::aiTextureType_DIFFUSE)))
                return E_FAIL;

            //if (FAILED(m_pVIBufferCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
            //	return E_FAIL;
            //if (FAILED(m_pVIBufferCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
            //	return E_FAIL;

            m_pShaderCom->Begin(0);

            m_pModelFBXCom->Render(i, m_pShaderCom, 0);
        }
    }
    else  if(nullptr != m_pModelCom)
    {
        _uint iNumMeshContainers = m_pModelCom->Get_NumMeshContainers();

        for (_uint i = 0; i < iNumMeshContainers; ++i)
        {
            if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TEXTYPE::TextureType_DIFFUSE)))
                return E_FAIL;

            //if (FAILED(m_pVIBufferCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
            //	return E_FAIL;
            //if (FAILED(m_pVIBufferCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
            //	return E_FAIL;

            m_pShaderCom->Begin(0);

            m_pModelCom->Render(i, m_pShaderCom, 0);
        }
    }

    return S_OK;
}

HRESULT CShowObject::Ready_Components()
{
    if (FAILED(Ready_Transform(COM_TRANSFORM)))
        return E_FAIL;

    m_pTransformCom->Set_Scale(_float3(0.01f, 0.01f, 0.01f));

    /* For.Com_Renderer */
    if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, (CComponent**)&m_pRendererCom)))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXMODEL, (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CShowObject::SetUp_ConstantTable()
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

        const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc();
        if (nullptr != pLightDesc)
        {
            if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->v4Diffuse, sizeof(_float4))))
                return E_FAIL;
            if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->v4Ambient, sizeof(_float4))))
                return E_FAIL;
            if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->v4Specular, sizeof(_float4))))
                return E_FAIL;
            if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPositionF3(), sizeof(_float3))))
                return E_FAIL;
        }
    }
    END_SINGLETON(pGameInstance);
    return S_OK;
}

CShowObject* CShowObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CShowObject* pGameObject = new CShowObject(_pDevice, _pContext);
    if (FAILED(pGameObject->NativeConstruct_Prototype()))
    {
        Safe_Release(pGameObject);
        MSG_BOX("CShowObject::Create To Failed");
    }
    return pGameObject;
}

CGameObject* CShowObject::Clone(void* pArg)
{
    CShowObject* pGameObject = new CShowObject(*this);
    if (FAILED(pGameObject->NativeConstruct_Clone(pArg)))
    {
        Safe_Release(pGameObject);
        MSG_BOX("CShowObject::Clone To Failed");
    }
    return pGameObject;
}

void CShowObject::Free()
{
    Safe_Release(m_pModelFBXCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
