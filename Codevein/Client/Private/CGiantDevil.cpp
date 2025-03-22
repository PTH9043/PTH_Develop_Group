#include "CMainView.h"
#include"CImGui_Manager.h"
#include "CGiantDevil.h"
#include "CGameInstance.h"
#include "CHammer.h"
#include "CTwoHandSword.h"
#include "CData_Manager.h"
#include "CPlayer.h"

CGiantDevil::CGiantDevil(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CMonster(_pDevice, _pContext)
{
}

CGiantDevil::CGiantDevil(const CGiantDevil& _rhs)
    : CMonster(_rhs)
{
}

HRESULT CGiantDevil::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;
    return S_OK;
}

HRESULT CGiantDevil::NativeConstruct_Clone(void* pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(pArg)))
        return E_FAIL;


    if (nullptr != pArg)
    {
        GIANTDEVILDESC tGiantDevilDesc = *static_cast<GIANTDEVILDESC*>(pArg);
        m_tMonsterDesc = tGiantDevilDesc.tMonsterDesc;
        m_eWeaponType = tGiantDevilDesc.eEquipWeaponType;
        m_eInitIdleType = tGiantDevilDesc.eInitAnimType;

        m_tMonsterDesc.tSightDesc.fFindAngle = 60.f;
        m_tMonsterDesc.tSightDesc.fFindDistance = 10.f;
        m_tMonsterDesc.tSightDesc.fYFindAngle = 1.f;
    }

    if (FAILED(Ready_Component()))
        return E_FAIL;

    if (FAILED(Ready_Others()))
        return E_FAIL;

    switch (m_eInitIdleType)
    {
    case CGiantDevil::INIT_IDLE:
        m_pModelCom->Set_Animation("AS_TGiantDevil_TCmn_NoneFightIdle_N_Loop");
        break;
    case CGiantDevil::INIT_CROUCH:
        m_pModelCom->Set_Animation("AS_TGiantDevil_TCmn_NoneFightIdleCrouch_N_Loop");
        break;
    case CGiantDevil::INIT_SIT:
        m_pModelCom->Set_Animation("AS_TGiantDevil_TCmn_NoneFightIdleSit_N_Loop");
        break;
    }

    m_fCurSpeed = 30.f;

    return S_OK;
}

void CGiantDevil::Tick(const _double& _dTimeDelta)
{
    if (nullptr != m_pModelCom)
        m_pModelCom->Update_Animation(_dTimeDelta);

    if (false == g_bGameRun)
    {
        Coll_Tick(m_pTransformCom->Get_WorldMatrix());
        return;
    }

    __super::Tick(_dTimeDelta);
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        Find_Player(pGameInstance);
        Coll_Tick(m_pTransformCom->Get_WorldMatrix());
        m_pGiantDevilStateMachine->Tick(_dTimeDelta, this);
        Calculate_OffsetPosition();
    }
    END_SINGLETON(pGameInstance);
}

void CGiantDevil::Late_Tick(const _double& _dTimeDelta)
{

    __super::Late_Tick(_dTimeDelta);

    BEGIN_SINGLETON(CGameInstance, pGameInstance)
    {
        if (true == pGameInstance->Is_Rendering(m_pTransformCom), 2.f)
            m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

        m_pGiantDevilStateMachine->Late_Tick(_dTimeDelta, this);
    }
    END_SINGLETON(pGameInstance);
}

HRESULT CGiantDevil::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    if (FAILED(SetUp_ConstantTable()))
        return E_FAIL;

    if (nullptr != m_pModelCom)
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

            m_pModelCom->Render(i, m_pShaderCom, 0);
        }
    }

    return S_OK;
}

