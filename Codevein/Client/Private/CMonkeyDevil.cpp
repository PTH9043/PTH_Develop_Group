#include "CMainView.h"
#include"CImGui_Manager.h"
#include "CMonkeyDevil.h"
#include "CGameInstance.h"
#include "CData_Manager.h"
#include "CPlayer.h"
#include "CHammer.h"
#include "COneHandSword.h"
#include "CMonsterEPController.h"

CMonkeyDevil::CMonkeyDevil(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CMonster(_pDevice, _pContext)
{
}

CMonkeyDevil::CMonkeyDevil(const CMonkeyDevil& _rhs)
	: CMonster(_rhs)
{
}

HRESULT CMonkeyDevil::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CMonkeyDevil::NativeConstruct_Clone(void* pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(pArg)))
        return E_FAIL;


    if (nullptr != pArg)
    {
        MONKEYDEVILDESC tMonkeyDevilDesc = *static_cast<MONKEYDEVILDESC*>(pArg);
        m_tGobDesc = tMonkeyDevilDesc.tGobDesc;
        m_tMonsterDesc = tMonkeyDevilDesc.tMonsterDesc;
        m_eInitIdleType = tMonkeyDevilDesc.eInitAnimType;
        m_eWeaponType = tMonkeyDevilDesc.eWeaponType;
        m_tPawnStatus = tMonkeyDevilDesc.tPawnStatus;

        m_tMonsterDesc.tSightDesc.fFindAngle = 60.f;
        m_tMonsterDesc.tSightDesc.fFindDistance = 10.f;
        m_tMonsterDesc.tSightDesc.fYFindAngle = 1.f;
        m_tCharInfo.UpdateInfo(m_tPawnStatus);
    }

    if (FAILED(Ready_Component()))
        return E_FAIL;

    if (FAILED(Ready_Others()))
        return E_FAIL;

    switch (m_eInitIdleType)
    {
    case CMonkeyDevil::INIT_IDLE:
        m_pModelCom->Set_Animation("AS_TMonkeyDevil_TCmn_NoneFightIdle_N_Loop");
        break;
    case CMonkeyDevil::INIT_FALL:
        m_pModelCom->Set_Animation("AS_TMonkeyDevil_TCmn_NoneFightIdleSit02_N_Loop");
        break;
    case CMonkeyDevil::INIT_WALK:
        m_pModelCom->Set_Animation("AS_TMonkeyDevil_TCmn_NoneFightWalk_F_Loop");
        break;
    case CMonkeyDevil::INIT_WALL:
        m_pModelCom->Set_Animation("AS_TMonkeyDevil_TCmn_WallFallLanding_N_Loop");
        break;
    case CMonkeyDevil::INIT_SIT:
        m_pModelCom->Set_Animation("AS_TMonkeyDevil_TCmn_PlafondFallLanding_N_Start");
        break;
    }

    m_fCurSpeed = 8.f;
    m_fRotationStandardValue = XMConvertToRadians(90.f);
    if (nullptr != m_pCollCollider)
        m_fDrawRadius = sqrtf(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pCollCollider->Get_Scale()) * 2.f, XMLoadFloat3(&m_pCollCollider->Get_Scale()) * 2.f)));
  
    m_pNavigation->Find_Cell(m_pTransformCom->Get_Pos());
    return S_OK;
}

void CMonkeyDevil::Tick(const _double& _dTimeDelta)
{
    if (nullptr != m_pModelCom)
        m_pModelCom->Update_Animation(_dTimeDelta);

    if (false == g_bGameRun)
    {
        Coll_Tick(m_pTransformCom->Get_WorldMatrix());
        return;
    }
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        IDLE_STATE(_dTimeDelta, pGameInstance);
        FIND_STATE(_dTimeDelta, pGameInstance);
        RETARGET_STATE(_dTimeDelta, pGameInstance);
        DODGE_STATE(_dTimeDelta, pGameInstance);
        RUN_STATE(_dTimeDelta, pGameInstance);
        ATTACK_STATE(_dTimeDelta, pGameInstance);
        DAMAGED_STATE(_dTimeDelta, pGameInstance);
        DIE_STATE(_dTimeDelta, pGameInstance);
        m_pMonkeyDevilStateMachine->Tick(_dTimeDelta, this);
        Calculate_OffsetPosition();
        Judge_AttackStrongCount();
    }
    END_SINGLETON(pGameInstance);
}

