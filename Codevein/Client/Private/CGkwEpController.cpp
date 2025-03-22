#include "CGkwEpController.h"
#include "CEffectParticleGroup.h"
#include "CAncientHannibal.h"
#include "CGameInstance.h"
#include "CParticle.h"
#include "C2DEffectParticle.h"
#include "CEffectGroup.h"

CGkwEpController::CGkwEpController(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice), m_pContext(_pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CGkwEpController::NativeConstruct(void* _pArg)
{
    if (nullptr == _pArg)
        return E_FAIL;

    GKWEPDESC tDesc = *static_cast<GKWEPDESC*>(_pArg);
    BEGIN_SINGLETON(CGameInstance, pGameInstance)
    {
        for (_uint i = 0; i < 500; ++i)
        {
            {
                CEffectGroup* pInstance = static_cast<CEffectGroup*>(pGameInstance->Clone_GameObject_Add(g_iCurrentLevel,
                    LAYER_EFFECTPARTICLE, PROTO_GOB_EFFPARTICLE_BOSSROAD));
                Safe_AddRef(pInstance);
                pInstance->Active(false);
                pInstance->Set_LifeTime(4.f);
                m_BossRoadEffectList.push_back(pInstance);
            }
        }

        for (_uint i = 0; i < 30; ++i)
        {
            {
                CEffectGroup* pInstance = static_cast<CEffectGroup*>(pGameInstance->Clone_GameObject_Add(g_iCurrentLevel,
                    LAYER_EFFECTPARTICLE, PROTO_GOB_EFFPARTICLE_ICEBOSSATTACK));
                Safe_AddRef(pInstance);
                pInstance->Active(false);
                m_BossAttackEffectList.push_back(pInstance);
            }

        }

        for (_uint i = 0; i < 100; ++i)
        {
            {
                CEffectParticleGroup* pInstance = static_cast<CEffectParticleGroup*>(pGameInstance->Clone_GameObject_Add(LEVEL_STATIC,
                    LAYER_EFFECTPARTICLE, PROTO_GOB_EFFPARTICLE_GTKWOOM_MAGIC));
                Safe_AddRef(pInstance);
                pInstance->Active(false);
                m_GateKeeprWomanMagic.push_back(pInstance);
            }
            {
                CEffectGroup* pInstance = static_cast<CEffectGroup*>(pGameInstance->Clone_GameObject_Add(g_iCurrentLevel,
                    LAYER_EFFECTPARTICLE, PROTO_GOB_EFFPARTICLE_BOSSUP));
                Safe_AddRef(pInstance);
                pInstance->Active(false);
                m_BossUpEffectList.push_back(pInstance);
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

        _float3 v3InitPos = _float3(-10.f, 0.f, -10.f);
        for (_uint i = 0; i < 20; ++i)
        {
            for (_uint j = 0; j < 20; ++j)
            {
                if (!(v3InitPos.x == 0 && v3InitPos.z == 0))
                {
                    m_vAttackPositionList.push_back(v3InitPos);
                }
                v3InitPos.x += 1.f;
            }
            v3InitPos.x = -10.f;
            v3InitPos.z += 1.f;
        }
    }
    END_SINGLETON(pGameInstance);
	return S_OK;
}

void CGkwEpController::Update(const _double& _dDeltaSeconds, CGKWStateMachine* _pMonsterStateMachine, CGateKeeperWoman* _pMonster)
{
 
}

void CGkwEpController::Late_Update(const _double& _dDeltaSeconds, CGKWStateMachine* _pMonsterStateMachine, CGateKeeperWoman* _pMonster)
{
}

void CGkwEpController::Boss_RoadEffect(_fvector _vPos) 
{
    if ((_uint)m_BossRoadEffectList.size() <= m_iBossRoadEffect)
        m_iBossRoadEffect = 0;

    m_BossRoadEffectList[m_iBossRoadEffect]->Update_Pos(_vPos);
    m_BossRoadEffectList[m_iBossRoadEffect++]->Active(true);
}

void CGkwEpController::Boss_RoadEnd()
{
}

void CGkwEpController::Boss_UpEffect(_fvector _vPos)
{
    if (false == m_bActive_UpEffect)
    {
        if (false == m_bTurnOnLifeLoop_UpEffect)
        {
            for (auto& iter : m_BossUpEffectList)
            {
                iter->Set_Active(false);
            }

            for (_uint i = 0; i < 10000; ++i)
            {
                _uint iRan1 = rand() % 399, iRan2 = rand() % 399;
                _float3 v3Pos1 = m_vAttackPositionList[iRan1];
                m_vAttackPositionList[iRan1] = m_vAttackPositionList[iRan2];
                m_vAttackPositionList[iRan2] = v3Pos1;
            }
            for (_uint i = 0; i < 100; ++i)
            {
                m_GateKeeprWomanMagic[i]->Active(true);
                m_GateKeeprWomanMagic[i]->Set_Pos(_vPos + XMLoadFloat3(&m_vAttackPositionList[i]));
            }
            m_bTurnOnLifeLoop_UpEffect = true;
        }
        _bool bIsFalse = false;
        for (auto& iter : m_GateKeeprWomanMagic)
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
            for (_uint i = 0; i < 100; ++i)
            {
                m_BossUpEffectList[i]->Update_Pos(XMLoadFloat3(&m_vAttackPositionList[i]) + _vPos);
                m_BossUpEffectList[i]->Active(true);

                CTransform* pTransform = static_cast<CTransform*>(m_BossUpEffectList[i]->Get_Component(COM_TRANSFORM));
                if (nullptr != pTransform)
                {
                    m_CollisionList[i]->Tick(pTransform->Get_WorldMatrix());
                }
            }
            m_bActive_UpEffect = true;
        }
    }
}

void CGkwEpController::Boss_UpEffect_End()
{
    for (_uint i = 0; i < 100; ++i)
    {
        m_GateKeeprWomanMagic[i]->Reset();
        m_GateKeeprWomanMagic[i]->Active(false);
    }

    m_bTurnOnLifeLoop_UpEffect = false;
    m_bActive_UpEffect = false;

    for (auto& iter : m_CollisionList)
    {
        iter->Tick(XMMatrixTranslation(-10000.f, -10000.f, -10000.f));
    }
}

void CGkwEpController::Boss_Shoot(_fvector _vPos, _fvector _vDir)
{
    if (false == m_bOneShoot)
    {
        for (auto& iter : m_BossAttackEffectList)
            iter->Update_Look(_vDir);

        m_BossAttackEffectList[0]->Update_Pos(_vPos);
        m_BossAttackEffectList[0]->Active(true);

        _float fRange = -5.f;
        for (_uint i = 1; i <= 15; ++i)
        {
            m_BossAttackEffectList[i]->Update_Rot(_float3(0.f, fRange, 0.f));
            m_BossAttackEffectList[i]->Update_Pos(_vPos);
            m_BossAttackEffectList[i]->Active(true);
            fRange -= 2.5f;
        }
        fRange = 5.f;
        for (_uint i = 8; i <= 29; ++i)
        {
            m_BossAttackEffectList[i]->Update_Rot(_float3(0.f, fRange, 0.f));
            m_BossAttackEffectList[i]->Update_Pos(_vPos);
            m_BossAttackEffectList[i]->Active(true);
            fRange += 2.5f;
        }
        m_bOneShoot = true;
    }
}

void CGkwEpController::Boss_Shoot_End()
{
    m_bOneShoot = false;
}

void CGkwEpController::Boss_RangeShoot(_fvector _vPos, _fvector _vRight)
{
    if (false == m_bRangeShoot)
    {
        m_BossAttackEffectList[0]->Update_Pos(_vPos + XMVectorSet(0.f, 3.f, 0.f, 0.f));
        m_BossAttackEffectList[0]->Update_Rot(_float3(-30.f, 0.f, 0.f));
        m_BossAttackEffectList[0]->Active(true);

        _float fRange = -5.f;
        _vector vPos = XMVectorSet(0.f, 3.f, 0.f, 0.f);
        for (_uint i = 1; i <= 15; ++i)
        {
            m_BossAttackEffectList[i]->Update_Rot(_float3(-30.f, fRange, 0.f));
            m_BossAttackEffectList[i]->Update_Pos(_vPos + vPos);
            m_BossAttackEffectList[i]->Active(true);
            fRange -= 12.f;
        }
        fRange = 5.f;
        vPos = XMVectorSet(0.f, 3.f, 0.f, 0.f);
        for (_uint i = 8; i <= 29; ++i)
        {
            m_BossAttackEffectList[i]->Update_Rot(_float3(-30.f, fRange, 0.f));
            m_BossAttackEffectList[i]->Update_Pos(_vPos + vPos);
            m_BossAttackEffectList[i]->Active(true);
            fRange += 12.f;
        }

        for (auto& iter : m_BossAttackEffectList)
            iter->Update_Scale(_float3(12.f, 12.f, 12.f));

        m_bRangeShoot = true;
    }
}

void CGkwEpController::Boss_RangeShoot_End()
{
    m_bRangeShoot = false;
}

void CGkwEpController::Reset_State()
{
}

_bool CGkwEpController::Is_Collision_UpEffect(CCollider* _pCollider)
{
    if (nullptr == _pCollider)
        return false;

    for (auto& iter : m_CollisionList)
        if (true == iter->Is_Collision(_pCollider))
            return true;

    return false;
}

_bool CGkwEpController::Is_Collision_Shoot(CCollider* _pCollider)
{
    if (nullptr == _pCollider)
        return false;

    m_GetCollisionList.clear();

    for (auto& iter : m_BossAttackEffectList)
        iter->Get_Collider(&m_GetCollisionList);

    for (auto& iter : m_GetCollisionList)
        if (iter->Is_Collision(_pCollider))
            return true;

    return false;
}

CGkwEpController* CGkwEpController::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
    CGkwEpController* pInstance = new CGkwEpController(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct(_pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CGkwEpController::Create To Failed");
    }
	return pInstance;
}

void CGkwEpController::Free()
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance)
    {
        for (auto& iter : m_BossAttackEffectList)
        {
            iter->Active(false);
            pGameInstance->Add_DeleteObject(g_iCurrentLevel, LAYER_EFFECTPARTICLE, iter);
            Safe_Release(iter);
        }

        for (auto& iter : m_BossRoadEffectList)
        {
            iter->Active(false);
            pGameInstance->Add_DeleteObject(g_iCurrentLevel, LAYER_EFFECTPARTICLE, iter);
            Safe_Release(iter);
        }

        for (auto& iter : m_BossUpEffectList)
        {
            iter->Active(false);
            pGameInstance->Add_DeleteObject(g_iCurrentLevel, LAYER_EFFECTPARTICLE, iter);
            Safe_Release(iter);
        }
    }
    END_SINGLETON(pGameInstance);

    for (auto& iter : m_CollisionList)
        Safe_Release(iter);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
