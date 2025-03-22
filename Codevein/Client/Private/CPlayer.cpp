#include "CMainView.h"
#include"CImGui_Manager.h"
#include "CPlayer.h"
#include "CGameInstance.h"
#include "CCamera_Free.h"
#include "CTwoHandSword.h"
#include "COneHandSword.h"
#include "CCollider.h"
#include "CMonster.h"
#include "CHpBar.h"
#include "CMpBar.h"
#include "CCheckPoints.h"
#include "CPlayerUIController.h"
#include "CData_Manager.h"
#include "CPlayerEPControlGroup.h"
#include "CMuffler.h"
#include "CLevel_GamePlay2.h"
#include "CLevel_Loading.h"

CPlayer::CPlayer(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CCharacter(_pDevice, _pContext)
{

}

CPlayer::CPlayer(const CPlayer& _rhs)
    : CCharacter(_rhs)
{
}

void CPlayer::Set_CameraObject(CCamera_Free* _pCamFree)
{
    Safe_Release(m_pCamerObject); 
    this->m_pCamerObject = _pCamFree; 
    Safe_AddRef(m_pCamerObject);

    if (nullptr != _pCamFree)
    {
        _pCamFree->Set_TargetTransform(m_pTransformCom);
    }
}

void CPlayer::Add_ArroundMonster(CMonster* _pMonster)
{
    if (nullptr == _pMonster || nullptr == m_pRangeCollider)
        return;

    CCollider* pRangleCollider = static_cast<CCollider*>(_pMonster->Get_Component(COM_FIRSTCHECKCOLLIDER));
    if (nullptr == pRangleCollider)
        return;

    if (true == m_pRangeCollider->Is_Collision(pRangleCollider))
    {
        m_AroundMonsterList.push_back(_pMonster);
    }
}

HRESULT CPlayer::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::NativeConstruct_Clone(void* pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(pArg)))
        return E_FAIL;

    if (nullptr != pArg)
    {
        PLAYERDESC tPlayerDesc =*static_cast<PLAYERDESC*>(pArg);
        m_tGobDesc = tPlayerDesc.tGobData;
    }

    m_tPawnStatus = PAWNSTATUS(1, 2000, 1, 2, 1, 1, 0.f, 0.f, 0.f);
    m_tCharInfo.UpdateInfo(m_tPawnStatus);

    if (FAILED(Ready_Component()))
        return E_FAIL;

    if (FAILED(Ready_Others()))
        return E_FAIL;

    m_fMaxSpeed =  m_fCurSpeed = 5.f;

    m_pPlayerEPControlGroup = CPlayerEPControlGroup::Create(m_pDevice, m_pContext, this);
    return S_OK;
}

void CPlayer::Tick(const _double& _dTimeDelta)
{
    if (nullptr != m_pModelCom)
        m_pModelCom->Update_Animation(_dTimeDelta);

    if (false == g_bGameRun)
    {
        if (nullptr != m_pTransformCom)
            m_pTransformCom->Set_Pos(XMVectorSet(0.f, 0.f, 100.f, 0.f));

        Coll_Tick(m_pTransformCom->Get_WorldMatrix());

        if (nullptr == m_pCurRegion)
        {
            BEGIN_SINGLETON(CData_Manager, pDataManager);
            {
                if(nullptr != pDataManager->Get_StageMgr()->Get_Stage())
                    m_pCurRegion = pDataManager->Get_StageMgr()->Get_Stage()->Get_Region(0);
                if (nullptr != m_pCurRegion)
                {
                    CNavigation::NAVIDESC tDesc;
                    tDesc.iCurIndex = 0;
                    m_pNavigation = static_cast<CNavigation*>(m_pCurRegion->Get_Navigation()->Clone(&tDesc));
                    m_pTransformCom->Update_Nav(m_pNavigation);
                }
            }
            END_SINGLETON(pDataManager);
        }

        return;
    }

    __super::Tick(_dTimeDelta);
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        if (false == m_bNotMove)
        {
            Add_AroundPossibleAttackMonster();
            Find_Camera();
            Bind_Supplement_Position();
            // Time Delta 
            Move(_dTimeDelta, pGameInstance);
            Attack(_dTimeDelta, pGameInstance);
            Attack_Sp(_dTimeDelta, pGameInstance);
            Target(_dTimeDelta, pGameInstance);
            Healing(_dTimeDelta, pGameInstance);
            Hit(_dTimeDelta, pGameInstance);
            Dead(_dTimeDelta, pGameInstance);
            Skill(_dTimeDelta, pGameInstance);
            OpenSight(_dTimeDelta, pGameInstance);
            Ect_Behavior(_dTimeDelta, pGameInstance);
            m_pPlayerStateMachine->Tick(_dTimeDelta, this);
            Calculate_OffsetPosition();
            Camera_Control_Update(_dTimeDelta);
            Reset_AroundMonster();
            m_pPlayerEPControlGroup->Update(this, m_pPlayerStateMachine, _dTimeDelta);
            Find_Potal_Ect_Behavior(_dTimeDelta, pGameInstance);
        }
        else
        {
            Ect_Behavior(_dTimeDelta, pGameInstance);
        }
    }
    END_SINGLETON(pGameInstance);
}

void CPlayer::Late_Tick(const _double& _dTimeDelta)
{
    if (true == g_bGameRun || true == m_bNotMove)
    {
        __super::Late_Tick(_dTimeDelta);
        Camera_Control_Late_Update(_dTimeDelta);
        Restrict_AnimationToMoveOffset();
        m_pPlayerEPControlGroup->Late_Update(this, m_pPlayerStateMachine, _dTimeDelta);
    }

    if (nullptr != m_pRendererCom)
        m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CPlayer::Render()
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

            if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_NormalTexture", i, TEXTYPE::TextureType_DIFFUSE)))
                return E_FAIL;

            m_pModelCom->Render(i, m_pShaderCom, 1);
        }
    }
  //  End_Render();
    return S_OK;
}