void CMonkeyDevil::Late_Tick(const _double& _dTimeDelta)
{
    if (true == g_bGameRun)
    {
        Add_AroundPossibleAttackPlayer();
        m_pMonkeyDevilStateMachine->Late_Tick(_dTimeDelta, this);
        Restrict_AnimationToMoveOffset(m_pMonkeyDevilStateMachine);
    }
    __super::Late_Tick(_dTimeDelta);
}

HRESULT CMonkeyDevil::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonkeyDevil::Damaged(CPawn* _pDamageOwner)
{
    if (FAILED(__super::Damaged(_pDamageOwner)))
        return E_FAIL;

    if (CMonsterState::STATE_DIE == m_pMonkeyDevilStateMachine->Get_State())
        return E_FAIL;

    if (CPawn::P_ITEM == _pDamageOwner->Get_PawnType())
    {
        CWeapon* pWeapon = static_cast<CWeapon*>(_pDamageOwner);
        if (nullptr == pWeapon)
            return E_FAIL;;

          m_tCharInfo.Damaged(pWeapon->Get_CharInfo().Get_Damage());

        if (m_tCharInfo.Is_CheckDie())
        {
            if (nullptr != m_pMonsterEPController)
            {
                CWeapon* pWeapon = static_cast<CWeapon*>(_pDamageOwner);
                _float3 v3Pos;
                _vector vChestPos = Get_OffsetArrPosition(NODE_CHEST);
                XMStoreFloat3(&v3Pos, XMLoadFloat3((_float3*)&pWeapon->Get_SockMatrix4x4().m[3][0]));
                _vector vDir = vChestPos - XMLoadFloat3(&v3Pos);
                m_pMonsterEPController->Show_Blood(vChestPos, vDir);
            }

            m_pMonkeyDevilStateMachine->Change_State(CMonkeyDevilStateMachine::STATE_DIE, this);
        }
        else
        {
            if (true == m_tHurtTimer.bPass)
            {
                if (nullptr != m_pMonsterEPController)
                {
                    CWeapon* pWeapon = static_cast<CWeapon*>(_pDamageOwner);
                    _float3 v3Pos;
                    _vector vChestPos = Get_OffsetArrPosition(NODE_CHEST);
                    XMStoreFloat3(&v3Pos, XMLoadFloat3((_float3*)&pWeapon->Get_SockMatrix4x4().m[3][0]));
                    _vector vDir = vChestPos - XMLoadFloat3(&v3Pos);
                    m_pMonsterEPController->Show_Blood(vChestPos, vDir);
                }

                if (CMonkeyDevilStateMachine::STATE_HIT == m_pMonkeyDevilStateMachine->Get_State() ||
                    CMonkeyDevilStateMachine::STATE_ATTACK == m_pMonkeyDevilStateMachine->Get_State()
                     || false == m_pModelCom->Get_PrevAnimation()->Is_SupplementSituation())
                    return E_FAIL;

                m_bDamaged = true;
                m_tHurtTimer.Reset_Timer();
                m_pMonkeyDevilStateMachine->Change_State(CMonkeyDevilStateMachine::STATE_HIT, this);
            }
        }
    }
    else  if (CPawn::P_CHARACTER == _pDamageOwner->Get_PawnType())
    {
    }

    return S_OK;
}

