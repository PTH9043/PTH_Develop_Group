#include "CMainView.h"
#include"CImGui_Manager.h"
#include "CWolfDevil.h"
#include "CGameInstance.h"
#include "CData_Manager.h"
#include "CPlayer.h"
#include "CMonsterEPController.h"

CWolfDevil::CWolfDevil(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CMonster(_pDevice, _pContext)
{
}

CWolfDevil::CWolfDevil(const CWolfDevil& _rhs)
	: CMonster(_rhs)
{
}

HRESULT CWolfDevil::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CWolfDevil::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

    if (nullptr != pArg)
    {
        WOLFDEVILDESC tDesc = *static_cast<WOLFDEVILDESC*>(pArg);
        m_eInitIdleType = tDesc.eInitAnimType;
        m_tGobDesc = tDesc.tGobDesc;
        m_tMonsterDesc = tDesc.tMonsterDesc;
        m_tPawnStatus = tDesc.tPawnStatus;

        m_tMonsterDesc.tSightDesc.fFindAngle = 60.f;
        m_tMonsterDesc.tSightDesc.fFindDistance = 10.f;
        m_tMonsterDesc.tSightDesc.fYFindAngle = 1.f;
        m_tCharInfo.UpdateInfo(m_tPawnStatus);
    }

    if (FAILED(Ready_Component()))
        return E_FAIL;

    if (FAILED(Ready_Others()))
        return E_FAIL;

    m_fCurSpeed = 8.f;
    m_fRotationStandardValue = XMConvertToRadians(90.f);

    m_pWolfDevilStateMachine->Change_State(CWolfDevilStateMachine::STATE_IDLE, this);

    if(nullptr != m_pCollCollider)
        m_fDrawRadius = sqrtf(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pCollCollider->Get_Scale()) * 2.f, XMLoadFloat3(&m_pCollCollider->Get_Scale()) * 2.f)));
	
    m_pNavigation->Find_Cell(m_pTransformCom->Get_Pos());
    m_pTransformCom->Update_Nav(m_pNavigation);
    return S_OK;
}

void CWolfDevil::Tick(const _double& _dTimeDelta)
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
        Bind_Supplement_Position();
        IDLE_STATE(_dTimeDelta, pGameInstance);
        FIND_STATE(_dTimeDelta, pGameInstance);
        RETARGET_STATE(_dTimeDelta, pGameInstance);
        DODGE_STATE(_dTimeDelta, pGameInstance);
        RUN_STATE(_dTimeDelta, pGameInstance);
        ATTACK_STATE(_dTimeDelta, pGameInstance);
        DAMAGED_STATE(_dTimeDelta, pGameInstance);
        DIE_STATE(_dTimeDelta, pGameInstance);
        m_pWolfDevilStateMachine->Tick(_dTimeDelta, this);
        Calculate_OffsetPosition();
        Add_AroundPossibleAttackPlayer();
    }
    END_SINGLETON(pGameInstance);
}

void CWolfDevil::Late_Tick(const _double& _dTimeDelta)
{
    if (true == g_bGameRun)
    {
        m_pWolfDevilStateMachine->Late_Tick(_dTimeDelta, this);
        Restrict_AnimationToMoveOffset(m_pWolfDevilStateMachine);
    }
    __super::Late_Tick(_dTimeDelta);
}

HRESULT CWolfDevil::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWolfDevil::Damaged(CPawn* _pDamageOwner)
{
    if ( CWolfDevilStateMachine::STATE_DIE == m_pWolfDevilStateMachine->Get_State())
        return E_FAIL;

    if (CPawn::P_ITEM == _pDamageOwner->Get_PawnType())
    {
        CWeapon* pWeapon = static_cast<CWeapon*>(_pDamageOwner);
        if (nullptr == pWeapon)
            return E_FAIL;

        if (CWolfDevilStateMachine::STATE_HIT == m_pWolfDevilStateMachine->Get_State())
            return S_OK;

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

            m_pWolfDevilStateMachine->Change_State(CWolfDevilStateMachine::STATE_DIE, this);
            m_bDead = true;
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

                m_tHurtTimer.Reset_Timer();
                if (CWolfDevilStateMachine::STATE_ATTACK == m_pWolfDevilStateMachine->Get_State())
                    return E_FAIL;
                m_pWolfDevilStateMachine->Change_State(CWolfDevilStateMachine::STATE_HIT, this);
            }
        }
    }
    else  if (CPawn::P_CHARACTER == _pDamageOwner->Get_PawnType())
    {

    }

    return S_OK;
}