HRESULT CPlayer::Damaged(CPawn* _pDamageOwner)
{
    if (CPlayerStateMachine::STATE_ECT_BEHABIOR_FINDCHECKPOINTS == m_pPlayerStateMachine->Get_State())
        return E_FAIL;

    if (FAILED(__super::Damaged(_pDamageOwner)))
        return E_FAIL;

    if (CPlayerStateMachine::STATE_ATTACK == m_pPlayerStateMachine->Get_State())
    {
        m_bChangeAttackMatrix = true;
        Bind_Attack_Postion();
        m_bEndAttack = false;
    }
    else if (CPlayerStateMachine::STATE_SPATTACK == m_pPlayerStateMachine->Get_State())
    {
        m_bChangeSpecialAttackMatrix = true;
        Bind_SpecailAttack_Position();
        m_bEndSpecialAttack = false;
    }

    if (CPawn::P_ITEM == _pDamageOwner->Get_PawnType())
    {
        CWeapon* pWeapon = static_cast<CWeapon*>(_pDamageOwner);
        if (nullptr == pWeapon)
            return E_FAIL;

        CTransform* pTranaform = static_cast<CTransform*>(pWeapon->Get_Component(COM_TRANSFORM));
        if (nullptr == pTranaform)
            return E_FAIL;

        XMStoreFloat3(&m_v3DamagedPos, pTranaform->Get_Pos());
   //     m_tCharInfo.Damaged(pWeapon->Get_CharInfo().Get_Damage());

        if (true == m_tCharInfo.Is_CheckDie())
        {
            m_pPlayerStateMachine->Change_State(CPlayerStateMachine::STATE_DIE, this);
        }
        else
        {
            if (true == m_tHurtTimer.bPass)
            {
                m_tHurtTimer.Reset_Timer();
            }
            else
            {
                Calculate_DmgPercent(m_tCharInfo.fCurHp / m_tCharInfo.fMaxHp);
                m_eAtkStrongEncounter = pWeapon->Get_OwnerAtkStrong();
                m_pPlayerStateMachine->Change_State(CPlayerStateMachine::STATE_HIT, this);
            }
        }
    }
    if (CPawn::P_CHARACTER == _pDamageOwner->Get_PawnType())
    {
        CMonster* pCharacter = static_cast<CMonster*>(_pDamageOwner);
        if (nullptr == pCharacter)
            return E_FAIL;

        CTransform* pTransform = static_cast<CTransform*>(_pDamageOwner->Get_Component(COM_TRANSFORM));
        if (nullptr == pTransform)
            return E_FAIL;

        XMStoreFloat3(&m_v3DamagedPos, pTransform->Get_Pos());
        m_tCharInfo.Damaged(pCharacter->Get_CharInfo().Get_Damage());

        if (nullptr != m_pHpBar)
        {
            m_pHpBar->Update_Value(m_tCharInfo.fCurHp);
        }

        if (true == m_tCharInfo.Is_CheckDie())
        {
            m_pPlayerStateMachine->Change_State(CPlayerStateMachine::STATE_DIE, this);
        }
        else
        {
            if (true == m_tHurtTimer.bPass)
            {
                m_tHurtTimer.Reset_Timer();
            }
            else
            {
                Calculate_DmgPercent(m_tCharInfo.fCurHp / m_tCharInfo.fMaxHp);
                m_eAtkStrongEncounter = pCharacter->Get_AtkStr();
                m_pPlayerStateMachine->Change_State(CPlayerStateMachine::STATE_HIT, this);
            }
        }
    }
    return S_OK;
}

_float CPlayer::DamageToDot_DirToSelfLook()
{
    _vector vPos = XMLoadFloat3(&m_v3DamagedPos);
    _float fDot = 0.f;

    _vector vLook1 = vPos - m_pTransformCom->Get_Pos();
    _vector vLook2 = m_pTransformCom->Get_Look();

    XMVectorSetY(vLook1, 0.f);

    _vector vDot = XMVector3Dot(XMVector3Normalize(vLook1), XMVector3Normalize(vLook2));
    return XMVectorGetX(vDot);
}

_float CPlayer::DamageToDot_DirToSelfRight()
{
    _vector vPos = XMLoadFloat3(&m_v3DamagedPos);
    _float fDot = 0.f;

    _vector vLook1 = vPos - m_pTransformCom->Get_Pos();
    _vector vLook2 = m_pTransformCom->Get_Right();

    XMVectorSetY(vLook1, 0.f);

    _vector vDot = XMVector3Dot(XMVector3Normalize(vLook1), XMVector3Normalize(vLook2));
    return XMVectorGetX(vDot);
}

_float CPlayer::DamageToDot_DirToSelfLookConverter()
{
    _float fDot = XMConvertToDegrees(acosf(DamageToDot_DirToSelfLook()));
    _float fValue = DamageToDot_DirToSelfRight();

    if (fValue < 0)
        fDot = -fDot;

    return fDot;
}