HRESULT CMonkeyDevil::Ready_Component()
{
    if (FAILED(__super::Ready_Component()))
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
        tCollDesc.vScale = _float3(1.f, 0.7f, 1.f);
        tCollDesc.vTranslation = _float3(0.f, 0.7f, 0.f);
        /* For.Com_Shader */
        if (FAILED(__super::Add_Component(COM_COLLCOLLIDER, LEVEL_STATIC,
            PROTO_COMP_OBBCOLLIDER, (CComponent**)&m_pCollCollider, &tCollDesc)))
            return E_FAIL;
    }

    {
        // First Check
        CCollider::COLLIDERDESC tCollDesc;
        tCollDesc.vScale = _float3(1.f, 0.7f, 1.f);
        tCollDesc.vTranslation = _float3(0.f, 0.7f, 0.f);
        if (FAILED(__super::Add_Component(COM_FIRSTCHECKCOLLIDER, LEVEL_STATIC, PROTO_COMP_AABBCOLLIDER,
            (CComponent**)&m_pFirstCheckCollider, &tCollDesc)))
            return E_FAIL;
    }

    {
        // Find Range Collider
        CCollider::COLLIDERDESC tCollDesc;
        tCollDesc.vScale = _float3(5.f, 0.7f, 2.5f);
        tCollDesc.vTranslation = _float3(0.f, 0.7f, 0.f);
        if (FAILED(__super::Add_Component(COM_FINDCOLLIDER, LEVEL_STATIC, PROTO_COMP_SPHERECOLLIDER,
            (CComponent**)&m_pRangeCollider, &tCollDesc)))
            return E_FAIL;
    }

    {
        // Find Range Collider
        CCollider::COLLIDERDESC tCollDesc;
        tCollDesc.vScale = _float3(12.f, 0.7f, 10.f);
        tCollDesc.vTranslation = _float3(0.f, 0.7f, 0.f);
        if (FAILED(__super::Add_Component(COM_ATTACKDISCOLLIDER, LEVEL_STATIC, PROTO_COMP_SPHERECOLLIDER,
            (CComponent**)&m_pAttackNowRangeCollider, &tCollDesc)))
            return E_FAIL;
    }

    if (FAILED(Find_OffsetNode()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonkeyDevil::SetUp_ConstantTable()
{
    if (FAILED(__super::SetUp_ConstantTable()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonkeyDevil::Ready_Others()
{
    if (EW_HAMMER == m_eWeaponType)
    {
        BEGIN_SINGLETON(CGameInstance, pGameInstance)
        {
            CHammer::HAMMERDESC tDesc;
            tDesc.tItemDesc = CItem::ITEMDESC(PROTO_COMP_MODEL_MONKEYDEVIL_MARDUKHAMMER);
            tDesc.tEquipDesc.pOwnerObject = this;
            tDesc.tEquipDesc.strAttachNodeName = "RightHandAttach_end";
            CEquipment* pWeapon = (CWeapon*)pGameInstance->Clone_GameObject_Add(g_iCurrentLevel, LAYER_WEAPON,
                PROTO_GOB_HAMMER, &tDesc);
            m_pEquipmentArr[CEquipment::EQUIP_WEAPON] = pWeapon->ReturnSettingToValue(this);
            Safe_AddRef(pWeapon);
        }
        END_SINGLETON(pGameInstance);
    }
    else
    {
        _float3 v3PivotPos = _float3(0.f, 1.f, 0.f);
        _float3 v3PivotScale = _float3(0.1f, 0.1f, 0.f);
        BEGIN_SINGLETON(CGameInstance, pGameInstance)
        {
            COneHandSword::ONEHANDSWRODDESC tDesc;
            tDesc.tItemDesc = CItem::ITEMDESC(PROTO_COMP_MODEL_MONKEYDEVIL_BURNINGSWORD);
            tDesc.tEquipDesc.pOwnerObject = this;
            tDesc.tEquipDesc.strAttachNodeName = "RightHandAttach_end";
            tDesc.tWeaponDesc.v3PivotColliderPos = v3PivotPos;
            tDesc.tWeaponDesc.v3PivotColliderSize = v3PivotScale;
            CEquipment* pWeapon = (CWeapon*)pGameInstance->Clone_GameObject_Add(g_iCurrentLevel, LAYER_WEAPON,
                PROTO_GOB_ONEHANDSWORD, &tDesc);
            m_pEquipmentArr[CEquipment::EQUIP_WEAPON] = pWeapon->ReturnSettingToValue(this);
            Safe_AddRef(pWeapon);
        }
        END_SINGLETON(pGameInstance);
    }

    m_pMonkeyDevilStateMachine = CMonkeyDevilStateMachine::Create(m_pDevice, m_pContext, this);
    if (nullptr == m_pMonkeyDevilStateMachine)
        return E_FAIL;

    return S_OK;
}

void CMonkeyDevil::Calculate_OffsetPosition()
{
    if (CMonkeyDevilStateMachine::STATE_ATTACK == m_pMonkeyDevilStateMachine->Get_State()
    || CMonkeyDevilStateMachine::STATE_DODGE == m_pMonkeyDevilStateMachine->Get_State())
    {
        _matrix matWorld = 
            m_pNodeArr[NODE_OFFSET]->Get_CombinedTransformationMatrix() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
        XMStoreFloat3(&m_v3OffsetPosition, matWorld.r[3]);
        matWorld = m_pTransformCom->Get_WorldMatrix();

        if (nullptr != m_pAttackPosCell)
        {
            _vector vPos = m_pTransformCom->Get_Pos();
            vPos = XMVectorSetY(vPos, m_pAttackPosCell->Compute_Height(XMLoadFloat3(&m_v3OffsetPosition)));
            XMStoreFloat3(&m_v3OffsetPosition, vPos);
            vPos = XMVectorSetY(m_pTransformCom->Get_Pos(), m_v3OffsetPosition.y);
            m_pTransformCom->Set_Pos(vPos);
        }
        else
        {
            _float3 v3Pos;
            XMStoreFloat3(&v3Pos, m_pTransformCom->Get_Pos());
            m_v3OffsetPosition.y = v3Pos.y;
        }

        matWorld.r[3] = XMLoadFloat3((_float3*)&m_v3OffsetPosition);
        XMStoreFloat4x4(&m_DrawAnimMatrix, matWorld);
        Coll_Tick(matWorld);
    }
    else
    {
        Coll_Tick(m_pTransformCom->Get_WorldMatrix());
        XMStoreFloat4x4(&m_DrawAnimMatrix, m_pTransformCom->Get_WorldMatrix());
        XMStoreFloat3(&m_v3OffsetPosition, m_pTransformCom->Get_Pos());
        m_bRestrictAttackPos = false;
    }
}

void CMonkeyDevil::Bind_Attack_Postion()
{
    __super::Bind_Attack_Postion();
}

void CMonkeyDevil::Bind_Dodge_Position()
{
    if (false == m_bChangeDodgeMatrix)
        return;

    m_pTransformCom->Set_Pos(XMLoadFloat3((_float3*)&m_OffsetMatrix.m[3][0]));
    m_bChangeDodgeMatrix = false;
    m_OffsetMatrix = m_pTransformCom->Get_WorldFloat4x4();
}

void CMonkeyDevil::Bind_Damaged_Position()
{
    if (false == m_bChangeDamagedPosition)
        return;

    m_pTransformCom->Set_Pos(XMLoadFloat3((_float3*)&m_OffsetMatrix.m[3][0]));
    m_bChangeDamagedPosition = false;
    m_OffsetMatrix = m_pTransformCom->Get_WorldFloat4x4();
}

void CMonkeyDevil::Bind_Die_Position()
{
    if (false == m_bChangeDiePosition)
        return;

    if (true == m_pModelCom->Is_ChangeAnimation())
    {
        m_pTransformCom->Set_Pos(XMLoadFloat3((_float3*)&m_OffsetMatrix.m[3][0]));
        m_bChangeDiePosition = false;
        m_OffsetMatrix = m_pTransformCom->Get_WorldFloat4x4();
    }
}

void CMonkeyDevil::Bind_Supplement_Position()
{
    __super::Bind_Supplement_Position();
}

void CMonkeyDevil::Bind_End_Render()
{
    Bind_Attack_Postion();
    Bind_Dodge_Position();
    Bind_Die_Position();
}

void CMonkeyDevil::Add_AroundPossibleAttackPlayer()
{
    if (CMonkeyDevilStateMachine::STATE_ATTACK == m_pMonkeyDevilStateMachine->Get_State())
    {
        CWeapon* pWeapon = static_cast<CWeapon*>(m_pEquipmentArr[CEquipment::EQUIP_WEAPON]);
        if (nullptr != pWeapon)
        {
            pWeapon->Add_CheckColliderList(m_pTargetPlayer);
        }
        if (nullptr != m_pCollCollider)
        {
            CCollider* pCollider = static_cast<CCollider*>(m_pTargetPlayer->Get_Component(COM_FIRSTCHECKCOLLIDER));
            if (nullptr == pCollider)
                return;

            if (true == m_pRangeCollider->Is_Collision(pCollider))
                return;

            pCollider = static_cast<CCollider*>(m_pTargetPlayer->Get_Component(COM_COLLCOLLIDER));
            if (nullptr == pCollider)
                return;
            if (true == m_pCollCollider->Is_Collision(pCollider))
            {
                m_pTargetPlayer->Damaged(this);
            }
        }
    }
}

void CMonkeyDevil::Judge_AttackStrongCount()
{
    if (ATK_JUMP == m_iAttackIndex)
        m_eAtkStrong = ATK_STR::AS_NORMAL;
    else if(ATK_NORMAL == m_iAttackIndex)
        m_eAtkStrong = ATK_STR::AS_WEEK;
    else if(ATK_SP == m_iAttackIndex)
        m_eAtkStrong = ATK_STR::AS_NORMAL;
}

void CMonkeyDevil::IDLE_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
    if (CMonkeyDevilStateMachine::STATE_IDLE != m_pMonkeyDevilStateMachine->Get_State())
        return;

    if (nullptr == m_pTargetPlayer)
        return;

    CCollider* pPlayerCheckCollider = static_cast<CCollider*>(m_pTargetPlayer->Get_Component(COM_FIRSTCHECKCOLLIDER));
    CTransform* pPlayerTransform = static_cast<CTransform*>(m_pTargetPlayer->Get_Component(COM_TRANSFORM));

    if (nullptr == pPlayerCheckCollider || nullptr == pPlayerTransform)
        return;

    if (false == m_bFindPlayer)
    {
        if (true == m_pAttackNowRangeCollider->Is_Collision(pPlayerCheckCollider))
        {
            switch(m_eInitIdleType)
            {
            case INIT_IDLE:

                break;
            }
            m_pMonkeyDevilStateMachine->Change_State(CMonkeyDevilStateMachine::STATE_FIND, this);
            m_bFindPlayer = true;
        }
        else
            m_pMonkeyDevilStateMachine->Change_State(CMonkeyDevilStateMachine::STATE_IDLE, this);
    }
    else
    {
        m_pMonkeyDevilStateMachine->Change_State(CMonkeyDevilStateMachine::STATE_FIND, this);
    }
}

void CMonkeyDevil::FIND_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
    if (CMonkeyDevilStateMachine::STATE_FIND != m_pMonkeyDevilStateMachine->Get_State())
        return;
    if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(1.2f))
    {
        _int ranValue = _pGameInstance->Get_RandomCount() % 2;
        if (0 == ranValue)
        {
            m_iAttackPattern = PAT_1;
            m_pMonkeyDevilStateMachine->Change_State(CMonkeyDevilStateMachine::STATE_RETARGET, this);
        }
        else if (1 == ranValue)
        {
            m_iAttackPattern = PAT_2;
            m_pMonkeyDevilStateMachine->Change_State(CMonkeyDevilStateMachine::STATE_RETARGET, this);
        }

        m_bChangeAttackMatrix = true;
        Bind_Attack_Postion();

        _float fYPos = m_pNavigation->Compute_Height(m_pTransformCom->Get_Pos());
        // Áö¿ì±â
        _float3 v3Pos;
        XMStoreFloat3(&v3Pos, m_pTransformCom->Get_Pos());
        m_pTransformCom->Set_Pos(XMLoadFloat3(&_float3(v3Pos.x, fYPos, v3Pos.z)));
    }
    else
    {
        Calculate_OffsetMatrix();
    }
}

void CMonkeyDevil::RETARGET_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
    if (CMonkeyDevilStateMachine::STATE_RETARGET != m_pMonkeyDevilStateMachine->Get_State())
        return;

    if (!strcmp("AS_TMonkeyDevil_TCmn_Walk_B_Loop", m_pModelCom->Get_CurAnimation()->Get_Name().c_str()))
    {
        m_pModelCom->Get_CurAnimation()->Set_FastValue(15.f);
        _float fDistance = PlayerToDistance();

        if (false == RotateLookToPlayer(_dTimeDelta))
            m_pTransformCom->Move_Back(_dTimeDelta, m_fCurSpeed / 4.f);
        else
        {
            if(fDistance <= 4.f)
            {
                _uint iRan = _pGameInstance->Get_RandomCount() % 2;
                if (0 == iRan)
                    m_iAttackIndex = ATK_NORMAL;
                else
                    m_iAttackIndex = ATK_SP;
                m_pMonkeyDevilStateMachine->Change_State(CMonkeyDevilStateMachine::STATE_ATTACK, this);
            }

            if (PAT_3 != m_iAttackPattern)
            {
                _float fBalnaceValue = 8.f;
                if (PAT_1 == m_iAttackPattern)
                    fBalnaceValue = 8.f;
                else if (PAT_2 == m_iAttackPattern)
                    fBalnaceValue = 5.f;


                if (fDistance >= fBalnaceValue)
                {
                    if (PAT_1 == m_iAttackPattern)
                    {
                        m_iAttackIndex = ATK_JUMP;
                        m_pMonkeyDevilStateMachine->Change_State(CMonkeyDevilStateMachine::STATE_RUN, this);
                    }
                    else if (PAT_2 == m_iAttackPattern)
                    {
                        _uint iRan = _pGameInstance->Get_RandomCount() % 2;
                        if (0 == iRan)
                            m_iAttackIndex = ATK_NORMAL;
                        else
                            m_iAttackIndex = ATK_SP;

                        m_pMonkeyDevilStateMachine->Change_State(CMonkeyDevilStateMachine::STATE_DODGE, this);
                    }
                }
                else
                {
                    m_pTransformCom->Move_Back(_dTimeDelta, m_fCurSpeed / 4.f);
                }
            }
            else
            {
                if (false == RotateLookToPlayer(_dTimeDelta))
                    m_pTransformCom->Move_Back(_dTimeDelta, m_fCurSpeed / 4.f);
                else 
                    m_pMonkeyDevilStateMachine->Change_State(CMonkeyDevilStateMachine::STATE_DODGE, this);
            }
        }
    }
}

void CMonkeyDevil::DODGE_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
    if (true == m_bEndDodge)
    {
        Bind_Dodge_Position();
        m_bEndDodge = false;
    }

    if (CMonkeyDevilStateMachine::STATE_DODGE != m_pMonkeyDevilStateMachine->Get_State())
        return;

    if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
    {
        _float fDistance = PlayerToDistance();

        _float fDot = PlayerToDot_DirToSelfLookConvert();

        if (fDot <= 10.f && fDot >= -10.f)
        {
            if (fDistance <= 3)
            {
                _uint iRan = _pGameInstance->Get_RandomCount() % 2;
                if (0 == iRan)
                    m_iAttackIndex = ATK_NORMAL;
                else
                    m_iAttackIndex = ATK_SP;
                m_pMonkeyDevilStateMachine->Change_State(CMonkeyDevilStateMachine::STATE_ATTACK, this);
            }
            else if (fDistance <= 5)
            {
                m_iAttackIndex = ATK_JUMP;
                m_pMonkeyDevilStateMachine->Change_State(CMonkeyDevilStateMachine::STATE_ATTACK, this);
            }
            else
            {
                m_pMonkeyDevilStateMachine->Change_State(CMonkeyDevilStateMachine::STATE_RUN, this);
            }
        }
        else
        {
            m_pMonkeyDevilStateMachine->Change_State(CMonkeyDevilStateMachine::STATE_RUN, this);
        }

        m_bChangeDodgeMatrix = true;
        m_bEndDodge = true;
    }
    else
    {
        Calculate_OffsetMatrix();
    }
}

