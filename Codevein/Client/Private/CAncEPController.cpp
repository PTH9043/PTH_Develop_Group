#include "CAncEPController.h"
#include "CEffectParticleGroup.h"
#include "CAncientHannibal.h"
#include "CGameInstance.h"
#include "CParticle.h"
#include "C2DEffectParticle.h"

CAncEPController::CAncEPController(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice(_pDevice), m_pContext(_pContext)
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CAncEPController::NativeConstruct(void* _pArg)
{
    if (nullptr == _pArg)
        return E_FAIL;

    ANCEPDESC tDesc = *static_cast<ANCEPDESC*>(_pArg);

    BEGIN_SINGLETON(CGameInstance, pGameInstance)
    {
        for (_uint i = 0; i < 3; ++i)
        {
            CEffectParticleGroup* pInstance = static_cast<CEffectParticleGroup*>(pGameInstance->Clone_GameObject_Add(g_iCurrentLevel,
                LAYER_EFFECTPARTICLE, PROTO_GOB_EFFPARTICLE_JUMP_DUST_GROUP));
            Safe_AddRef(pInstance);
            pInstance->Active(false);
            m_JumpDustList.push_back(pInstance);
        }

        for (_uint i = 0; i < 2; ++i)
        {
            CEffectParticleGroup* pInstance = static_cast<CEffectParticleGroup*>(pGameInstance->Clone_GameObject_Add(LEVEL_STATIC,
                LAYER_EFFECTPARTICLE, PROTO_GOB_DRAGONHAND_PARTICLE));
            Safe_AddRef(pInstance);
            m_HandParticle.push_back(pInstance);
        }

        for (_uint i = 0; i < 15; ++i)
        {
            {
                CEffectParticleGroup* pInstance = static_cast<CEffectParticleGroup*>(pGameInstance->Clone_GameObject_Add(LEVEL_STATIC,
                    LAYER_EFFECTPARTICLE, PROTO_GOB_EFFPARTICLE_FIREWALL));
                Safe_AddRef(pInstance);
                pInstance->Active(false);
                m_FireWallList.push_back(pInstance);

            }
            {
                CEffectParticleGroup* pInstance = static_cast<CEffectParticleGroup*>(pGameInstance->Clone_GameObject_Add(LEVEL_STATIC,
                    LAYER_EFFECTPARTICLE, PROTO_GOB_EFFPARTICLE_DRAGON_MAGIC));
                Safe_AddRef(pInstance);
                pInstance->Active(false);
                m_DragonMagic.push_back(pInstance);
            }
            {
                CCollider::COLLIDERDESC tDesc;
                tDesc.vScale = _float3(3.f, 0.f, 0.f);
                tDesc.vTranslation = _float3(0.f, 0.f, 0.f);
                CCollider* pCollider = static_cast<CCollider*>(pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_SPHERECOLLIDER
                    , &tDesc));
                m_CollisionList.push_back(pCollider);
            }
        }

        for (auto& iter : tDesc.pInstance->Get_BonesColliderDesc())
        {
            if (!strcmp("joint_LeftHandMiddle1", iter.pHierachyNode->Get_Name().c_str()))
            {
                m_HandHierackyNodeList.push_back(iter.pHierachyNode);
                Safe_AddRef(iter.pHierachyNode);
            }
            else if (!strcmp("joint_RightHandMiddle1", iter.pHierachyNode->Get_Name().c_str()))
            {
                m_HandHierackyNodeList.push_back(iter.pHierachyNode);
                Safe_AddRef(iter.pHierachyNode);
            }
        }

        _float3 v3InitPos = _float3(-100.f, 0.f, -100.f);
        for (_uint i = 0; i < 50; ++i)
        {
            for (_uint j = 0; j < 50; ++j)
            {
                if (!(v3InitPos.x == 0 && v3InitPos.z == 0))
                {
                    m_vAttackPositionList.push_back(v3InitPos);
                }
                v3InitPos.x += 4.f;
            }
            v3InitPos.x = -100.f;
            v3InitPos.z += 4.f;
        }
    }
    END_SINGLETON(pGameInstance);

    return S_OK;
}

void CAncEPController::Update(const _double& _dDeltaSeconds, CAncHanibalStateMachine* _pMonsterStateMachine, CAncientHannibal* _pMonster)
{
    Update_HandParticlePositon(_dDeltaSeconds, _pMonsterStateMachine, _pMonster);
}