HRESULT CPlayer::Ready_Component()
{
    if (FAILED(__super::Ready_Component()))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(COM_ANIMMODEL, LEVEL_STATIC, PROTO_COMP_MODEL_PLAYER, (CComponent**)&m_pModelCom)))
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
        tCollDesc.vScale = _float3(20.5f, 0.8f, 2.5f);
        tCollDesc.vTranslation = _float3(0.f, 0.8f, 0.f);
        if (FAILED(__super::Add_Component(COM_FINDCOLLIDER, LEVEL_STATIC, PROTO_COMP_SPHERECOLLIDER,
            (CComponent**)&m_pRangeCollider, &tCollDesc)))
            return E_FAIL;
    }

    if (FAILED(Reset_TransformData()))
        return E_FAIL;

    if (nullptr != m_pModelCom)
        m_pModelCom->Set_Animation("TCmn_Idle_N_Loop");

    if (FAILED(Find_OffsetNode()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::SetUp_ConstantTable()
{
    if (FAILED(__super::SetUp_ConstantTable()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Ready_Others()
{
    m_pPlayerStateMachine = CPlayerStateMachine::Create(m_pDevice, m_pContext, this);
    if (nullptr == m_pPlayerStateMachine)
        return E_FAIL;

    BEGIN_SINGLETON(CGameInstance, pGameInstance)
    {
        {
            COneHandSword::ONEHANDSWRODDESC tDesc;
            tDesc.tItemDesc = CItem::ITEMDESC(PROTO_COMP_MODEL_HANNIBALSWORD);
            tDesc.tEquipDesc.pOwnerObject = this;
            tDesc.tEquipDesc.strAttachNodeName = "RightHandAttach";
            CEquipment* pWeapon = (CWeapon*)pGameInstance->Clone_GameObject_Add(g_iCurrentLevel, LAYER_WEAPON,
                PROTO_GOB_ONEHANDSWORD, &tDesc);
            m_pEquipmentArr[CEquipment::EQUIP_WEAPON] = pWeapon->ReturnSettingToValue(this);
            Safe_AddRef(pWeapon);
        }

        {
            //CMuffler::MUFFLERDESC tDesc;
            //tDesc.tItemDesc = CItem::ITEMDESC(PROTO_COMP_MODEL_PLAYERMUFFLER);
            //tDesc.tEquipDesc.pOwnerObject = this;
            //tDesc.tEquipDesc.strAttachNodeName = "Spine4";
            //CMuffler* pMullfer = static_cast<CMuffler*>(pGameInstance->Clone_GameObject_Add(g_iCurrentLevel,
            //    LAYER_WEAPON, PROTO_GOB_MUFFLER,&tDesc));
            //Safe_AddRef(pMullfer);
        }
    
        {
            CUserInterface::UIDESC tDesc = CUserInterface::UIDESC(L"T_HPGaugeTexture_UI", _float2(150.f, 630.f), _float2(220.f, 40.f), 0.05f);
            CBar::BARDESC tBarDesc = CBar::BARDESC(m_tCharInfo.fMaxHp, _float2(220.f, 40.f));
            CBar::BARGROUP tGroup = CBar::BARGROUP(tDesc, tBarDesc);
            m_pHpBar = static_cast<CHpBar*>(pGameInstance->Clone_GameObject_Add(g_iCurrentLevel,
                LAYER_UI, PROTO_GOB_UI_HPBAR, &tGroup));
            Safe_AddRef(m_pHpBar);
        }

        {
            CUserInterface::UIDESC tDesc = CUserInterface::UIDESC(L"T_HPGaugeTexture_UI", _float2(140.f, 670.f), _float2(200.f, 30.f), 0.04f);
            CBar::BARDESC tBarDesc = CBar::BARDESC(m_tCharInfo.fMaxHp, _float2(220.f, 30.f));
            CBar::BARGROUP tGroup = CBar::BARGROUP(tDesc, tBarDesc);
            m_pMpBar = static_cast<CMpBar*>(pGameInstance->Clone_GameObject_Add(g_iCurrentLevel,
                LAYER_UI, PROTO_GOB_UI_MPBAR, &tGroup));
            Safe_AddRef(m_pMpBar);
        } 

        {
            m_pPlayerUIController = CPlayerUIController::Create(m_pDevice, m_pContext, this);
            if (nullptr == m_pPlayerUIController)
            {
                END_SINGLETON(pGameInstance);
                return E_FAIL;
            }
        }

    }
    END_SINGLETON(pGameInstance);
    return S_OK;
}

void CPlayer::Find_Camera()
{
    if (nullptr == m_pCamerObject)
    {
        BEGIN_SINGLETON(CGameInstance, pGameInstance)
        {
            m_pCamerObject = static_cast<CCamera_Free*>(pGameInstance->Find_GameObject(g_iCurrentLevel, LAYER_CAM, 0));
            if (nullptr != m_pCamerObject)
            {
                m_pCamerObject->Set_TargetTransform(m_pTransformCom);
                Safe_AddRef(m_pTransformCom);

                CTransform* pTransform = static_cast<CTransform*>(m_pCamerObject->Get_Component(COM_TRANSFORM));
                if (nullptr != pTransform)
                {
                    pTransform->Set_Pos(XMLoadFloat3(&m_v3OffsetPosition) + XMVectorSet(0.f, 4.f, -2.f, 0.f));
                }
            }
        }
        END_SINGLETON(pGameInstance);
    }
}

void CPlayer::Restrict_AnimationToMoveOffset()
{
    if (CPlayerStateMachine::STATE_ATTACK == m_pPlayerStateMachine->Get_State()
        || CPlayerStateMachine::STATE_ROLL == m_pPlayerStateMachine->Get_State()
        || CPlayerStateMachine::STATE_SPATTACK == m_pPlayerStateMachine->Get_State())
    {
        if (nullptr == m_pNavigation)
            return;

    }
}

void CPlayer::LockOn_TargetMonster(CGameInstance* _pGameInstance)
{
    if (_pGameInstance->Get_DIMBtnDown(CInput_Device::DIMB_WHEEL))
    {
        if (m_AroundMonsterList.size() <= 0)
            return;

        list<TARGETMONSTERDESC> DistanceList;
        for (auto& iter : m_AroundMonsterList)
        {
            DistanceList.push_back(TARGETMONSTERDESC(iter, iter->PlayerToDistance()));
        }
        DistanceList.sort([&](const TARGETMONSTERDESC& t1, const TARGETMONSTERDESC& t2)->bool
        {
            return t1.fDistance > t2.fDistance;
        });

        m_pTargetMonster = DistanceList.front().pMonster;
        m_pPlayerStateMachine->Set_Target(true);
    }
}

void CPlayer::Camera_Control_Update(const _double& _dTimeDelta)
{
    if (nullptr == m_pCamerObject)
        return;

    if (nullptr != m_pTargetMonster)
    {
        _vector vLookDir = -XMVector3Normalize(m_pTransformCom->Get_Look());
        _vector vPos = XMVectorSet(0.f, 3.f, XMVectorGetZ(vLookDir * 8.f), 1.f);

        m_pCamerObject->Target_Mode(m_pPlayerStateMachine->Is_Target(), vPos,
            m_pTargetMonster->Get_NodePosition(NODE_CHEST));
    }

    m_pCamerObject->Set_Speed(m_fCurSpeed);
    m_pCamerObject->Set_Move(!m_bRestrictAttackPos);
}

void CPlayer::Camera_Control_Late_Update(const _double& _dTimeDelta)
{
    if (nullptr == m_pCamerObject)
        return;
}

_vector CPlayer::Get_CamToDirection(CTransform* _pTransform)
{
    if (nullptr == _pTransform)
        return XMLoadFloat3(&_float3(0.f, 0.f, 0.f));

    _float3 v3Pos, v3CamPos;

    XMStoreFloat3(&v3Pos, m_pTransformCom->Get_Pos());
    XMStoreFloat3(&v3CamPos, _pTransform->Get_Pos());

    v3Pos.y = 0.f; v3CamPos.y = 0.f;

    return XMLoadFloat3(&v3Pos) - XMLoadFloat3(&v3CamPos);
}

void CPlayer::Control_CameraOffsetPosition()
{

}

#pragma region BIND_MATRIX_REGION

void CPlayer::Calculate_OffsetPosition()
{
    if (nullptr == m_pCamerObject)
        return;

    if (CPlayerStateMachine::STATE_ECT_BEHABIOR_FINDCHECKPOINTS == m_pPlayerStateMachine->Get_State())
        return;

    if (false == m_pModelCom->Is_ChangeAnimation())
    {
        _matrix matWorld =
            m_pNodeArr[NODE_OFFSET]->Get_CombinedTransformationMatrix() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();

        XMStoreFloat3(&m_v3OffsetPosition, matWorld.r[3]);
        matWorld = m_pTransformCom->Get_WorldMatrix();
        matWorld.r[3] = XMLoadFloat3(&m_v3OffsetPosition);
        XMStoreFloat4x4(&m_OffsetMatrix, matWorld);
        XMStoreFloat4x4(&m_DrawAnimMatrix, matWorld);
        Coll_Tick(matWorld);
    }
    else   if (CPlayerStateMachine::STATE_ATTACK == m_pPlayerStateMachine->Get_State()
        || CPlayerStateMachine::STATE_ROLL == m_pPlayerStateMachine->Get_State()
        || CPlayerStateMachine::STATE_SPATTACK == m_pPlayerStateMachine->Get_State())
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

        matWorld.r[3] = XMLoadFloat3(&m_v3OffsetPosition);
        XMStoreFloat4x4(&m_OffsetMatrix, matWorld);
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
    m_pCamerObject->Set_OffsetPosition(XMLoadFloat3(&m_v3OffsetPosition));
}

void CPlayer::Bind_Roll_Position()
{
    if (false == m_bChangeRollMatrix)
        return;

    m_pTransformCom->Set_Pos(XMLoadFloat3(&m_v3OffsetPosition));
    m_bChangeRollMatrix = false;
    m_OffsetMatrix = m_pTransformCom->Get_WorldFloat4x4();
}

void CPlayer::Bind_Attack_Postion()
{
    if (false == m_bChangeAttackMatrix)
        return;

    m_pTransformCom->Set_Pos(XMLoadFloat3((_float3*)&m_OffsetMatrix.m[3][0]));
    m_bChangeAttackMatrix = false;
    m_OffsetMatrix = m_pTransformCom->Get_WorldFloat4x4();
}

void CPlayer::Bind_Supplement_Position()
{
    if (false == m_bChangeSuppleCount)
        return;

    m_pTransformCom->Set_Pos(XMLoadFloat3((_float3*)&m_OffsetMatrix.m[3][0]));
    m_bChangeSuppleCount = false;
    m_OffsetMatrix = m_pTransformCom->Get_WorldFloat4x4();
}

void CPlayer::Bind_SpecailAttack_Position()
{
    if (false == m_bChangeSpecialAttackMatrix)
        return;

    m_pTransformCom->Set_Pos(XMLoadFloat3((_float3*)&m_OffsetMatrix.m[3][0]));
    m_bChangeSpecialAttackMatrix = false;
    m_OffsetMatrix = m_pTransformCom->Get_WorldFloat4x4();
}

void CPlayer::Bind_Hit_Position()
{
    if (false == m_bChangeDamagedMatrix)
        return;

    m_pTransformCom->Set_Pos(XMLoadFloat3((_float3*)&m_OffsetMatrix.m[3][0]));
    m_bChangeDamagedMatrix = false;
    m_OffsetMatrix = m_pTransformCom->Get_WorldFloat4x4();
}

void CPlayer::End_Render()
{
    if (true == m_bEndRollMatrix)
    {
        if (false == m_pModelCom->Get_CurAnimation()->Is_SupplementSituation())
        {
            Bind_Roll_Position();
            m_bEndRollMatrix = false;
        }
    }

}
#pragma endregion BIND_MATRIX_REGION

void CPlayer::Add_AroundPossibleAttackMonster()
{
    if (CPlayerStateMachine::STATE_ATTACK == m_pPlayerStateMachine->Get_State()
        || CPlayerStateMachine::STATE_SPATTACK == m_pPlayerStateMachine->Get_State())
    {
        CWeapon* pWeapon = static_cast<CWeapon*>(m_pEquipmentArr[CEquipment::EQUIP_WEAPON]);
        if (nullptr != pWeapon)
        {
            for (auto& iter : m_AroundMonsterList)
                pWeapon->Add_CheckColliderList(iter);
        }
    }
}

void CPlayer::Reset_AroundMonster()
{
    m_AroundMonsterList.clear();
}

#pragma region STATE_REGION

void CPlayer::Move(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
    if (true == m_bEndRollMatrix)
    {
        Bind_Roll_Position();
        m_bEndRollMatrix = false;
    }

    if (CPlayerStateMachine::STATE_IDLE != m_pPlayerStateMachine->Get_State() 
        && CPlayerStateMachine::STATE_WALK != m_pPlayerStateMachine->Get_State()
       && CPlayerStateMachine::STATE_RUN != m_pPlayerStateMachine->Get_State()
        && CPlayerStateMachine::STATE_ROLL != m_pPlayerStateMachine->Get_State())
        return;

    if (true == m_bChangeAttackMatrix || true == m_bChangeDamagedMatrix || true == m_bChangeDamagedPosition
        || true == m_bChangeDiePosition || true == m_bChangeSpecialAttackMatrix ||
        true == m_bChangeSuppleCount)
        return;

    if (CPlayerStateMachine::STATE_ROLL == m_pPlayerStateMachine->Get_State())
    {
        if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
        {
            m_pPlayerStateMachine->Reset_RollState();
            m_bChangeRollMatrix = true;
            m_bEndRollMatrix = true;
            m_pPlayerStateMachine->Change_State(CPlayerStateMachine::STATE_IDLE, this);
        }
        else
        {
            if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(1.8f))
            {
                if (true == _pGameInstance->Get_DIKeyDown(DIK_SPACE))
                {
                    _bool bIsW = _pGameInstance->Get_DIKeyPressing(DIK_W);
                    _bool bIsA = _pGameInstance->Get_DIKeyPressing(DIK_A);
                    _bool bIsS = _pGameInstance->Get_DIKeyPressing(DIK_S);
                    _bool bIsD = _pGameInstance->Get_DIKeyPressing(DIK_D);

                    if (true == bIsW && true == bIsA)
                        m_eDirection = DIR_FORLEFT;
                    else  if (true == bIsW && true == bIsD)
                        m_eDirection = DIR_FORRIGHT;
                    else if (true == bIsS && true == bIsA)
                        m_eDirection = DIR_BACKLEFT;
                    else  if (true == bIsS && true == bIsD)
                        m_eDirection = DIR_BACKRIGHT;
                    else if (true == bIsW)
                        m_eDirection = DIR_FOR;
                    else if (true == bIsS)
                        m_eDirection = DIR_BACK;
                    else if (true == bIsD)
                        m_eDirection = DIR_RIGHT;
                    else if (true == bIsA)
                        m_eDirection = DIR_LEFT;

                    m_bChangeRollMatrix = true;
                    m_bEndRollMatrix = true;
                    m_pPlayerStateMachine->Set_DoubleRoll(true);
                    m_pPlayerStateMachine->Change_State(CPlayerStateMachine::STATE_ROLL, this);
                    return;
                }
            }
        }
        return;
    }

    _bool bIsW = _pGameInstance->Get_DIKeyPressing(DIK_W);
    _bool bIsA = _pGameInstance->Get_DIKeyPressing(DIK_A);
    _bool bIsS = _pGameInstance->Get_DIKeyPressing(DIK_S);
    _bool bIsD = _pGameInstance->Get_DIKeyPressing(DIK_D);

    if (false == bIsW && false == bIsA && false == bIsS && false == bIsD
         || CPlayerStateMachine::STATE_ATTACK == m_pPlayerStateMachine->Get_PrevState() || 
        CPlayerStateMachine::STATE_SPATTACK == m_pPlayerStateMachine->Get_PrevState())
    {
        m_pPlayerStateMachine->Change_State(CPlayerStateMachine::STATE_IDLE, this);
        m_pPlayerStateMachine->Set_StartRunAndWalk(false);
        return;
    }

    _bool bIsShift = _pGameInstance->Get_DIKeyPressing(DIK_LSHIFT);
    _float fSpeed = 0.f;

    if (true == bIsShift)
    {
        m_pPlayerStateMachine->Change_State(CPlayerStateMachine::STATE_RUN, this);
    }
    else
    {
        m_pPlayerStateMachine->Change_State(CPlayerStateMachine::STATE_WALK, this);
    }

    fSpeed = m_fMaxSpeed * 2.f;

    _float3 v3Pos, v3CamPos;

    if (nullptr == m_pCamerObject)
        return;

    CTransform* pCamTransform = (CTransform*)m_pCamerObject->Get_Component(COM_TRANSFORM);
    if (nullptr == pCamTransform)
    {
        return;
    }

    XMStoreFloat3(&v3Pos, m_pTransformCom->Get_Pos());
    XMStoreFloat3(&v3CamPos, pCamTransform->Get_Pos());

    v3Pos.y = 0.f; v3CamPos.y = 0.f;

    _vector vDirection = Get_CamToDirection(pCamTransform);
    m_ePrevDirection = m_eDirection;
    if (false == m_pPlayerStateMachine->Is_Target())
    {
        if (true == bIsW && true == bIsA)
        {
            m_pTransformCom->LookAt(XMVector3Normalize(vDirection) - XMVector3Normalize(pCamTransform->Get_Right())
                + m_pTransformCom->Get_Pos());
            m_eDirection = DIR_FORLEFT;
        }
        else  if (true == bIsW && true == bIsD)
        {
            m_pTransformCom->LookAt(XMVector3Normalize(vDirection) + XMVector3Normalize(pCamTransform->Get_Right())
                + m_pTransformCom->Get_Pos());
            m_eDirection = DIR_FORRIGHT;
        }
        else if (true == bIsS && true == bIsA)
        {
            m_pTransformCom->LookAt(-XMVector3Normalize(vDirection) - XMVector3Normalize(pCamTransform->Get_Right())
                + m_pTransformCom->Get_Pos());
            m_eDirection = DIR_BACKLEFT;
        }
        else  if (true == bIsS && true == bIsD)
        {
            m_pTransformCom->LookAt(-XMVector3Normalize(vDirection) + pCamTransform->Get_Right()
                + m_pTransformCom->Get_Pos());
            m_eDirection = DIR_BACKRIGHT;
        }
        else if (true == bIsW)
        {
            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + XMVector3Normalize(vDirection));
            m_eDirection = DIR_FOR;
        }
        else if (true == bIsS)
        {
            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() - XMVector3Normalize(vDirection));
            m_eDirection = DIR_BACK;
        }
        else if (true == bIsD)
        {
            m_pTransformCom->LookAt(pCamTransform->Get_Right() + m_pTransformCom->Get_Pos());
            m_eDirection = DIR_RIGHT;
        }
        else if (true == bIsA)
        {
            m_pTransformCom->LookAt(-pCamTransform->Get_Right() + m_pTransformCom->Get_Pos());
            m_eDirection = DIR_LEFT;
        }

        if (_pGameInstance->Get_DIKeyDown(DIK_SPACE))
        {
            _pGameInstance->Reset_KeyBoardContainers();
            m_pPlayerStateMachine->Change_State(CPlayerStateMachine::STATE_ROLL, this);
        }
        else
        {
            if (false == m_pPlayerStateMachine->Is_StartRunAndWalk())
            {
                if ((CPlayerStateMachine::STATE_WALK == m_pPlayerStateMachine->Get_PrevState()) || CPlayerStateMachine::STATE_RUN == m_pPlayerStateMachine->Get_PrevState())
                {
                    if (false == m_pModelCom->Get_CurAnimation()->Is_SupplementSituation())
                    {
                        if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(10.f))
                            m_pPlayerStateMachine->Set_StartRunAndWalk(true);
                    }
                }
            }
            else
                m_pTransformCom->Move_Forward(_dTimeDelta, fSpeed);
        }
    }
    else
    {
        if (true == bIsW && true == bIsA)
            m_eDirection = DIR_FORLEFT;
        else  if (true == bIsW && true == bIsD)
            m_eDirection = DIR_FORRIGHT;
        else if (true == bIsS && true == bIsA)
            m_eDirection = DIR_BACKLEFT;
        else  if (true == bIsS && true == bIsD)
            m_eDirection = DIR_BACKRIGHT;
        else if (true == bIsW)
            m_eDirection = DIR_FOR;
        else if (true == bIsS)
            m_eDirection = DIR_BACK;
        else if (true == bIsD)
            m_eDirection = DIR_RIGHT;
        else if (true == bIsA)
            m_eDirection = DIR_LEFT;

        if (_pGameInstance->Get_DIKeyDown(DIK_SPACE))
        {
            _pGameInstance->Reset_KeyBoardContainers();
            m_pPlayerStateMachine->Change_State(CPlayerStateMachine::STATE_ROLL, this);
        }
        else
        {
            if (false == m_pPlayerStateMachine->Is_StartRunAndWalk())
            {
                if ((CPlayerStateMachine::STATE_WALK == m_pPlayerStateMachine->Get_PrevState()) || CPlayerStateMachine::STATE_RUN == m_pPlayerStateMachine->Get_PrevState())
                {
                    if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(10.f))
                        m_pPlayerStateMachine->Set_StartRunAndWalk(true);
                }
            }
            else
            {
                _vector vDirection = pCamTransform->Get_Look();
                vDirection = XMVectorSetY(vDirection, 0.f);
                if (false == m_pModelCom->Get_CurAnimation()->Is_SupplementSituation())
                {
                    if(DIR_FOR == m_eDirection)
                        m_pTransformCom->Translate_Dir(XMVector3Normalize(vDirection), _dTimeDelta, fSpeed);
                    else  if (DIR_BACK == m_eDirection)
                        m_pTransformCom->Translate_Dir(-XMVector3Normalize(vDirection), _dTimeDelta, fSpeed);
                    else  if (DIR_RIGHT == m_eDirection)
                        m_pTransformCom->Translate_Dir(XMVector3Normalize(pCamTransform->Get_Right()),
                            _dTimeDelta, fSpeed);
                    else  if (DIR_LEFT == m_eDirection)
                        m_pTransformCom->Translate_Dir(XMVector3Normalize(- pCamTransform->Get_Right()),
                            _dTimeDelta, fSpeed);
                    else  if (DIR_FORRIGHT == m_eDirection)
                        m_pTransformCom->Translate_Dir(XMVector3Normalize(vDirection + pCamTransform->Get_Right()),
                            _dTimeDelta, fSpeed);
                    else  if (DIR_FORLEFT == m_eDirection)
                        m_pTransformCom->Translate_Dir(XMVector3Normalize(vDirection -pCamTransform->Get_Right()),
                            _dTimeDelta, fSpeed);
                    else  if (DIR_BACKRIGHT == m_eDirection)
                        m_pTransformCom->Translate_Dir(XMVector3Normalize(-vDirection + pCamTransform->Get_Right()),
                            _dTimeDelta, fSpeed);
                    else  if (DIR_BACKLEFT == m_eDirection)
                        m_pTransformCom->Translate_Dir(XMVector3Normalize(-vDirection - pCamTransform->Get_Right()),
                            _dTimeDelta, fSpeed);
                }
            }
        }
    }
}