void CMonkeyDevil::RUN_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
    if (CMonkeyDevilStateMachine::STATE_RUN != m_pMonkeyDevilStateMachine->Get_State())
        return;

    _float fDistance = PlayerToDistance();
    _float fDot = PlayerToDot_DirToSelfLookConvert();

    if (false == RotateLookToPlayer(_dTimeDelta))
    {
        if (ATK_JUMP == m_iAttackIndex)
        {
            if (m_tJumpAttackTimer.Is_Over(_dTimeDelta))
            {
                m_iAttackIndex = ATK_NORMAL;
                m_tJumpAttackTimer.Reset_Timer();
            }
        }
        m_pTransformCom->Move_Back(_dTimeDelta, m_fCurSpeed / 4.f);
    }
    else
    {
        _float fDistance = PlayerToDistance();
        m_pTransformCom->LookAt(m_pTargetPlayer->Get_OffsetPos());
        if (ATK_JUMP == m_iAttackIndex)
        {
            if (7.f >= fDistance)
                m_pMonkeyDevilStateMachine->Change_State(CMonkeyDevilStateMachine::STATE_ATTACK, this);

            m_pTransformCom->Move_Forward(_dTimeDelta, m_fCurSpeed);
        }
        else if (ATK_NORMAL == m_iAttackIndex || ATK_SP == m_iAttackIndex)
        {
            if (3.f >= fDistance)
                m_pMonkeyDevilStateMachine->Change_State(CMonkeyDevilStateMachine::STATE_ATTACK, this);

            m_pTransformCom->Move_Forward(_dTimeDelta, m_fCurSpeed);
        }
    }
}

