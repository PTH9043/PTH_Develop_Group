#include "CMainView.h"
#include "CImGui_Manager.h"
#include "CSkyBox.h"
#include "CGameInstance.h"
#include "CData_Manager.h"

CSkyBox::CSkyBox(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CGameObject(_pDevice, _pContext)
{
}

CSkyBox::CSkyBox(const CSkyBox& _rhs)
    : CGameObject(_rhs)
{
}

HRESULT CSkyBox::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSkyBox::NativeConstruct_Clone(void* pArg)
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;


    if (nullptr != pArg)
        m_tSkyBoxDesc = *static_cast<SKYBOXDESC*>(pArg);
    return S_OK;
}

void CSkyBox::Tick(const _double& _dTimeDelta)
{
    __super::Tick(_dTimeDelta);
}

void CSkyBox::Late_Tick(const _double& _dTimeDelta)
{
    __super::Late_Tick(_dTimeDelta);

    if (nullptr != m_pRendererCom)
        m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CSkyBox::Render()
{
    if (nullptr == m_pTexture || nullptr == m_pVIBufferCube)
        return E_FAIL;

    if (FAILED(SetUp_SRV()))
        return E_FAIL;

    m_pShaderComp->Begin(1);

    m_pVIBufferCube->Render();
    return S_OK;
}

HRESULT CSkyBox::Ready_Component()
{
    if (FAILED(Ready_Transform(COM_TRANSFORM)))
        return E_FAIL;

    if (FAILED(Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER,
        (CComponent**)&m_pRendererCom)))
        return E_FAIL;

    if (FAILED(Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXCUBE,
        (CComponent**)&m_pShaderComp)))
        return E_FAIL;

    if (FAILED(Add_Component(COM_VIBUFFER, LEVEL_STATIC, PROTO_COMP_VIBUFFER_CUBE,
        (CComponent**)&m_pVIBufferCube)))
        return E_FAIL;

    if (FAILED(Add_Component(COM_TEXTURE, LEVEL_STATIC, PROTO_COMP_TEXTURE_SKYBOXGROUP, 
        (CComponent**)&m_pTexture)))
        return E_FAIL;

    return S_OK;
}

HRESULT CSkyBox::SetUp_SRV()
{
    if (nullptr == m_pShaderComp)
        return E_FAIL;

    if (FAILED(m_pShaderComp->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
        return E_FAIL;

    BEGIN_SINGLETON(CGameInstance, pGameInstance)
    {
        if (FAILED(m_pShaderComp->Set_RawValue("g_ViewMatrix", 
           & pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
            return E_FAIL;

        if (FAILED(m_pShaderComp->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
            return E_FAIL;
    }
    END_SINGLETON(pGameInstance);

    if (FAILED(m_pTexture->SetUp_ConstantTable_FileName(m_pShaderComp, "g_DiffuseTexture", m_tSkyBoxDesc.strSkyBoxName.c_str())))
        return E_FAIL;

    if (FAILED(m_pShaderComp->Set_RawValue("g_PadeOut", &m_pRendererCom->Get_PadeOutValue(), sizeof(_float))))
        return E_FAIL;

    return S_OK;
}

CSkyBox* CSkyBox::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CSkyBox* pInstance = new CSkyBox(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CSkyBox::Create To Failed");
    }
    return pInstance;
}

CGameObject* CSkyBox::Clone(void* _pArg)
{
    CSkyBox* pInstance = new CSkyBox(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CSkyBox::Clone To Failed");
    }
    return pInstance;
}

void CSkyBox::Free()
{
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pShaderComp);
    Safe_Release(m_pTexture);
    Safe_Release(m_pVIBufferCube);

    __super::Free();
}