HRESULT CWolfDevil::Ready_Component()
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
        tCollDesc.vScale = _float3(0.8f, 0.7f, 0.8f);
        tCollDesc.vTranslation = _float3(0.f, 0.7f, 0.f);
        /* For.Com_Shader */
        if (FAILED(__super::Add_Component(COM_COLLCOLLIDER, LEVEL_STATIC,
            PROTO_COMP_OBBCOLLIDER, (CComponent**)&m_pCollCollider, &tCollDesc)))
            return E_FAIL;
    }

    {
        // First Check
        CCollider::COLLIDERDESC tCollDesc;
        tCollDesc.vScale = _float3(0.8f, 0.7f, 0.8f);
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

HRESULT CWolfDevil::SetUp_ConstantTable()
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWolfDevil::Ready_Others()
{
	if (FAILED(__super::Ready_Others()))
		return E_FAIL;

    m_pWolfDevilStateMachine = CWolfDevilStateMachine::Create(m_pDevice, m_pContext, this);
    if (nullptr == m_pWolfDevilStateMachine)
        return E_FAIL;

	return S_OK;
}

void CWolfDevil::Calculate_OffsetPosition()
{
    
    if (CWolfDevilStateMachine::STATE_ATTACK == m_pWolfDevilStateMachine->Get_State())
    {
        _matrix World = m_pTransformCom->Get_WorldMatrix();
        World.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
        _matrix matWorld = 
            m_pNodeArr[NODE_OFFSET]->Get_CombinedTransformationMatrix() * 
            m_pModelCom->Get_PivotMatrix() * World;
        XMStoreFloat3(&m_v3OffsetPosition, matWorld.r[3] + m_pTransformCom->Get_Pos());

        if (nullptr != m_pAttackPosCell)
        {

         }
        else
        {
            _float3 v3Pos;
            XMStoreFloat3(&v3Pos, m_pTransformCom->Get_Pos());
            m_v3OffsetPosition.y = v3Pos.y;
        }

        matWorld = m_pTransformCom->Get_WorldMatrix();
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

void CWolfDevil::Bind_Dodge_Position()
{
    if (false == m_bChangeDodgeMatrix)
        return;

    Bind_Position();
    m_bChangeDodgeMatrix = false;
}

void CWolfDevil::Add_AroundPossibleAttackPlayer()
{
    if (CWolfDevilStateMachine::STATE_ATTACK == m_pWolfDevilStateMachine->Get_State())
    {
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

void CWolfDevil::Judge_AttackStrongCount()
{
}

#pragma region STATE
void CWolfDevil::IDLE_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{

    if (nullptr == m_pTargetPlayer)
        return;

    if (CWolfDevilStateMachine::STATE_IDLE != m_pWolfDevilStateMachine->Get_State())
        return;

    CCollider* pPlayerCheckCollider = static_cast<CCollider*>(m_pTargetPlayer->Get_Component(COM_FIRSTCHECKCOLLIDER));
    CTransform* pPlayerTransform = static_cast<CTransform*>(m_pTargetPlayer->Get_Component(COM_TRANSFORM));

    if (nullptr == pPlayerCheckCollider || nullptr == pPlayerTransform)
        return;

    if (false == m_bFindPlayer)
    {
        if (true == m_pAttackNowRangeCollider->Is_Collision(pPlayerCheckCollider))
        {
            m_pWolfDevilStateMachine->Change_State(CWolfDevilStateMachine::STATE_FIND, this);
            m_bFindPlayer = true;
        }
        else
            m_pWolfDevilStateMachine->Change_State(CWolfDevilStateMachine::STATE_IDLE, this);
    }
    else
    {
        m_pWolfDevilStateMachine->Change_State(CWolfDevilStateMachine::STATE_FIND, this);
    }
}

void CWolfDevil::FIND_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
    if (CWolfDevilStateMachine::STATE_FIND != m_pWolfDevilStateMachine->Get_State())
        return;

    if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
    {
        _float fDistance = PlayerToDistance();
        if (fDistance <= 3)
        {
            m_iAttackPattern = PAT_3;
        }
        else
        {
            _int ranValue = _pGameInstance->Get_RandomCount() % 10;
            if (0 <= ranValue && ranValue <= 3)
                m_iAttackPattern = PAT_1;
            else if (4 <= ranValue && ranValue <= 9)
                m_iAttackPattern = PAT_2;
        }

        if (PAT_3 == m_iAttackPattern)
            m_pWolfDevilStateMachine->Change_State(CWolfDevilStateMachine::STATE_DODGE, this);
        else
            m_pWolfDevilStateMachine->Change_State(CWolfDevilStateMachine::STATE_RETARGET, this);
    }
}

void CWolfDevil::RETARGET_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
    if (CWolfDevilStateMachine::STATE_RETARGET != m_pWolfDevilStateMachine->Get_State())
        return;

    if (false == RotateLookToPlayer_JudgeNotRepeat(_dTimeDelta, 10.))
    {
        m_pTransformCom->Move_Forward(_dTimeDelta, m_fCurSpeed);
    }
    else
    {
        _float fDistance = PlayerToDistance();
        if (fDistance <= 3)
        {
            m_iAttackPattern = PAT_3;
        }
        else
        {
            _int ranValue = _pGameInstance->Get_RandomCount() % 10;
            if (0 <= ranValue && ranValue <= 3)
                m_iAttackPattern = PAT_1;
            else if (4 <= ranValue && ranValue <= 9)
                m_iAttackPattern = PAT_2;
        }

        m_pTransformCom->Move_Forward(_dTimeDelta, m_fCurSpeed);
        m_pWolfDevilStateMachine->Change_State(CWolfDevilStateMachine::STATE_RUN, this);
    }
}

void CWolfDevil::DODGE_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
    if (true == m_bEndDodge)
    {
        Bind_Dodge_Position();
        m_bEndDodge = false;
    }

    if (CWolfDevilStateMachine::STATE_DODGE != m_pWolfDevilStateMachine->Get_State())
        return;

    if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
    {
        m_iAttackIndex = ATK_SP;
        m_bChangeDodgeMatrix = true;
        m_bEndDodge = true;
        m_bChangeDodgeMatrix = true;
        m_pWolfDevilStateMachine->Change_State(CWolfDevilStateMachine::STATE_RUN, this);
    }
    else
    {
        Calculate_OffsetMatrix();
    }
}

void CWolfDevil::RUN_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
    if (CWolfDevilStateMachine::STATE_RUN != m_pWolfDevilStateMachine->Get_State())
        return;

    if (false == RotateLookToPlayer_JudgeNotRepeat(_dTimeDelta, 10.))
    {
        m_pTransformCom->Move_Forward(_dTimeDelta, m_fCurSpeed);
    }
    else
    {
        _float fDistance = PlayerToDistance();
        if (PAT_1 == m_iAttackPattern)
        {
            m_iAttackIndex = ATK_NORMAL;
            if (fDistance <= 3.f)
                m_pWolfDevilStateMachine->Change_State(CWolfDevilStateMachine::STATE_ATTACK, this);
            else
                m_pTransformCom->Move_Forward(_dTimeDelta, m_fCurSpeed);
        }
        else if (PAT_2 == m_iAttackPattern)
        {
            m_iAttackIndex = ATK_JUMP;

            if (fDistance < 3.5f)
                m_pWolfDevilStateMachine->Change_State(CWolfDevilStateMachine::STATE_ATTACK, this);
            else
                m_pTransformCom->Move_Forward(_dTimeDelta, m_fCurSpeed);
        }
        else if (PAT_3 == m_iAttackPattern)
        {
            m_iAttackIndex = ATK_JUMP;
            if (fDistance < 3.5f)
                m_pWolfDevilStateMachine->Change_State(CWolfDevilStateMachine::STATE_ATTACK, this);
            else
                m_pTransformCom->Move_Forward(_dTimeDelta, m_fCurSpeed);
        }
    }
}

void CWolfDevil::ATTACK_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
    if (true == m_bEndAttack)
    {
        Bind_Attack_Postion();
        m_bEndAttack = false;
    }

    if (CWolfDevilStateMachine::STATE_ATTACK != m_pWolfDevilStateMachine->Get_State())
        return;

    if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
    {
        m_bChangeAttackMatrix = true;
        m_bEndAttack = true;
        m_pWolfDevilStateMachine->Change_State(CWolfDevilStateMachine::STATE_RETARGET, this);
    }
    else
    {
        Calculate_OffsetMatrix();
    }
}