HRESULT CGiantDevil::Ready_Component()
{
    if (FAILED(__super::Ready_Component()))
        return E_FAIL;

    if (FAILED(Ready_Transform(COM_TRANSFORM)))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(COM_ANIMMODEL, g_iCurrentLevel, m_tMonsterDesc.strModelName.c_str()
        , (CComponent**)&m_pModelCom)))
        return E_FAIL;

    if (FAILED(Reset_TransformData()))
        return E_FAIL;

    {
        // CollCollider
        CCollider::COLLIDERDESC tCollDesc;
        tCollDesc.vScale = _float3(0.3f, 0.8f, 0.1f);
        tCollDesc.vTranslation = _float3(0.f, 0.8f, 0.f);
        /* For.Com_Shader */
        if (FAILED(__super::Add_Component(COM_COLLCOLLIDER, LEVEL_STATIC,
            PROTO_COMP_OBBCOLLIDER, (CComponent**)&m_pCollCollider, &tCollDesc)))
            return E_FAIL;
    }

    {
        // First Check
        CCollider::COLLIDERDESC tCollDesc;
        tCollDesc.vScale = _float3(0.3f, 0.8f, 0.1f);
        tCollDesc.vTranslation = _float3(0.f, 0.8f, 0.f);
        if (FAILED(__super::Add_Component(COM_FIRSTCHECKCOLLIDER, LEVEL_STATIC, PROTO_COMP_AABBCOLLIDER,
            (CComponent**)&m_pFirstCheckCollider, &tCollDesc)))
            return E_FAIL;
    }

    {
        // Find Range Collider
        CCollider::COLLIDERDESC tCollDesc;
        tCollDesc.vScale = _float3(2.5f, 0.8f, 2.5f);
        tCollDesc.vTranslation = _float3(0.f, 0.8f, 0.f);
        if (FAILED(__super::Add_Component(COM_FINDCOLLIDER, LEVEL_STATIC, PROTO_COMP_SPHERECOLLIDER,
            (CComponent**)&m_pRangeCollider, &tCollDesc)))
            return E_FAIL;
    }

    {
        // Find Range Collider
        CCollider::COLLIDERDESC tCollDesc;
        tCollDesc.vScale = _float3(10.f, 0.8f, 10.f);
        tCollDesc.vTranslation = _float3(0.f, 0.8f, 0.f);
        if (FAILED(__super::Add_Component(COM_ATTACKDISCOLLIDER, LEVEL_STATIC, PROTO_COMP_SPHERECOLLIDER,
            (CComponent**)&m_pAttackNowRangeCollider, &tCollDesc)))
            return E_FAIL;
    }

    if (FAILED(Find_OffsetNode()))
        return E_FAIL;

    return S_OK;
}

HRESULT CGiantDevil::SetUp_ConstantTable()
{
    if (FAILED(__super::SetUp_ConstantTable()))
        return E_FAIL;

    return S_OK;
}

void CGiantDevil::Calculate_OffsetPosition()
{
    if (CGiantDevilStateMachine::STATE_ATTACK == m_pGiantDevilStateMachine->Get_State())
    {
        _matrix matWorld = m_pNodeArr[NODE_OFFSET]->Get_OffsetMatrix() *
            m_pNodeArr[NODE_OFFSET]->Get_CombinedTransformationMatrix() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
        XMStoreFloat3(&m_v3OffsetPosition, matWorld.r[3]);
        matWorld = m_pTransformCom->Get_WorldMatrix();
        matWorld.r[3] = XMLoadFloat3((_float3*)&m_v3OffsetPosition);
        Coll_Tick(matWorld);
    }
    else
    {
        Coll_Tick(m_pTransformCom->Get_WorldMatrix());
        XMStoreFloat3(&m_v3OffsetPosition, m_pTransformCom->Get_Pos());
        if (true == m_bFindPlayer)
        {
            if(nullptr != m_pTargetPlayer)
                m_pTransformCom->LookAt(m_pTargetPlayer->Get_OffsetPos());
        }
    }
}