void CPlayer::Attack(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
    if (true == m_bEndAttack)
    {
        if (false  == m_pModelCom->Is_ChangeAnimation())
        {
            Bind_Attack_Postion();
            m_bEndAttack = false;
        }
    }

    if (true == m_bChangeRollMatrix || true == m_bChangeDamagedMatrix || true == m_bChangeDamagedPosition
        || true == m_bChangeDiePosition || true == m_bChangeSpecialAttackMatrix)
        return;

    if (CPlayerStateMachine::STATE_SPATTACK == m_pPlayerStateMachine->Get_State() ||
        CPlayerStateMachine::STATE_ROLL == m_pPlayerStateMachine->Get_State() ||
        CPlayerStateMachine::STATE_HIT == m_pPlayerStateMachine->Get_State() ||
        CPlayerStateMachine::STATE_DIE == m_pPlayerStateMachine->Get_State())
        return;

    if (CPlayerStateMachine::STATE_SPATTACK == m_pPlayerStateMachine->Get_PrevState() ||
        CPlayerStateMachine::STATE_ROLL == m_pPlayerStateMachine->Get_PrevState() ||
        CPlayerStateMachine::STATE_HIT == m_pPlayerStateMachine->Get_PrevState() ||
        CPlayerStateMachine::STATE_DIE == m_pPlayerStateMachine->Get_PrevState())
        return;

    if (nullptr == m_pEquipmentArr[CEquipment::EQUIP_WEAPON])
        return;

    if (CPlayerStateMachine::STATE_ATTACK == m_pPlayerStateMachine->Get_State())
    {
        if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
        {
            m_pPlayerStateMachine->Change_State(CPlayerStateMachine::STATE_IDLE, this);
            m_pPlayerStateMachine->Set_NextAttackMotion(false);
            m_pPlayerStateMachine->Set_AttackCount(0);
            m_bChangeAttackMatrix = true;
            m_bChangeSuppleCount = false;
            m_bEndAttack = true;
            return;
        }
    }

    _bool bMouseButton = _pGameInstance->Get_DIMBtnDown(CInput_Device::DIMB_L);
    if (false == bMouseButton)
    {
        return;
    }

    if (false == m_pPlayerStateMachine->Is_NextAttackMotion())
    {
        if (0 == m_pPlayerStateMachine->Get_AttackCount())
        {
            m_pPlayerStateMachine->Change_State(CPlayerStateMachine::STATE_ATTACK, this);
            m_pPlayerStateMachine->Set_NextAttackMotion(true);
        }
    }
    else
    { 
        if (0 == m_pPlayerStateMachine->Get_AttackCount())
        {
            if (false == m_pModelCom->Get_CurAnimation()->Is_SupplementSituation())
            {
                if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(m_pPlayerStateMachine->Get_AttackAnimControlValue()))
                {
                    m_pPlayerStateMachine->Set_NextAttackMotion(true);
                    m_pPlayerStateMachine->Change_State(CPlayerStateMachine::STATE_ATTACK, this);
                    m_bChangeSuppleCount = true;
                }
            }
        }
        else  if (1 == m_pPlayerStateMachine->Get_AttackCount())
        {
            if (false == m_pModelCom->Get_CurAnimation()->Is_SupplementSituation())
            {
                if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(m_pPlayerStateMachine->Get_AttackAnimControlValue()))
                {
                    m_pPlayerStateMachine->Set_NextAttackMotion(true);
                    m_pPlayerStateMachine->Change_State(CPlayerStateMachine::STATE_ATTACK, this);
                    m_bChangeSuppleCount = true;
                }
            }
        }
        else if (2 == m_pPlayerStateMachine->Get_AttackCount())
        {
            if (false == m_pModelCom->Get_CurAnimation()->Is_SupplementSituation())
            {
                if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(m_pPlayerStateMachine->Get_AttackAnimControlValue()))
                {
                    m_pPlayerStateMachine->Set_NextAttackMotion(true);
                    m_pPlayerStateMachine->Change_State(CPlayerStateMachine::STATE_ATTACK, this);
                    m_bChangeSuppleCount = true;
                }
            }
        }
    }
}

