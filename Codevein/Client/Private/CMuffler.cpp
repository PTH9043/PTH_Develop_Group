#include "CMuffler.h"
#include "CHierarchyNode.h"
#include "CGameInstance.h"

CMuffler::CMuffler(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CEquipment(_pDevice, _pContext)
{
}

CMuffler::CMuffler(const CMuffler& _rhs) 
    : CEquipment(_rhs)
{
}

HRESULT CMuffler::NativeConstruct_Prototype()
{
    return S_OK;
}

HRESULT CMuffler::NativeConstruct_Clone(void* pArg)
{
    if (nullptr != pArg)
    {
        MUFFLERDESC tMufflerDesc = *static_cast<MUFFLERDESC*>(pArg);
        m_tEquipDesc = tMufflerDesc.tEquipDesc;
        m_tItemDesc = tMufflerDesc.tItemDesc;

        // Find HierackyNode
        Find_HierackyNode(m_tEquipDesc.pOwnerObject, m_tEquipDesc.strAttachNodeName);
    }

    if (FAILED(Ready_Transform(COM_TRANSFORM)))
        return E_FAIL;
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXMODEL_EQUIP, (CComponent**)&m_pShaderCom)))
        return E_FAIL;
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, 
        (CComponent**)&m_pRendererCom)))
        return E_FAIL;
    if (FAILED(Reset_TransformData()))
        return E_FAIL;

    if (FAILED(__super::Add_Component(COM_MODEL, LEVEL_STATIC, PROTO_COMP_MODEL_PLAYERMUFFLER,
        ((CComponent**)&m_pModelCom))))
        return E_FAIL;

    _vector vPos = XMVectorSet(0.f, -120.f, +0.f, 0.f);
    vPos = XMVector3TransformCoord(vPos, m_pModelCom->Get_PivotMatrix());
    _float3 v3Pos;
    XMStoreFloat3(&v3Pos, vPos);

    m_pTransformCom->Set_Pos(vPos);
    return S_OK;
}

void CMuffler::Tick(const _double& _dTimeDelta)
{
    __super::Tick(_dTimeDelta);
}

void CMuffler::Late_Tick(const _double& _dTimeDelta)
{
    __super::Late_Tick(_dTimeDelta);

    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        if (true == pGameInstance->Is_Rendering(m_pTargetTransform, m_fDrawRadius))
        {
            m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
        }
    }
    END_SINGLETON(pGameInstance);
}

HRESULT CMuffler::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    if (nullptr == m_pModelCom)
        return E_FAIL;

    if (FAILED(SetUp_ConstantTable()))
        return E_FAIL;

    if (nullptr == m_pBindMatrixNode || nullptr == m_pTargetTransform)
        return E_FAIL;

    _float4x4 CombineMatrix, PivotMatrix;
    _float4x4 OwnerMatrixTP = m_pTargetTransform->Get_WorldFloat4x4_TP();

    XMStoreFloat4x4(&CombineMatrix, 
        XMMatrixTranspose(
            m_pBindMatrixNode->Get_CombinedTransformationMatrix()));


    XMStoreFloat4x4(&PivotMatrix, 
        XMMatrixTranspose(m_pTargetModel->Get_PivotMatrix()));


    if (FAILED(m_pShaderCom->Set_RawValue("g_CombineMatrix", &CombineMatrix, sizeof(_float4x4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Set_RawValue("g_PivotMatrix", &PivotMatrix, sizeof(_float4x4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Set_RawValue("g_TargetWorldMatrix", &OwnerMatrixTP, sizeof(_float4x4))))
        return E_FAIL;

    _uint iNumMeshContainers = m_pModelCom->Get_NumMeshContainers();

    for (_uint i = 0; i < iNumMeshContainers; ++i)
    {
        if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TEXTYPE::TextureType_DIFFUSE)))
        {
            return E_FAIL;
        }

        m_pModelCom->Render_NonAnim(i, m_pShaderCom, 1);
    }

    return S_OK;
}

HRESULT CMuffler::Find_HierackyNode(CGameObject* _pGameObject, const string& _str)
{
    if (nullptr == _pGameObject)
        return E_FAIL;

    CModel* pModel = static_cast<CModel*>(_pGameObject->Get_Component(COM_ANIMMODEL));
    if (nullptr == pModel)
        return E_FAIL;

    m_pBindMatrixNode = pModel->Get_HierarchyNodes(_str.c_str());
    Safe_AddRef(m_pBindMatrixNode);

    m_pBindPosNode = pModel->Get_HierarchyNodes("CHARA_OFFSET");
    Safe_AddRef(m_pBindPosNode);

    m_pTargetTransform = static_cast<CTransform*>(_pGameObject->Get_Component(COM_TRANSFORM));
    Safe_AddRef(m_pTargetTransform);
    if (nullptr == m_pTargetTransform)
        return E_FAIL;

    m_pTargetModel = static_cast<CModel*>(_pGameObject->Get_Component(COM_ANIMMODEL));
    Safe_AddRef(m_pTargetModel);
    if (nullptr == m_pTargetModel)
        return E_FAIL;

    return S_OK;
}

CMuffler* CMuffler::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
    CMuffler* pInstance = new CMuffler(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CMuffler::Create To Failed");
    }
    return pInstance;
}

CGameObject* CMuffler::Clone(void* _pArg)
{
    CMuffler* pInstance = new CMuffler(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CMuffler::Clone To Failed");
    }
    return pInstance;
}

void CMuffler::Free()
{
    Safe_Release(m_pBindMatrixNode);
    Safe_Release(m_pBindPosNode);
    Safe_Release(m_pTargetTransform);
    Safe_Release(m_pTargetModel);
    Safe_Release(m_pEquipMatrixNode);
    __super::Free();
}