void CWolfDevil::DAMAGED_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
    if (true == m_bEndDamagedPosition)
    {
        Bind_Damaged_Position();
        m_bEndDamagedPosition = false;
    }

    if (CWolfDevilStateMachine::STATE_HIT != m_pWolfDevilStateMachine->Get_State())
        return;
    if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
    {
        m_bChangeDamagedPosition = true;
        m_bEndDamagedPosition = true;
        m_pWolfDevilStateMachine->Change_State(CWolfDevilStateMachine::STATE_RETARGET, this);
        m_bChangeDamagedPosition = true;
    }
    else
    {
        Calculate_OffsetMatrix();
    }
}

void CWolfDevil::DIE_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
    if (true == m_bEndDiePosition)
    {
        Bind_Die_Position();
        m_bEndDiePosition = false;
    }

    if (CWolfDevilStateMachine::STATE_DIE != m_pWolfDevilStateMachine->Get_State())
        return;

    if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
    {
        m_bChangeDamagedPosition = true;
        m_bEndDamagedPosition = true;

        BEGIN_SINGLETON(CGameInstance, pGameInstnace)
        {
            pGameInstnace->Add_DeleteObject(g_iCurrentLevel, LAYER_MONSTER, this);
        }
        END_SINGLETON(pGameInstnace);
    }
}

