#include "CMainView.h"
#include "CImGui_Manager.h"
#include "CWeapon.h"
#include "CGameInstance.h"
#include "CCharacter.h"

CWeapon::CWeapon(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CEquipment(_pDevice, _pContext)
{
    m_eEquipType = EQUIP_WEAPON;
}

CWeapon::CWeapon(const CWeapon& _rhs)
    : CEquipment(_rhs), 
    m_eWeaponType(_rhs.m_eWeaponType)
{
}

_wstring CWeapon::Get_WeaponName()
{
    if (nullptr == m_pFileGroup)
        return L"";
    return m_pFileGroup->strUppderName;
}

HRESULT CWeapon::Add_CheckColliderList(CPawn* _pPawnObject)
{
    if (nullptr == _pPawnObject)
        return E_FAIL; 

    auto iter = find_if(m_CollisionCheckList.begin(), m_CollisionCheckList.end(), [&](auto& Pawn)->bool
    {
        if (Pawn == _pPawnObject)
            return true;

        return false;
    });

    if (m_CollisionCheckList.end() != iter)
        return E_FAIL;

    m_CollisionCheckList.push_back(_pPawnObject);
    return S_OK;
}

HRESULT CWeapon::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CWeapon::NativeConstruct_Clone(void* pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(pArg)))
        return E_FAIL;

    Receive_ModelFiles();
    return S_OK;
}

void CWeapon::Tick(const _double& _dTimeDelta)
{
    __super::Tick(_dTimeDelta);
}

void CWeapon::Late_Tick(const _double& _dTimeDelta)
{
    __super::Late_Tick(_dTimeDelta);

    Check_Collision();
    Update_Transform();

    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        if(true == pGameInstance->Is_Rendering(m_pTargetTransform, m_fDrawRadius))
        {
            m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
#ifdef _DEBUG
            Add_Collider_Render();
#endif
        }
    }
    END_SINGLETON(pGameInstance);
}

HRESULT CWeapon::Render()
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
        {
            return E_FAIL;
        }

        m_pModelCom->Render_NonAnim(i, m_pShaderCom, 1);
    }

    return S_OK;
}

HRESULT CWeapon::Find_HierackyNode(CGameObject* _pGameObject, const string& _str)
{
    if (nullptr == _pGameObject)
        return E_FAIL;

    CModel* pModel = static_cast<CModel*>(_pGameObject->Get_Component(COM_ANIMMODEL));
    if (nullptr == pModel)
        return E_FAIL;

    m_pBindMatrixNode = pModel->Get_HierarchyNodes(_str.c_str());
    Safe_AddRef(m_pBindMatrixNode);

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

CEquipment* CWeapon::ReturnSettingToValue(CCharacter* _pCharacter)
{
    if (nullptr == _pCharacter)
        return this;

    Bind_Weapon_Status(_pCharacter);
    return this;
}

void CWeapon::Bind_Weapon_Status(CCharacter* _pCharacter)
{
    m_tSaveStatus = m_tPawnStatus + _pCharacter->Get_PawnStatus();
    m_tCharInfo.UpdateInfo(m_tSaveStatus);
}

HRESULT CWeapon::Ready_Component()
{
    if (FAILED(Ready_Transform(COM_TRANSFORM)))
    {
        return E_FAIL;
    }
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXMODEL_EQUIP, (CComponent**)&m_pShaderCom)))
    {
        return E_FAIL;
    }
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, (CComponent**)&m_pRendererCom)))
    {
        return E_FAIL;
    }
    if (FAILED(Reset_TransformData()))
    {
        return E_FAIL;
    }

    return S_OK;
}
HRESULT CWeapon::SetUp_ConstantTable()
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

    if(FAILED(Bind_Transform()))
        return E_FAIL;

    return S_OK;
}

HRESULT CWeapon::Update_Transform()
{
    if (nullptr == m_pBindMatrixNode || nullptr == m_pTargetTransform || nullptr == m_pCollCollider)
        return E_FAIL;

    _float4x4 mWorld;
    XMStoreFloat4x4(&mWorld, 
        m_pBindMatrixNode->Get_CombinedTransformationMatrix() * m_pModelCom->Get_PivotMatrix() * m_pTargetModel->Get_PivotMatrix() * m_pTargetTransform->Get_WorldMatrix());

    _float3 v3Translate = m_pCollCollider->Get_Translate();

    XMStoreFloat3((_float3*)&mWorld.m[0][0], XMVector4Normalize(XMLoadFloat4((_float4*)&mWorld.m[0][0])) * m_pCollCollider->Get_Scale().z);
    XMStoreFloat3((_float3*)&mWorld.m[1][0], -XMVector4Normalize(XMLoadFloat4((_float4*)&mWorld.m[1][0])) * m_pCollCollider->Get_Scale().y);
    XMStoreFloat3((_float3*)&mWorld.m[2][0], -XMVector4Normalize(XMLoadFloat4((_float4*)&mWorld.m[2][0])) * m_pCollCollider->Get_Scale().z);

    Coll_Tick(XMLoadFloat4x4(&mWorld));

    m_SocketMatrix = mWorld;
    XMStoreFloat3((_float3*)&m_SocketMatrix.m[3][0], m_pCollCollider->Get_CurPos());
    return S_OK;
}