void CMonkeyDevil::ATTACK_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
    if (true == m_bEndAttack)
    {
        if (false == m_pModelCom->Is_ChangeAnimation())
        {
            Bind_Attack_Postion();
            m_bEndAttack = false;
        }
    }

    if (CMonkeyDevilStateMachine::STATE_ATTACK != m_pMonkeyDevilStateMachine->Get_State())
        return;

    if (ATK_JUMP == m_iAttackIndex)
    {
        _float fValue = 1.2f;

        if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(fValue))
        {
            _int ranValue = _pGameInstance->Get_RandomCount() % 2;
            if (0 == ranValue)
            {
                m_iAttackPattern = PAT_1;
                m_pMonkeyDevilStateMachine->Change_State(CMonkeyDevilStateMachine::STATE_RETARGET, this);
            }
            else if (1 == ranValue)
            {
                m_iAttackPattern = PAT_2;
                m_pMonkeyDevilStateMachine->Change_State(CMonkeyDevilStateMachine::STATE_RETARGET, this);
            }

            m_pMonkeyDevilStateMachine->Set_AttackCount(0);
        }
        else
        {
            Calculate_OffsetMatrix();
        }
        if (EW_SWORD == m_eWeaponType)
        {
            if (!strcmp("AS_TMonkeyDevil_TSword_AttackJump02_N", m_pModelCom->Get_CurAnimation()->Get_Name().c_str()))
            {
                if (true == m_pModelCom->Get_CurAnimation()->Is_BetweenTime_FastTime())
                {
                    m_pTransformCom->Move_Forward(_dTimeDelta, m_fCurSpeed * 1.8f);
                }
            }
        }
    }
    else if (ATK_NORMAL == m_iAttackIndex || ATK_SP == m_iAttackIndex)
    {
        if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
        {
            _int ranValue = _pGameInstance->Get_RandomCount() % 2;
            if (0 == ranValue)
                m_iAttackPattern = PAT_1;
            else if (1 == ranValue)
                m_iAttackPattern = PAT_2;

            m_pMonkeyDevilStateMachine->Change_State(CMonkeyDevilStateMachine::STATE_RETARGET, this);

            m_pMonkeyDevilStateMachine->Set_AttackCount(0);

            m_bChangeAttackMatrix = true;
            m_bEndAttack = true;
        }
        else
        {
            if (false == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(1.1f))
                Calculate_OffsetMatrix();
        }
    }
}