void CAncEPController::Late_Update(const _double& _dDeltaSeconds, CAncHanibalStateMachine* _pMonsterStateMachine, CAncientHannibal* _pMonster)
{
}

void CAncEPController::Attack_FireWallStatre(_fvector vPlayerPos, _fvector vPos)
{
    if (false == m_bTurnOnLifeLoop_FireWall)
    {
        for (auto& iter : m_FireWallList)
        {
            iter->Turn_On_LifeLoop();
            iter->Turn_OnLoop();
            iter->Not_Reset(true);
            iter->Reset();
        }
        _vector _vPlayerPos = vPlayerPos;
        _vector _vPos = vPos;

        sort(m_vAttackPositionList.begin(), m_vAttackPositionList.end(), [&](const _float3& Pos1, const _float3& Pos2)->bool
        {
            float fLength1 = XMVectorGetX(XMVector3Length(_vPlayerPos - (_vPos + XMLoadFloat3(&Pos1))));
            float fLength2 = XMVectorGetX(XMVector3Length(_vPlayerPos - (_vPos + XMLoadFloat3(&Pos2))));

            return fLength1 < fLength2;
        });

        m_bTurnOnLifeLoop_FireWall = true;

        _uint k = 0;
        for (_uint i = 0; i < 15; ++i)
        {
            m_DragonMagic[i]->Active(true);
            m_DragonMagic[i]->Set_Pos(XMLoadFloat3(&m_vAttackPositionList[k + i]) + vPos);
        }
        return;
    }

    if (false == m_bActive_FireWall)
    {
        _bool bIsFalse = false;
        for (auto& iter : m_DragonMagic)
        {
            if (nullptr != iter->Get_2DEffectParticle())
            {
                bIsFalse = iter->Get_2DEffectParticle()->Is_Active();
                if(true == bIsFalse)
                    break;
            }
            else
            {
                bIsFalse = true;
                break;
            }
        }

        if (false == bIsFalse)
        {
            _uint k = 0;
            for (_uint i = 0; i < 15; ++i)
            {
                m_FireWallList[i]->Set_Pos(XMLoadFloat3(&m_vAttackPositionList[k + i]) + vPos);
                m_FireWallList[i]->Active(true);

                CTransform* pTransform = static_cast<CTransform*>(m_FireWallList[i]->Get_Component(COM_TRANSFORM));
                if (nullptr != pTransform)
                {
                    m_CollisionList[i]->Tick(pTransform->Get_WorldMatrix());
                }
            }

            m_bStopAnimation = false;
            m_bActive_FireWall = true;
        }
        else
        {

            m_bStopAnimation = true;
        }
    }
}

void CAncEPController::Attack_Fire_WallState_Just5(_fvector vPlayerPos, _fvector vPos)
{
    if (false == m_bTurnOnLifeLoop_FireWall)
    {
        for (auto& iter : m_FireWallList)
        {
            iter->Turn_On_LifeLoop();
            iter->Turn_OnLoop();
            iter->Not_Reset(true);
            iter->Reset();
        }
        _vector _vPlayerPos = vPlayerPos;
        _vector _vPos = vPos;

        sort(m_vAttackPositionList.begin(), m_vAttackPositionList.end(), [&](const _float3& Pos1, const _float3& Pos2)->bool
        {
            float fLength1 = XMVectorGetX(XMVector3Length(_vPlayerPos - (_vPos + XMLoadFloat3(&Pos1))));
            float fLength2 = XMVectorGetX(XMVector3Length(_vPlayerPos - (_vPos + XMLoadFloat3(&Pos2))));

            return fLength1 < fLength2;
        });

        m_bTurnOnLifeLoop_FireWall = true;

        _uint k = 0;
        for (_uint i = 0; i < 5; ++i)
        {
            m_DragonMagic[i]->Active(true);
            m_DragonMagic[i]->Set_Pos(XMLoadFloat3(&m_vAttackPositionList[k + i]) + vPos);
        }
        return;
    }

    if (false == m_bActive_FireWall)
    {
        _bool bIsFalse = false;
        for (auto& iter : m_DragonMagic)
        {
            if (nullptr != iter->Get_2DEffectParticle())
            {
                bIsFalse = iter->Get_2DEffectParticle()->Is_Active();
                if (true == bIsFalse)
                    break;
            }
            else
            {
                bIsFalse = true;
                break;
            }
        }

        if (false == bIsFalse)
        {
            _uint k = 0;
            for (_uint i = 0; i < 5; ++i)
            {
                m_FireWallList[i]->Set_Pos(XMLoadFloat3(&m_vAttackPositionList[k + i]) + vPos);
                m_FireWallList[i]->Active(true);

                CTransform* pTransform = static_cast<CTransform*>(m_FireWallList[i]->Get_Component(COM_TRANSFORM));
                if (nullptr != pTransform)
                {
                    m_CollisionList[i]->Tick(pTransform->Get_WorldMatrix());
                }
            }

            m_bStopAnimation = false;
            m_bActive_FireWall = true;
        }
        else
        {

            m_bStopAnimation = true;
        }
    }
}