void CGiantDevil::Bind_Attack_Postion()
{
    if (false == m_bChangeAttackMatrix)
        return;

    m_pTransformCom->Set_Pos(XMLoadFloat3((_float3*)&m_OffsetMatrix.m[3][0]));
    m_bChangeAttackMatrix = false;
    m_OffsetMatrix = m_pTransformCom->Get_WorldFloat4x4();
}

void CGiantDevil::Bind_Supplement_Position()
{
    if (false == m_bChangeSuppleCount)
        return;

    if (false == m_pModelCom->Get_CurAnimation()->Is_SupplementSituation())
    {
        m_pTransformCom->Set_Pos(XMLoadFloat3((_float3*)&m_OffsetMatrix.m[3][0]));
        m_bChangeSuppleCount = false;
        m_OffsetMatrix = m_pTransformCom->Get_WorldFloat4x4();
    }
}


//void CGiantDevil::IDLE_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
//{
//    if (CGiantDevilStateMachine::STATE_ATTACK == m_pGiantDevilStateMachine->Get_State() || 
//        CGiantDevilStateMachine::STATE_HIT == m_pGiantDevilStateMachine->Get_State() || 
//        CGiantDevilStateMachine::STATE_DIE == m_pGiantDevilStateMachine->Get_State() || 
//        CGiantDevilStateMachine::STATE_FIND == m_pGiantDevilStateMachine->Get_State() ||
//        CGiantDevilStateMachine::STATE_THREAT == m_pGiantDevilStateMachine->Get_State() ||
//        CGiantDevilStateMachine::STATE_GETUP == m_pGiantDevilStateMachine->Get_State())
//        return;
//
//    if (nullptr == m_pTargetPlayer)
//        return;
//
//    CCollider* pPlayerCheckCollider = static_cast<CCollider*>(m_pTargetPlayer->Get_Component(COM_FIRSTCHECKCOLLIDER));
//    CTransform* pPlayerTransform = static_cast<CTransform*>(m_pTargetPlayer->Get_Component(COM_TRANSFORM));
//
//    if (nullptr == pPlayerCheckCollider || nullptr == pPlayerTransform)
//        return;
//
//    if (false == m_bFindPlayer)
//    {
//        if (false == Is_FindTarget(_pGameInstance))
//        {
//            if (true == m_pRangeCollider->Is_Collision(pPlayerCheckCollider))
//            {
//                m_pGiantDevilStateMachine->Change_State(CGiantDevilStateMachine::STATE_GETUP, this);
//                m_bFindPlayer = true;
//            }
//            else
//                m_pGiantDevilStateMachine->Change_State(CGiantDevilStateMachine::STATE_IDLE, this);
//        }
//        else
//        {
//            m_pGiantDevilStateMachine->Change_State(CGiantDevilStateMachine::STATE_GETUP, this);
//            m_bFindPlayer = true;
//        }
//    }
//    else
//    {
//        m_pGiantDevilStateMachine->Change_State(CGiantDevilStateMachine::STATE_FIND, this);
//    }
//}
//
//void CGiantDevil::GETUP_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
//{
//    if (CGiantDevilStateMachine::STATE_GETUP != m_pGiantDevilStateMachine->Get_State())
//        return;
//
//    if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(1.2f))
//        m_pGiantDevilStateMachine->Change_State(CGiantDevilStateMachine::STATE_THREAT, this);
//}
//
//void CGiantDevil::THREAT_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
//{
//    if (CGiantDevilStateMachine::STATE_THREAT != m_pGiantDevilStateMachine->Get_State())
//        return;
//
//    if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(1.6f))
//        m_pGiantDevilStateMachine->Change_State(CGiantDevilStateMachine::STATE_FIND, this);
//}
//
//void CGiantDevil::FIND_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
//{
//    if (CGiantDevilStateMachine::STATE_FIND == m_pGiantDevilStateMachine->Get_State())
//    {
//        if (nullptr == m_pTargetPlayer)
//            return;
//
//        CCollider* pPlayerCheckCollider = static_cast<CCollider*>(m_pTargetPlayer->Get_Component(COM_FIRSTCHECKCOLLIDER));
//        if (nullptr == pPlayerCheckCollider)
//            return;
//
//        if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
//        {
//            if (true == m_pAttackNowRangeCollider->Is_Collision(pPlayerCheckCollider))
//            {
//                _float fDistance = PlayerToDistance();
//                if (EW_HAMMER == m_eWeaponType)
//                {
//                    if (fDistance >= 5.f)
//                    {
//                        m_iAttackPattern = _pGameInstance->Get_RandomCount(1000) % 2 + 2;
//                    }
//                    else
//                    {
//                        m_iAttackPattern = _pGameInstance->Get_RandomCount(1000) % 2;
//                    }
//                }
//                else
//                {
//                    if (fDistance <= 5.f)
//                    {
//                        m_iAttackPattern = _pGameInstance->Get_RandomCount(1000) % 4;
//                    }
//                    else
//                    {
//                        m_iAttackPattern = _pGameInstance->Get_RandomCount(1000) % 2 + 4;
//                    }
//                }
//
//                m_pGiantDevilStateMachine->Change_State(CGiantDevilStateMachine::STATE_ATTACK, this);
//            }
//            else
//            {
//                m_pGiantDevilStateMachine->Change_State(CGiantDevilStateMachine::STATE_IDLE, this);
//                m_bFindPlayer = false;
//            }
//        }
//    }
//}
//
//void CGiantDevil::ATTACK_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
//{
//    CTransform* pPlayerTransform = static_cast<CTransform*>(m_pTargetPlayer->Get_Component(COM_TRANSFORM));
//    if (nullptr == pPlayerTransform)
//        return;
//
//    if (CGiantDevilStateMachine::STATE_ATTACK == m_pGiantDevilStateMachine->Get_State())
//    {
//        if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(1.2f))
//        {
//            m_pGiantDevilStateMachine->Change_State(CGiantDevilStateMachine::STATE_ATTACK, this);
//            m_pGiantDevilStateMachine->Set_AttackCount(0);
//        }
//        else if (true == m_pModelCom->Get_CurAnimation()-> Get_AnimTimeSituation(2.f))
//        {
//            Calculate_OffsetMatrix();
//            m_bChangeAttackMatrix = true;
//            Bind_Attack_Postion();
//        }
//
//        if (EW_SWORD == m_eWeaponType)
//        {
//            if (true == m_pGiantDevilStateMachine->Is_NextAttack())
//            {
//                
//                if (m_pGiantDevilStateMachine->Get_AtkCount() < 3)
//                {
//                    if (false == m_pModelCom->Get_CurAnimation()->Is_SupplementSituation())
//                    {
//                        if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(2.5f))
//                        {
//                            m_pGiantDevilStateMachine->Change_State(CPlayerStateMachine::STATE_ATTACK, this);
//                            Calculate_OffsetMatrix();
//                            m_bChangeSuppleAttackMatrix = true;
//                            Setting_CurrentLookPos();
//                        }
//                    }
//                }
//            }
//        }
//        else if (EW_HAMMER == m_eWeaponType)
//        {
//            if (PAT_N1 == m_iAttackPattern)
//            {
//                if (m_pGiantDevilStateMachine->Get_AtkCount() < 3)
//                {
//                    if (false == m_pModelCom->Get_CurAnimation()->Is_SupplementSituation())
//                    {
//                        if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(2.5f))
//                        {
//                            m_pGiantDevilStateMachine->Change_State(CGiantDevilStateMachine::STATE_ATTACK, this);
//                            Calculate_OffsetMatrix();
//                            m_bChangeSuppleAttackMatrix = true;
//                            Setting_CurrentLookPos();
//                        }
//                    }
//                }
//            }
//            else if (PAT_N2 == m_iAttackPattern)
//            {
//                if (m_pGiantDevilStateMachine->Get_AtkCount() < 2)
//                {
//                    if (false == m_pModelCom->Get_CurAnimation()->Is_SupplementSituation())
//                    {
//                        if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(2.5f))
//                        {
//                            m_pGiantDevilStateMachine->Change_State(CGiantDevilStateMachine::STATE_ATTACK, this);
//                            m_pTransformCom->LookAt(pPlayerTransform->Get_Pos());
//                            Calculate_OffsetMatrix();
//                            m_bChangeSuppleAttackMatrix = true;
//                            Setting_CurrentLookPos();
//                        }
//                    }
//                }
//            }
//        }
//    }
//}