void CMonkeyDevil::DAMAGED_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
    if (true == m_bEndDamagedPosition)
    {
        Bind_Damaged_Position();
        m_bEndDamagedPosition = false;
    }

    if (CMonkeyDevilStateMachine::STATE_HIT != m_pMonkeyDevilStateMachine->Get_State())
        return;

    if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
    {
        m_pMonkeyDevilStateMachine->Change_State(CMonkeyDevilStateMachine::STATE_RETARGET, this);
        m_bChangeDamagedPosition = true;
        m_bEndDamagedPosition = true;
    }
    else
    {
        Calculate_OffsetMatrix();
    }
}

void CMonkeyDevil::DIE_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
    if (true == m_bEndDiePosition)
    {
        m_bEndDiePosition = false;
        Bind_Die_Position();
    }

    if (CMonkeyDevilStateMachine::STATE_DIE != m_pMonkeyDevilStateMachine->Get_State())
        return;

    m_bDead = true;

    if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
    {
        m_pMonkeyDevilStateMachine->Change_State(CMonkeyDevilStateMachine::STATE_DIE, this);
        m_bChangeDiePosition = true;
        m_bEndDiePosition = true;

        BEGIN_SINGLETON(CGameInstance, pGameInstnace)
        {
            pGameInstnace->Add_DeleteObject(g_iCurrentLevel, LAYER_MONSTER, this);
        }
        END_SINGLETON(pGameInstnace);
    }
    else
    {
        Calculate_OffsetMatrix();
    }
}