void CPlayer::Attack_Sp(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
    if (true == m_bEndSpecialAttack)
    {
        Bind_SpecailAttack_Position();
        m_bEndSpecialAttack = false;
    }

    if (true == m_bChangeSpecialAttackMatrix || true == m_bChangeDamagedMatrix || true == m_bChangeDamagedPosition
        || true == m_bChangeDiePosition || true == m_bChangeRollMatrix ||
        true == m_bChangeSuppleCount)
        return;

    if (nullptr == m_pEquipmentArr[CEquipment::EQUIP_WEAPON])
        return;

    if (CPlayerStateMachine::STATE_ATTACK == m_pPlayerStateMachine->Get_State() ||
        CPlayerStateMachine::STATE_ROLL == m_pPlayerStateMachine->Get_State() ||
        CPlayerStateMachine::STATE_HIT == m_pPlayerStateMachine->Get_State() ||
        CPlayerStateMachine::STATE_DIE == m_pPlayerStateMachine->Get_State())
        return;

    if (CPlayerStateMachine::STATE_ATTACK == m_pPlayerStateMachine->Get_PrevState() ||
        CPlayerStateMachine::STATE_ROLL == m_pPlayerStateMachine->Get_PrevState() ||
        CPlayerStateMachine::STATE_HIT == m_pPlayerStateMachine->Get_PrevState() ||
        CPlayerStateMachine::STATE_DIE == m_pPlayerStateMachine->Get_PrevState())
        return;

    _bool     bChargeAttackStart = _pGameInstance->Get_DIMBtnDown(CInput_Device::DIMB_R);
    m_bChargeAttack = _pGameInstance->Get_DIMBtnPressing(CInput_Device::DIMB_R);

    if (CPlayerStateMachine::STATE_SPATTACK != m_pPlayerStateMachine->Get_State())
    {
        if(true == bChargeAttackStart)
            m_pPlayerStateMachine->Change_State(CPlayerStateMachine::STATE_SPATTACK, this);
    }

    if (CPlayerStateMachine::STATE_SPATTACK == m_pPlayerStateMachine->Get_State())
    {
        if (false == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(5.f))
        {
            if(false== m_bChargeAttack)
                m_pPlayerStateMachine->Change_State(CPlayerStateMachine::STATE_IDLE, this);
        }
        if (true == m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(1.05f))
        {
            m_bChangeSpecialAttackMatrix = true;
            m_bEndSpecialAttack = true;
            m_pPlayerStateMachine->Change_State(CPlayerStateMachine::STATE_IDLE, this);
        }
    }
}