HRESULT CGiantDevil::Ready_Others()
{
    if (EW_END == m_eWeaponType)
        return E_FAIL;

    if (EW_HAMMER == m_eWeaponType)
    {
        BEGIN_SINGLETON(CGameInstance, pGameInstance)
        {
            CHammer::HAMMERDESC tDesc;
            tDesc.tItemDesc = CItem::ITEMDESC(PROTO_COMP_MODEL_GIANTEVIL_LARGEGREATHAMMER);
            tDesc.tEquipDesc.pOwnerObject = this;
            tDesc.tEquipDesc.strAttachNodeName = "RightHandAttach_end";
            CEquipment* pWeapon = (CWeapon*)pGameInstance->Clone_GameObject_Add(g_iCurrentLevel, LAYER_WEAPON,
                PROTO_GOB_HAMMER, &tDesc);
            m_pEquipmentArr[CEquipment::EQUIP_WEAPON] = pWeapon;
            Safe_AddRef(pWeapon);
        }
        END_SINGLETON(pGameInstance);
    }
    else
    {
        BEGIN_SINGLETON(CGameInstance, pGameInstance)
        {
            CTwoHandSword::TWOHANDSWORDDESC tDesc;
            tDesc.tItemDesc = CItem::ITEMDESC(PROTO_COMP_MODEL_GIANTEVIL_LARGEGREATSWORD);
            tDesc.tEquipDesc.pOwnerObject = this;
            tDesc.tEquipDesc.strAttachNodeName = "RightHandAttach_end";
            CEquipment* pWeapon = (CWeapon*)pGameInstance->Clone_GameObject_Add(g_iCurrentLevel, LAYER_WEAPON,
                PROTO_GOB_TWOHANDWORD, &tDesc);
            m_pEquipmentArr[CEquipment::EQUIP_WEAPON] = pWeapon;
            Safe_AddRef(pWeapon);
        }
        END_SINGLETON(pGameInstance);
    }

    m_pGiantDevilStateMachine = CGiantDevilStateMachine::Create(m_pDevice, m_pContext, this);
    if (nullptr == m_pGiantDevilStateMachine)
        return E_FAIL;

    return S_OK;
}
#pragma region USE_TOOL_METHOD
_bool CGiantDevil::Extern_EditValue()
{
    _bool bActive = false;

    if (ImGui::TreeNodeEx("GiantDevil"))
    {
        ImGui::PushItemWidth(200);
        Extern_GobData();
        Reset_Data();
        Extern_JudgeMonsterAndWeaponType();
        Extern_InitAnation();
        Extern_ModifySightDesc();
        bActive = ImGui::Button("Create");
        ImGui::PopItemWidth();
        ImGui::TreePop();
    }

    return bActive;
}