#pragma region USE_TOOL_METHOD
_bool CMonkeyDevil::Extern_EditValue()
{
    _bool bActive = false;

    if (ImGui::TreeNodeEx("MonekyDevil"))
    {
        ImGui::PushItemWidth(200);
        Extern_PawnData();
        Extern_GobData();
        Extern_JudgeMonsterAndWeaponType();
        Extern_InitAnation();
        Extern_ModifySightDesc();
        bActive = ImGui::Button("Create");
        ImGui::PopItemWidth();
        ImGui::TreePop();
    }

    return bActive;
}

_bool CMonkeyDevil::Extern_HierakeyView(const string& _str)
{
    if (nullptr == m_pTransformCom)
        return false;

    _bool bActive = false;
    if (ImGui::TreeNodeEx(_str.c_str()))
    {
        ImGui::PushItemWidth(200);
        Extern_PawnData();
        Extern_GobData();
        Control_ColliderData();
        Extern_ShowCurAnimation_Data();
        _bool bClone = ImGui::Button("Clone");
        if (true == bClone)
        {
            BEGIN_SINGLETON(CGameInstance, pGameInstance);
            {
                MONKEYDEVILDESC tDesc;
                tDesc.tGobDesc = m_tGobDesc;
                tDesc.tMonsterDesc = m_tMonsterDesc;
                tDesc.eInitAnimType = m_eInitIdleType;
                tDesc.eWeaponType = m_eWeaponType;
                tDesc.tPawnStatus = m_tPawnStatus;
                pGameInstance->Clone_GameObject(g_iCurrentLevel, Get_Layer(), Get_Proto(), &tDesc);
            }
            END_SINGLETON(pGameInstance);
        }
        bActive = ImGui::Button("Delete");
        ImGui::PopItemWidth();
        ImGui::TreePop();
        return bActive;
    }
    return false;
}

