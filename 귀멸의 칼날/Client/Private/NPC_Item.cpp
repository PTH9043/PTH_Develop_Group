#include "NPC_Item.h"
#include "Renderer.h"
#include "GameInstance.h"
#include "Sheath.h"
#include "NPC.h"

CNPC_Item::CNPC_Item(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CItem(_pDevice, _pContext)
{
}

CNPC_Item::CNPC_Item(const CNPC_Item& _rhs)
    : CItem(_rhs)
{
}

HRESULT CNPC_Item::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;
    return S_OK;
}

HRESULT CNPC_Item::NativeConstruct_Clone(void* pArg)
{
    RETURN_IFNOT(nullptr == pArg, E_FAIL);

	CSheath::ITEMDESC tItemDesc = *reinterpret_cast<CSheath::ITEMDESC*>(pArg);
	if(tItemDesc.strModelProtoName == PROTO_COMP_MODEL_LANTERN_L)
		m_iType = Item1;
    if (tItemDesc.strModelProtoName == PROTO_COMP_MODEL_LANTERN_R)
        m_iType = Item2;

	
    if (FAILED(__super::NativeConstruct_Clone(pArg)))
        return E_FAIL;

	Setting_Pos(m_iType);
	
    return S_OK;
}

void CNPC_Item::Tick(const _double& _dTimeDelta)
{
    __super::Tick(_dTimeDelta);

}

void CNPC_Item::Late_Tick(const _double& _dTimeDelta)
{
    if (nullptr != m_pRendererCom)
    {
        m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
    }
}

HRESULT CNPC_Item::Render()
{
    if (FAILED(SetUp_ConstantTable()))
        return E_FAIL;

    if (nullptr != m_pModelCom)
    {
        _uint iNumMeshContainers = m_pModelCom->Get_NumMeshContainers();

        for (_uint i = 0; i < iNumMeshContainers; ++i)
        {
            if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE)))
                return E_FAIL;

            m_pModelCom->Render_NonAnim(i, m_pShaderCom, 10);
        }
    }

    return S_OK;
}

void CNPC_Item::Extern_ImGuiClass()
{
}

HRESULT CNPC_Item::SetUp_ConstantTable()
{
    if (FAILED(__super::SetUp_ConstantTable()))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_Item::SetUp_Components()
{
    if (FAILED(__super::SetUp_Components()))
        return E_FAIL;
    return S_OK;
}

void CNPC_Item::Setting_Pos(_int Type)
{
    switch (Type)
    {
    case Item1:
        m_pTransformCom->Set_PosF3(_float3(1.9f,2.85f, 0.45f));
        m_pTransformCom->Rotate_Turn(_float3(24.4f, -29.15f, 83.15f));
        break;
    case Item2:
        m_pTransformCom->Set_PosF3(_float3(0.2f,-2.65f, -0.5f));
        m_pTransformCom->Rotate_Turn(_float3(-25.45f, 0.f, 4.8f));
        break;
    }
}

CNPC_Item* CNPC_Item::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
    CNPC_Item* pInstance = new CNPC_Item(_pDevice, _pContext);
    CHECK_CREATE_PROTOTYPE_NOTVALUE(pInstance, "CNPC_Item::Create To Failed");
    return pInstance;
}

CGameObject* CNPC_Item::Clone(void* pArg)
{
    CNPC_Item* pInstance = new CNPC_Item(*this);
    CHECK_CREATE_CLONE(pInstance, pArg, "CNPC_Item::Clone To Failed");
    return pInstance;
}

void CNPC_Item::Free()
{
    __super::Free();
}