void CPlayer::Target(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
    if (false == m_pPlayerStateMachine->Is_Target())
    {
        LockOn_TargetMonster(_pGameInstance);
    }
    else
    {
        if (true == _pGameInstance->Get_DIMBtnDown(CInput_Device::DIMB_WHEEL))
        {
            m_pPlayerStateMachine->Set_Target(false);
        }
    }
}

void CPlayer::Healing(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
}

void CPlayer::Hit(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
    if (true == m_bEndDamagedMatrix)
    {
        if (false == m_pModelCom->Is_ChangeAnimation())
        {
            Bind_Hit_Position();
            m_bEndDamagedMatrix = false;
        }
    }

    if (CPlayerStateMachine::STATE_HIT != m_pPlayerStateMachine->Get_State())
        return;

    if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
    {
        m_pPlayerStateMachine->Change_State(CPlayerStateMachine::STATE_IDLE, this);
        m_bChangeDamagedMatrix = true;
        m_bEndDamagedMatrix = true;
        Bind_Hit_Position();
        m_bChangeDamagedMatrix = true;
    }
}

void CPlayer::Dead(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
    if (CPlayerStateMachine::STATE_DIE != m_pPlayerStateMachine->Get_State())
        return;

    if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
    {
        // 
    }
}

void CPlayer::Skill(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
}