HRESULT CMonkeyDevil::Create_Self(CGameObject** _ppOut)
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        MONKEYDEVILDESC tMonkeyDevilData;
        tMonkeyDevilData.tGobDesc = m_tGobDesc;
        tMonkeyDevilData.tMonsterDesc = m_tMonsterDesc;
        tMonkeyDevilData.eInitAnimType = m_eInitIdleType;
        tMonkeyDevilData.eWeaponType = m_eWeaponType;
        tMonkeyDevilData.tPawnStatus = m_tPawnStatus;
        // Save 
        *_ppOut = static_cast<CGameObject_Client*>(pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, Get_Layer()
            , Get_Proto(), &tMonkeyDevilData));
    }
    END_SINGLETON(pGameInstance);
    return S_OK;
}

HRESULT CMonkeyDevil::Delete_Self()
{
    return __super::Delete_Self();
}

HRESULT CMonkeyDevil::Save_Data()
{
    BEGIN_SINGLETON(CData_Manager, pDataManager);
    {
        MONKEYDEVILDESC tMonkeyDevilData;
        tMonkeyDevilData.tGobDesc = m_tGobDesc;
        tMonkeyDevilData.tMonsterDesc = m_tMonsterDesc;
        tMonkeyDevilData.eInitAnimType = m_eInitIdleType;
        tMonkeyDevilData.eWeaponType = m_eWeaponType;
        tMonkeyDevilData.tPawnStatus = m_tPawnStatus;
        // Save 
        pDataManager->Get_MonsterMgr()->Insert_MonkeyDevilData(tMonkeyDevilData);
    }
    END_SINGLETON(pDataManager);
    return S_OK;
}

HRESULT CMonkeyDevil::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        *_ppOut = static_cast<CGameObject_Client*>(pGameInstance->Clone_GameObject_Add(_eLevel, Get_Layer()
            , Get_Proto(), _pData));
    }
    END_SINGLETON(pGameInstance);
    return S_OK;
}

_bool CMonkeyDevil::Is_Picking(_float3* _pOut)
{
    return __super::Is_Picking(_pOut);
}

void CMonkeyDevil::Reset_Data()
{
    __super::Reset_Data();
}

void CMonkeyDevil::Extern_JudgeMonsterAndWeaponType()
{
    const char* ModelType[2] = { "V1", "V2"};
    const char* WeaponType[2] = { "Hammer", "Sword" };

    ImGui::Combo("ModelType", &m_iModelID, ModelType, 2);
    ImGui::Combo("WeaponType", (int*)&m_eWeaponType, WeaponType, 2);

    switch (m_iModelID)
    {
    case 0:
        m_tMonsterDesc.strModelName = PROTO_COMP_MODEL_MONKEYDEVIL_V1;
        break;
    case 1:
        m_tMonsterDesc.strModelName = PROTO_COMP_MODEL_MONKEYDEVIL_V2;
        break;
    }
}

void CMonkeyDevil::Extern_InitAnation()
{
    const char* IdleType[INIT_ANIMTYPE::INIT_END] = { "NoneFightIdle",
        "WalkIdle", "NoneFightIdleSit",  "PlafondFallLanding", "WallIdle"};
    ImGui::Combo("IdleType", (_int*)&m_eInitIdleType, IdleType, INIT_ANIMTYPE::INIT_END);
}
#pragma region USE_TOOL_METHOD

CMonkeyDevil* CMonkeyDevil::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
    CMonkeyDevil* pInstance = new CMonkeyDevil(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CMonkeyDevil::Create To Failed");
    }
	return pInstance;
}

CGameObject* CMonkeyDevil::Clone(void* pArg)
{
    CMonkeyDevil* pInstance = new CMonkeyDevil(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CMonkeyDevil::Clone To Failed");
    }
    return pInstance;
}

void CMonkeyDevil::Free()
{
     BEGIN_SINGLETON(CGameInstance, pGameInstance)
    {
        pGameInstance->Add_DeleteObject(g_iCurrentLevel, LAYER_WEAPON, m_pEquipmentArr[CEquipment::EQUIP_WEAPON]);
    }
    END_SINGLETON(pGameInstance);
    Safe_Release(m_pMonkeyDevilStateMachine);
    __super::Free();
}
