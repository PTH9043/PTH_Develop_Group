#include "CMainView.h"
#include "CImGui_Manager.h"
#include "CItem.h"
#include "CGameInstance.h"

CItem::CItem(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CPawn(_pDevice, _pContext)
{
}

CItem::CItem(const CItem& _rhs)
    : CPawn(_rhs),
    m_eItemType(_rhs.m_eItemType)
{
}

void CItem::Bind_Texture(CShader* _pShader, const _char* _pConstantName)
{
    if (nullptr != _pShader)
        return;

    m_pImgTexture->SetUp_ConstantTable_FileName(_pShader, _pConstantName, m_tItemDesc.strTextureName.c_str());
}

HRESULT CItem::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;

    m_ePawnType = CPawn::P_ITEM;
    return S_OK;
}

HRESULT CItem::NativeConstruct_Clone(void* pArg)
{
    if (nullptr != m_pCollCollider)
        m_fDrawRadius = sqrtf(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pCollCollider->Get_Scale()) * 2.f, XMLoadFloat3(&m_pCollCollider->Get_Scale()) * 2.f)));

    return S_OK;
}

void CItem::Tick(const _double& _dTimeDelta)
{
    __super::Tick(_dTimeDelta);
}

void CItem::Late_Tick(const _double& _dTimeDelta)
{
    __super::Late_Tick(_dTimeDelta);
}

HRESULT CItem::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    if (FAILED(SetUp_ConstantTable()))
        return E_FAIL;

    return S_OK;
}
HRESULT CItem::Ready_Component()
{
    LEVEL eLevel = LEVEL_END;
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        eLevel = static_cast<LEVEL>(pGameInstance->Get_CurLevel());
    }
    END_SINGLETON(pGameInstance);
    /* For.Com_Renderer */
    if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, (CComponent**)&m_pRendererCom)))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXMODEL, (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(COM_TEXTURE, LEVEL_STATIC, PROTO_COMP_TEXTURE_UIGROUP, (CComponent**)&m_pImgTexture)))
        return E_FAIL;

    return S_OK;
}

HRESULT CItem::SetUp_ConstantTable()
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

#pragma region USE_TOOL_METHOD
_bool CItem::Extern_EditValue()
{
    return _bool();
}

_bool CItem::Extern_HierakeyView(const string& _str)
{
    return _bool();
}

HRESULT CItem::Create_Self(CGameObject** _ppOut)
{
    return E_NOTIMPL;
}

HRESULT CItem::Delete_Self()
{
    return E_NOTIMPL;
}

HRESULT CItem::Save_Data()
{
    return E_NOTIMPL;
}

HRESULT CItem::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
    return E_NOTIMPL;
}

_bool CItem::Is_Picking(_float3* _pOut)
{
    return _bool();
}

void CItem::Reset_Data()
{
}
void CItem::Extern_PawnData()
{
    __super::Extern_PawnData();
    ImGui::DragFloat("Weight", &m_tPawnStatus.fWeight);
    ImGui::DragFloat("Soul", &m_tPawnStatus.fSoul);
    ImGui::DragFloat("Exp", &m_tPawnStatus.fExp);
}
#pragma endregion USE_TOOL_METHOD

void CItem::Free()
{

    Safe_Release(m_pImgTexture);
    __super::Free();
}
