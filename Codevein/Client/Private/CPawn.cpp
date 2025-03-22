#include "CMainView.h"
#include "CImGui_Manager.h"
#include "CPawn.h"
#include "CGameInstance.h"
#include "CData_Manager.h"

CPawn::CPawn(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CGameObject_Client(_pDevice, _pContext)
{
}

CPawn::CPawn(const CPawn& _rhs)
    : CGameObject_Client(_rhs), m_ePawnType(_rhs.m_ePawnType)
{
}

HRESULT CPawn::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;
    return S_OK;
}

HRESULT CPawn::NativeConstruct_Clone(void* pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(pArg)))
        return E_FAIL;
    return S_OK;
}

void CPawn::Tick(const _double& _dTimeDelta)
{
    __super::Tick(_dTimeDelta);
}

void CPawn::Late_Tick(const _double& _dTimeDelta)
{
    __super::Late_Tick(_dTimeDelta);
}

HRESULT CPawn::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;


#ifdef _DEBUG
    Add_Collider_Render();
#endif // DEBUG

    return S_OK;
}
HRESULT CPawn::Damaged(CPawn* _pDamageOwner)
{
    if (nullptr == _pDamageOwner)
        return E_FAIL;


    return S_OK;
}
HRESULT CPawn::Ready_Component()
{
    /* For.Com_Renderer */
    if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, (CComponent**)&m_pRendererCom)))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXMODEL, (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    return S_OK;
}
HRESULT CPawn::SetUp_ConstantTable()
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

void CPawn::Coll_Tick(_matrix matWorld)
{
    if (nullptr != m_pCollCollider)
        m_pCollCollider->Tick(matWorld);
}
#ifdef _DEBUG
_bool CPawn::Add_Collider_Render()
{
    if (nullptr != m_pRendererCom)
    {
        if (nullptr != m_pCollCollider)
        {
            m_pRendererCom->Add_Components(m_pCollCollider);
            return true;
        }
    }
    return false;
}
#endif
#pragma region USE_TOOL_METHOD
_bool CPawn::Extern_EditValue()
{
    return _bool();
}

_bool CPawn::Extern_HierakeyView(const string& _str)
{
    return _bool();
}

HRESULT CPawn::Create_Self(CGameObject** _ppOut)
{
    return E_NOTIMPL;
}

HRESULT CPawn::Delete_Self()
{
    return E_NOTIMPL;
}

HRESULT CPawn::Save_Data()
{
    return E_NOTIMPL;
}

HRESULT CPawn::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
    return E_NOTIMPL;
}

_bool CPawn::Is_Picking(_float3* _pOut)
{
    _bool bIsPicking = false;
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        for (auto& iter : m_pModelCom->Get_MeshContainers())
        {
            bIsPicking = pGameInstance->Picking_Mesh(this, m_pTransformCom, (CVIBuffer*)iter, _pOut);
        }
    }
    END_SINGLETON(pGameInstance);
    return bIsPicking;
}

void CPawn::Reset_Data()
{
    Reset_TransformData();
}
#pragma endregion USE_TOOL_METHOD

void CPawn::Extern_PawnData()
{
    PAWNSTATUS PawnStatus = m_tPawnStatus;

    ImGui::DragInt("Health", (_int*) & PawnStatus.iHealth, 1.f, 0, 100);
    ImGui::DragInt("Energy", (_int*)&PawnStatus.iEnergy, 1.f, 0, 100);
    ImGui::DragInt("Power", (_int*)&PawnStatus.iPower, 1.f, 0, 100);
    ImGui::DragInt("Speed", (_int*)&PawnStatus.iSpeed, 1.f, 0, 100);
    ImGui::DragInt("Shield", (_int*)&PawnStatus.iShield, 1.f, 0, 100);
    m_tPawnStatus = PawnStatus;
}

void CPawn::Control_ColliderData()
{
    if (nullptr != m_pCollCollider)
    {
        if (ImGui::TreeNodeEx("CollCollider"))
        {
            COLLDATA tCollData;
            tCollData.v3CollScale = m_pCollCollider->Get_Scale();
            tCollData.v3Translation = m_pCollCollider->Get_Translate();

            _float Translate[3] = { tCollData.v3Translation.x,  tCollData.v3Translation.y, tCollData.v3Translation.z };
            _float CollSclae[3] = { tCollData.v3CollScale.x ,  tCollData.v3CollScale.y , tCollData.v3CollScale.z  };

            if (ImGui::InputFloat3("CollT", Translate, "%.1f"))
            {
                m_pCollCollider->Set_Translate(_float3(Translate[0], Translate[1], Translate[2]));
            }
            if (ImGui::InputFloat3("CollS", CollSclae, "%.1f"))
            {
                m_pCollCollider->Set_Scale(_float3(CollSclae[0], CollSclae[1], CollSclae[2]));
            }

            ImGui::TreePop();
        }
    }
}

void CPawn::Free()
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pCollCollider);

    __super::Free();
}