_bool CGiantDevil::Extern_HierakeyView(const string& _str)
{
    if (nullptr == m_pTransformCom)
        return false;

    _bool bActive = false;
    if (ImGui::TreeNodeEx(_str.c_str()))
    {
        ImGui::PushItemWidth(200);
        Extern_GobData();
        Control_ColliderData();
        Extern_ShowCurAnimation_Data();
        bActive = ImGui::Button("Delete");
        ImGui::PopItemWidth();
        ImGui::TreePop();
        return bActive;
    }
    return false;
}

HRESULT CGiantDevil::Create_Self(CGameObject** _ppOut)
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        GIANTDEVILDESC tGiantDevilDesc;
        tGiantDevilDesc.tGobDesc = m_tGobDesc;
        tGiantDevilDesc.tMonsterDesc = m_tMonsterDesc;
        tGiantDevilDesc.eEquipWeaponType = m_eWeaponType;
        tGiantDevilDesc.tMonsterDesc = m_tMonsterDesc;
        tGiantDevilDesc.eInitAnimType = m_eInitIdleType;

        *_ppOut = static_cast<CGameObject_Client*>(pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, Get_Layer()
            , Get_Proto(), &tGiantDevilDesc));
    }
    END_SINGLETON(pGameInstance);
    return S_OK;
}