HRESULT CWeapon::Bind_Transform()
{
    if (nullptr == m_pBindMatrixNode || nullptr == m_pTargetTransform)
        return E_FAIL;

    _float4x4 CombineMatrix, PivotMatrix;
    _float4x4 OwnerMatrixTP = m_pTargetTransform->Get_WorldFloat4x4_TP();

    XMStoreFloat4x4(&CombineMatrix, XMMatrixTranspose(m_pBindMatrixNode->Get_CombinedTransformationMatrix()));
    XMStoreFloat4x4(&PivotMatrix, XMMatrixTranspose(m_pTargetModel->Get_PivotMatrix()));


    if (FAILED(m_pShaderCom->Set_RawValue("g_CombineMatrix", &CombineMatrix, sizeof(_float4x4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Set_RawValue("g_PivotMatrix", &PivotMatrix, sizeof(_float4x4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Set_RawValue("g_TargetWorldMatrix", &OwnerMatrixTP, sizeof(_float4x4))))
        return E_FAIL;
 
    return S_OK;
}

void CWeapon::Setting_WeaponPivotPos(const _fvector& _vPivotPos, const _fvector& _vPivotScale)
{
    if (nullptr != m_pCollCollider)
    {
        _float3 v3Tranlate;
        XMStoreFloat3(&v3Tranlate, XMLoadFloat3(&m_pCollCollider->Get_Translate()) + _vPivotPos);
        m_pCollCollider->Set_Translate(v3Tranlate);


        _float3 v3Scale;
        XMStoreFloat3(&v3Scale, XMLoadFloat3(&m_pCollCollider->Get_Scale())+_vPivotScale);
        m_pCollCollider->Set_Scale(v3Scale);
    }

    if (nullptr != m_pFirstCheckCollider)
    {
        _float3 v3Tranlate;
        XMStoreFloat3(&v3Tranlate, XMLoadFloat3(&m_pFirstCheckCollider->Get_Translate()) + _vPivotPos);
        m_pFirstCheckCollider->Set_Translate(v3Tranlate);


        _float3 v3Scale;
        XMStoreFloat3(&v3Scale, XMLoadFloat3(&m_pFirstCheckCollider->Get_Scale()) + _vPivotScale);
        m_pFirstCheckCollider->Set_Scale(v3Scale);
    }
}

void CWeapon::Check_Collision()
{
    if (nullptr == m_pFirstCheckCollider)
        return;

    if (nullptr == m_pCollCollider)
        return;

    for (auto& iter : m_CollisionCheckList)
    {
        if (true == iter->Is_Collision(m_pCollCollider))
            iter->Damaged(this);
    }
    m_CollisionCheckList.clear();
}

void CWeapon::Coll_Tick(_matrix matWorld)
{
    __super::Coll_Tick(matWorld);
    if (nullptr != m_pFirstCheckCollider)
        m_pFirstCheckCollider->Tick(matWorld);
}
#ifdef _DEBUG
_bool CWeapon::Add_Collider_Render()
{
    if (true == __super::Add_Collider_Render())
    {
        if (nullptr != m_pFirstCheckCollider)
            m_pRendererCom->Add_Components(m_pFirstCheckCollider);

        return true;
    }
    return false;
}
#endif
#pragma region USE_TOOL_METHOD

void CWeapon::Initialize_InsertData()
{

}

_bool CWeapon::Extern_EditValue()
{
    _bool bActive = false;

    if (ImGui::TreeNodeEx("Weapon"))
    {
        ImGui::PushItemWidth(200);
        bActive = ImGui::Button("Create");
        ImGui::PopItemWidth();
        ImGui::TreePop();
        return bActive;
    }
    return false;
}

_bool CWeapon::Extern_HierakeyView(const string& _str)
{
     if (nullptr == m_pTransformCom)
        return false;

    if (ImGui::TreeNodeEx(_str.c_str()))
    {
        ImGui::PushItemWidth(200);
        Control_ColliderData();
        ImGui::PopItemWidth();
        ImGui::TreePop();
    }
    return false;
}

HRESULT CWeapon::Create_Self(CGameObject** _ppOut)
{
    return E_NOTIMPL;
}

HRESULT CWeapon::Delete_Self()
{
    return E_NOTIMPL;
}

HRESULT CWeapon::Save_Data()
{
    return E_NOTIMPL;
}

HRESULT CWeapon::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
    return E_NOTIMPL;
}

_bool CWeapon::Is_Picking(_float3* _pOut)
{
    return _bool();
}

void CWeapon::Reset_Data()
{
}

#pragma endregion USE_TOOL_METHOD

void CWeapon::Free()
{
    Safe_Release(m_pFirstCheckCollider);
    Safe_Release(m_pTargetModel);
    Safe_Release(m_pBindMatrixNode);
    Safe_Release(m_pEquipMatrixNode);
    Safe_Release(m_pTargetTransform);
    __super::Free();
}