void CPlayer::OpenSight(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
}

void CPlayer::Ect_Behavior(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
    Find_CheckPoints_Ect_Behavior(_dTimeDelta, _pGameInstance);
}

void CPlayer::Find_CheckPoints_Ect_Behavior(const _double& _dTimeDetla, CGameInstance* _pGameInstance)
{
    if (nullptr != m_pCheckPoints)
    {
        if (false == m_pCheckPoints->Is_Activation())
        {
            m_pPlayerUIController->Get_KnownKey_E()->Set_Active(true);

            if (CPlayerStateMachine::STATE_ECT_BEHABIOR_FINDCHECKPOINTS != m_pPlayerStateMachine->Get_State())
            {
                // Space¸¦ ´­·¶À» ¶§
                if (true == _pGameInstance->Get_DIKeyDown(DIK_E))
                {
                    m_pPlayerStateMachine->Change_State(CPlayerStateMachine::STATE_ECT_BEHABIOR_FINDCHECKPOINTS, this);
                }
            }
            else
            {
                if (1 == m_pPlayerStateMachine->Get_Activation_CheckPoints())
                {
                    if (m_pModelCom->Get_CurAnimation()->Is_Finished())
                    {
                        m_pPlayerStateMachine->Change_State(CPlayerStateMachine::STATE_ECT_BEHABIOR_FINDCHECKPOINTS, this);
                    }
                }
                else  if (2 == m_pPlayerStateMachine->Get_Activation_CheckPoints())
                {
                    if (false == m_pModelCom->Get_PrevAnimation()->Is_SupplementSituation())
                    {
                        if (m_pModelCom->Get_CurAnimation()->Is_Finished())
                        {
                            m_pPlayerStateMachine->Change_State(CPlayerStateMachine::STATE_ECT_BEHABIOR_FINDCHECKPOINTS, this);
                        }
                    }
                }
                else if (3 == m_pPlayerStateMachine->Get_Activation_CheckPoints())
                {
                    if (false == m_pModelCom->Get_PrevAnimation()->Is_SupplementSituation())
                    {
                        if (false == m_bBindActiveCheckPoint)
                        {
                            m_bBindActiveCheckPoint = true;
                            m_pTransformCom->Set_Pos(m_pTransformCom->Get_Pos() + m_pTransformCom->Get_Look());
                        }

                        if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
                        {
                            m_pPlayerStateMachine->Change_State(CPlayerStateMachine::STATE_IDLE, this);
                            m_pPlayerStateMachine->Reset_Activation_CheckPoints();
                            m_pCheckPoints->Set_Activation(true);
                            m_bBindActiveCheckPoint = false;
                        }
                    }
                    
                }
                CTransform* pTransform = static_cast<CTransform*>(m_pCheckPoints->Get_Component(COM_TRANSFORM));
                if (nullptr != pTransform)
                {
                    m_pTransformCom->LookAt(pTransform->Get_Pos());
                    m_pTransformCom->Set_Pos(pTransform->Get_Pos() - m_pTransformCom->Get_Look() * 1.5f);
                }
                XMStoreFloat3(&m_v3OffsetPosition, m_pTransformCom->Get_Pos());
                m_bNotMove = true;
                m_pNodeArr[NODE_OFFSET]->Restrict_Position(m_pTransformCom->Get_Pos());
            }
        }
        else
        {
            if (false == m_pPlayerUIController->Get_CheckPointInterface()->Is_Active())
            {
                m_bNotMove = false;
                m_pPlayerUIController->Get_KnownKey_E()->Set_Active(true);
                // Space¸¦ ´­·¶À» ¶§
                if (true == _pGameInstance->Get_DIKeyDown(DIK_E))
                {
                    m_pPlayerUIController->Get_CheckPointInterface()->Set_Active(true);
                    m_bNotMove = true;
                    m_pCamerObject->Set_Active(false);
                }
            }
            else
            {
                if (true == m_pPlayerUIController->Get_CheckPointInterface()->Is_PadeOutState())
                {
                    CCheckPoints* pCheckPoint = m_pPlayerUIController->Get_CheckPointInterface()->Get_CheckPoints();
                    if (nullptr != pCheckPoint)
                    {
                        if (m_pCheckPoints == pCheckPoint)
                        {
                            m_pPlayerUIController->Get_CheckPointInterface()->Set_PadeOutPM(false);
                            m_pPlayerUIController->Get_CheckPointInterface()->Set_PadeOutState(false);
                            m_pRendererCom->Set_PadeOutValue(0.f);
                        }
                        else
                        {
                            if (true == m_pPlayerUIController->Get_CheckPointInterface()->Is_PadeOutPm())
                            {
                                CTransform* pTransform = static_cast<CTransform*>(pCheckPoint->Get_Component(COM_TRANSFORM));
                                if (nullptr != pTransform)
                                {
                                    m_pTransformCom->Set_Pos(pTransform->Get_Pos() - m_pTransformCom->Get_Look() * 1.5f);
                                    m_pTransformCom->LookAt(pTransform->Get_Pos());
                                    m_pCamerObject->Set_Active(true);
                                    XMStoreFloat3(&m_v3OffsetPosition, m_pTransformCom->Get_Pos());
                                    m_pCamerObject->Set_OffsetPosition(XMLoadFloat3(&m_v3OffsetPosition));
                                }
                                if (0.f == m_pRendererCom->Get_PadeOutValue())
                                {
                                    m_pPlayerUIController->Get_CheckPointInterface()->Set_PadeOutPM(false);
                                    m_pPlayerUIController->Get_CheckPointInterface()->Set_PadeOutState(false);
                                    m_pPlayerUIController->Get_CheckPointInterface()->Set_Active(false);

                                    m_bNotMove = false;
                                    m_pCamerObject->Set_Active(true);
                                    m_pCheckPoints = nullptr;

                                    CTransform* pTransform = static_cast<CTransform*>(pCheckPoint->Get_Component(COM_TRANSFORM));
                                    if (nullptr != pTransform)
                                    {
                                        m_pTransformCom->Set_Pos(pTransform->Get_Pos() - m_pTransformCom->Get_Look() * 1.5f);
                                        m_pTransformCom->LookAt(pTransform->Get_Pos());
                                    }

                                    m_pNavigation->Find_Cell(m_pTransformCom->Get_Pos());
                                    XMStoreFloat3(&m_v3OffsetPosition, m_pTransformCom->Get_Pos());
                                }
                            }
                        }
                    }
                }
                // Space¸¦ ´­·¶À» ¶§
                else if (true == _pGameInstance->Get_DIKeyDown(DIK_E))
                {
                    m_pPlayerUIController->Get_CheckPointInterface()->Set_Active(false);
                    m_bNotMove = false;
                    m_pCamerObject->Set_Active(true);
                }
            }
        }

        m_pCheckPoints = nullptr;
    }
}