#pragma endregion STATE

#pragma region USE_TOOL_METHOD
_bool CWolfDevil::Extern_EditValue()
{
    _bool bActive = false;

    if (ImGui::TreeNodeEx(Get_Name()))
    {
        ImGui::PushItemWidth(200);
        Extern_PawnData();
        Extern_GobData();
        Extern_InitAnation();
        Extern_ModelType();
        Extern_ModifySightDesc();
        bActive = ImGui::Button("Create");
        ImGui::PopItemWidth();
        ImGui::TreePop();
    }

    return bActive;
}

_bool CWolfDevil::Extern_HierakeyView(const string& _str)
{
	if (nullptr == m_pTransformCom)
		return false;

	_bool bActive = false;
	if (ImGui::TreeNodeEx(_str.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
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
                WOLFDEVILDESC tDesc;
                tDesc.eInitAnimType = m_eInitIdleType;
                tDesc.tGobDesc = m_tGobDesc;
                tDesc.tMonsterDesc = m_tMonsterDesc;
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

HRESULT CWolfDevil::Create_Self(CGameObject** _ppOut)
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        WOLFDEVILDESC tDesc;
        tDesc.eInitAnimType = m_eInitIdleType;
        tDesc.tGobDesc = m_tGobDesc;
        tDesc.tMonsterDesc = m_tMonsterDesc;
        tDesc.tPawnStatus = m_tPawnStatus;
        // Save 
        *_ppOut = static_cast<CGameObject_Client*>(pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, Get_Layer()
            , Get_Proto(), &tDesc));
    }
    END_SINGLETON(pGameInstance);
    return S_OK;
}

HRESULT CWolfDevil::Delete_Self()
{
    return __super::Delete_Self();
}

HRESULT CWolfDevil::Save_Data()
{
    BEGIN_SINGLETON(CData_Manager, pDataManager);
    {
        WOLFDEVILDESC tDesc;
        tDesc.eInitAnimType = m_eInitIdleType;
        tDesc.tGobDesc = m_tGobDesc;
        tDesc.tMonsterDesc = m_tMonsterDesc;
        tDesc.tPawnStatus = m_tPawnStatus;
        // Save 
        pDataManager->Get_MonsterMgr()->Insert_WolfDevilData(tDesc);
    }
    END_SINGLETON(pDataManager);
    return S_OK;
}

HRESULT CWolfDevil::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        *_ppOut = static_cast<CGameObject_Client*>(pGameInstance->Clone_GameObject_Add(_eLevel, Get_Layer()
            , Get_Proto(), _pData));
    }
    END_SINGLETON(pGameInstance);
    return S_OK;
}

_bool CWolfDevil::Is_Picking(_float3* _pOut)
{
    return __super::Is_Picking(_pOut);
}

void CWolfDevil::Reset_Data()
{
    __super::Reset_Data();
}

void CWolfDevil::Extern_InitAnation()
{
    const char* IdleType[INIT_ANIMTYPE::INIT_END] = { "Idle",
    "Sit", "Eat",  "Walk" };
    ImGui::Combo("IdleType", (_int*)&m_eInitIdleType, IdleType, INIT_ANIMTYPE::INIT_END);
}
void CWolfDevil::Extern_ModelType()
{
    const char* ModelType[2] = { "V1", "V2" };
    ImGui::Combo("ModelType", &m_iModelID, ModelType, 2);
    switch (m_iModelID)
    {
    case 0:
        m_tMonsterDesc.strModelName = PROTO_COMP_MODEL_WOLFDEVIL_V1;
        break;
    case 1:
        m_tMonsterDesc.strModelName = PROTO_COMP_MODEL_WOLFDEVIL_V2;
        break;
    }
}
#pragma endregion USE_TOOL_METHOD

CWolfDevil* CWolfDevil::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
    CWolfDevil* pInstance = new CWolfDevil(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("WolfDevil::Create To Failed");
    }
	return pInstance;
}

CGameObject* CWolfDevil::Clone(void* pArg)
{
    CWolfDevil* pInstance = new CWolfDevil(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("WolfDevil::Clone To Failed");
    }
    return pInstance;
}

void CWolfDevil::Free()
{
    Safe_Release(m_pWolfDevilStateMachine);
    __super::Free();
}