void CAncEPController::End_FireWallState()
{
    for (auto& iter : m_FireWallList)
    {
        iter->Turn_Off_LifeLoop();
        iter->Turn_OffLoop(5.f);
        iter->Not_Reset(false);
    }
    for (_uint i = 0; i < 15; ++i)
    {
        m_DragonMagic[i]->Reset();
        m_DragonMagic[i]->Active(false);
    }
    m_bTurnOnLifeLoop_FireWall = false;
    m_bActive_FireWall = false;
    for (auto& iter : m_CollisionList)
    {
        iter->Tick(XMMatrixTranslation(-10000.f, -10000.f, -10000.f));
    }
}

void CAncEPController::Jump_State(_fvector vPos)
{
    if (m_iJumpDustIndex >= m_JumpDustList.size())
        m_iJumpDustIndex = 0;

    if (false ==  m_bJumpState)
    {
        m_JumpDustList[m_iJumpDustIndex]->Active(true);
        m_JumpDustList[m_iJumpDustIndex++]->Set_Pos(vPos);
    }
}

void CAncEPController::Reset_Value()
{
    m_bJumpState = false;
}

_bool CAncEPController::Is_Collision(CCollider* _pCollider)
{
    for (auto& iter : m_CollisionList)
        if (iter->Is_Collision(_pCollider))
            return true;
    return false;
}

void  CAncEPController::Update_HandParticlePositon(const _double& _dDeltaSeconds, CAncHanibalStateMachine* _pMonsterStateMachine, CAncientHannibal* _pMonster)
{
    CModel* pModel = static_cast<CModel*>(_pMonster->Get_Component(COM_ANIMMODEL));
    CTransform* pTransform = static_cast<CTransform*>(_pMonster->Get_Component(COM_TRANSFORM));
    if (nullptr == pModel || nullptr == pTransform)
        return;

    _matrix WorldMatrix = pModel->Get_PivotMatrix() * pTransform->Get_WorldMatrix();
    for(_uint i = 0; i < 2; ++i)
    {
        _matrix MultipleWorld = m_HandHierackyNodeList[i]->Get_CombinedTransformationMatrix() * WorldMatrix;
        CTransform* pParticleTransform = static_cast<CTransform*>(m_HandParticle[i]->Get_Component(COM_TRANSFORM));
        if (nullptr != pParticleTransform)
        {
            pParticleTransform->Set_Pos(MultipleWorld.r[3]);
            m_HandParticle[i]->Set_Active(true);
        }
    }
}

CAncEPController* CAncEPController::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
    CAncEPController* pInstance = new CAncEPController(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct(_pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CAncEPController::Create To Failed");
    }
    return pInstance;
}

void CAncEPController::Free()
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance)
    {
        for (auto& iter : m_JumpDustList)
        {
            iter->Active(false);
            pGameInstance->Add_DeleteObject(g_iCurrentLevel, LAYER_EFFECTPARTICLE, iter);
            Safe_Release(iter);
        }

        for (auto& iter : m_HandParticle)
        {
            iter->Active(false);
            pGameInstance->Add_DeleteObject(g_iCurrentLevel, LAYER_EFFECTPARTICLE, iter);
            Safe_Release(iter);
        }

        for (auto& iter : m_FireWallList)
        {
            iter->Active(false);
            pGameInstance->Add_DeleteObject(g_iCurrentLevel, LAYER_EFFECTPARTICLE, iter);
            Safe_Release(iter);
        }

        for (auto& iter : m_DragonMagic)
        {
            iter->Active(false);
            pGameInstance->Add_DeleteObject(g_iCurrentLevel, LAYER_EFFECTPARTICLE, iter);
            Safe_Release(iter);
        }
    }
    END_SINGLETON(pGameInstance);

    for (auto& iter : m_HandHierackyNodeList)
        Safe_Release(iter);

    for (auto& iter : m_CollisionList)
        Safe_Release(iter);

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