void CPlayer::Find_Potal_Ect_Behavior(const _double& _dTimeDetla, CGameInstance* _pGameInstance)
{
    if (nullptr == m_pPotal)
        return;

    m_pPlayerUIController->Get_KnownKey_E()->Set_Active(true);
    // Space¸¦ ´­·¶À» ¶§

    if (true == _pGameInstance->Get_DIKeyDown(DIK_E))
    {
        m_bLevelEnd = true;
    }

    m_pPotal = nullptr;
}

#pragma endregion STATE_REGION

#pragma region USE_TOOL_METHOD

_bool CPlayer::Extern_EditValue() { return false; }

_bool CPlayer::Extern_HierakeyView(const string& _str)
{
    if (ImGui::CollapsingHeader("Player"))
    {
        ImGui::PushItemWidth(200);

        Extern_PawnData();
        Extern_GobData();
        Extern_ShowCurAnimation_Data();
        Control_ColliderData();
        ImGui::PopItemWidth();
        return false;
    }
    return false;
}

HRESULT CPlayer::Create_Self(CGameObject** _ppOut)
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        PLAYERDESC tPlayerDesc;
        tPlayerDesc.tGobData = m_tGobDesc;
        *_ppOut = static_cast<CGameObject_Client*>(pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, Get_Layer()
            , Get_Proto(), &tPlayerDesc));
    }
    END_SINGLETON(pGameInstance);
    return S_OK;
}

HRESULT CPlayer::Delete_Self()
{
    return S_OK;
}

HRESULT CPlayer::Save_Data()
{
    return S_OK;
}

HRESULT CPlayer::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
    return S_OK;
}

_bool CPlayer::Is_Picking(_float3* _pOut)
{
    return __super::Is_Picking(_pOut);
}

void CPlayer::Reset_Data()
{
    Reset_TransformData();
}
#pragma endregion USE_TOOL_METHOD


CPlayer* CPlayer::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
    CPlayer* pInstance = new CPlayer(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CPlayer::Create to Failed");
    }
    return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
    CPlayer* pInstance = new CPlayer(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CPlayer::Clone to Failed");
    }
    return pInstance;
}

void CPlayer::Free()
{
    Safe_Release(m_pPlayerStateMachine);
    Safe_Release(m_pHpBar);
    Safe_Release(m_pMpBar);
    Safe_Release(m_pPlayerUIController);
    Safe_Release(m_pPlayerEPControlGroup);
    Safe_Release(m_pMuffler);
    __super::Free();
}