HRESULT CGiantDevil::Delete_Self()
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        pGameInstance->Add_DeleteObject(LEVEL_TOOL, Get_Layer(), this);
    }
    END_SINGLETON(pGameInstance);
    return S_OK;
}

HRESULT CGiantDevil::Save_Data()
{
    BEGIN_SINGLETON(CData_Manager, pDataManager);
    {
        GIANTDEVILDESC tGiantDevilDesc;
        tGiantDevilDesc.tGobDesc = m_tGobDesc;
        tGiantDevilDesc.tMonsterDesc = m_tMonsterDesc;
        tGiantDevilDesc.eEquipWeaponType = m_eWeaponType;
        tGiantDevilDesc.tMonsterDesc = m_tMonsterDesc;
        tGiantDevilDesc.eInitAnimType = m_eInitIdleType;

        // Save 
        pDataManager->Get_MonsterMgr()->Insert_GiantDevilData(tGiantDevilDesc);
    }
    END_SINGLETON(pDataManager);
    return S_OK;
}

HRESULT CGiantDevil::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        *_ppOut = static_cast<CGameObject_Client*>(pGameInstance->Clone_GameObject_Add(_eLevel, Get_Layer()
            , Get_Proto(), _pData));
    }
    END_SINGLETON(pGameInstance);
    return S_OK;
}

_bool CGiantDevil::Is_Picking(_float3* _pOut)
{
    return __super::Is_Picking(_pOut);
}

void CGiantDevil::Reset_Data()
{
    __super::Reset_Data();
}

void CGiantDevil::Extern_JudgeMonsterAndWeaponType()
{
    const char* ModelType[3] = { "Normal", "Special1", "Specal2" };
    const char* WeaponType[2] = { "Hammer", "Sword" };

    ImGui::Combo("ModelType", &m_iModelID, ModelType, 3);
    ImGui::Combo("WeaponType", (int*)&m_eWeaponType, WeaponType, 2);

    switch (m_iModelID)
    {
    case 0:
        m_tMonsterDesc.strModelName = PROTO_COMP_MODEL_GIANTDEVIL_NORMAL;
        break;
    case 1:
        m_tMonsterDesc.strModelName = PROTO_COMP_MODEL_GIANTDEVIL_SP1;
        break;
    case 2:
        m_tMonsterDesc.strModelName = PROTO_COMP_MODEL_GIANTDEVIL_SP2;
        break;
    }
}

void CGiantDevil::Extern_InitAnation()
{
    const char* ModelType[3] = {"NoneFightIdle_N_Loop", "NoneFightIdleCrouch_N", "NoneFightIdleSit_N"};
    ImGui::Combo("ModelType", (_int*) & m_eInitIdleType, ModelType, 3);
}

#pragma endregion USE_TOOL_METHOD
CGiantDevil* CGiantDevil::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
    CGiantDevil* pInstance = new CGiantDevil(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CGiantDevil::Create to Failed");
    }
    return pInstance;
}

CGameObject* CGiantDevil::Clone(void* pArg)
{
    CGiantDevil* pInstance = new CGiantDevil(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CGiantDevil::Clone to Failed");
    }
    return pInstance;
}

void CGiantDevil::Free()
{
    Safe_Release(m_pGiantDevilStateMachine);
    Safe_Release(m_pTargetPlayer);
    __super::Free();
}
