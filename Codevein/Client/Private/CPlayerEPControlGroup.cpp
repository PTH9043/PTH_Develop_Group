#include "CPlayerEPControlGroup.h"
#include "CGameInstance.h"
#include "CPlayerStateMachine.h"
#include "CPlayer.h"
#include "CWeapon.h"

CPlayerEPControlGroup::CPlayerEPControlGroup(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
}

HRESULT CPlayerEPControlGroup::NativeConstruct(CPlayer* _pPlayer)
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance)
    {
        CTransform* pTransform = static_cast<CTransform*>(_pPlayer->Get_Component(COM_TRANSFORM));
        CEffectParticleGroup::EFFECTPARTICLEGROUPDESC tDesc;
        tDesc.pTransform = pTransform;
        tDesc.v3SupplementPos = _float3(0.f, 0.1f, 0.f);
        for (_uint i = 0; i < 20; ++i)
        {
            CEffectParticleGroup* pInstance = static_cast<CEffectParticleGroup*>(pGameInstance->Clone_GameObject_Add(LEVEL_STATIC, LAYER_EFFECTPARTICLE,
                PROTO_GOB_EFFPARTICLE_PLAYER_MOVEEFFECT, &tDesc));
            
            pInstance->Active(false);
            Safe_AddRef(pInstance);
            m_PlayerWalkSmokeList.push_back(pInstance);
        }
    }
    {
        CEffectParticleGroup::EFFECTPARTICLEGROUPDESC tDesc;
        m_pPlayerSworldParticle = static_cast<CEffectParticleGroup*>(pGameInstance->Clone_GameObject_Add(LEVEL_STATIC, LAYER_EFFECTPARTICLE,
            PROTO_GOB_EFFPARTICLE_PLAYER_SWORLDPARTICLE, &tDesc));
        Safe_AddRef(m_pPlayerSworldParticle);
    }
    END_SINGLETON(pGameInstance);

    return S_OK;
}

void CPlayerEPControlGroup::Update(CPlayer* _pPlayer, CPlayerStateMachine* _pPlayerStateMachine, const _double& _dTimeDelta)
{
    if (nullptr == _pPlayer)
        return;

    if (CPlayerStateMachine::STATE_WALK == _pPlayerStateMachine->Get_State() ||
        CPlayerStateMachine::STATE_RUN == _pPlayerStateMachine->Get_State())
    {
        if (m_tWalkTime.Is_Over(_dTimeDelta))
        {
            m_tWalkTime.Reset_Timer();
            Active_PlayerWalSmokeList();
        }
    }
    else
    {
        m_tWalkTime.Reset_Timer();
    }
}

void CPlayerEPControlGroup::Late_Update(CPlayer* _pPlayer, CPlayerStateMachine* _pPlayerStateMachine, const _double& _dTimeDelta)
{
    Weapon_Pos_Particle(_pPlayer->Get_Equip(CEquipment::EQUIP_WEAPON));
}

void CPlayerEPControlGroup::Active_PlayerWalSmokeList()
{
    if (m_PlayerWalkSmokeList.size() <= m_iPlayerWalkSmokeIndex)
        m_iPlayerWalkSmokeIndex = 0;

    m_PlayerWalkSmokeList[m_iPlayerWalkSmokeIndex++]->Active(true);
}

void CPlayerEPControlGroup::Weapon_Pos_Particle(CEquipment* _pWeapon)
{
    CWeapon* pWeapon = static_cast<CWeapon*>(_pWeapon);
    if (nullptr == pWeapon)
        return;

    CTransform* pTrasform = static_cast<CTransform*>(_pWeapon->Get_Component(COM_TRANSFORM));
    if (nullptr == pTrasform)
        return;

    _vector vPos = XMLoadFloat3((_float3*)&pWeapon->Get_SockMatrix4x4().m[3][0]);

    m_pPlayerSworldParticle->Set_Pos(vPos);
    m_pPlayerSworldParticle->Active(true);
}

CPlayerEPControlGroup* CPlayerEPControlGroup::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CPlayer* _pPlayer)
{
    CPlayerEPControlGroup* pInstance = new CPlayerEPControlGroup(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct(_pPlayer)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CPlayerEPControlGroup::Create To Failed");
    }
    return pInstance;
}

void CPlayerEPControlGroup::Free()
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance)
    {
        for (auto& iter : m_PlayerWalkSmokeList)
        {
            pGameInstance->Add_DeleteObject(g_iCurrentLevel, LAYER_EFFECTPARTICLE, iter);
            Safe_Release(iter);
        }
        pGameInstance->Add_DeleteObject(g_iCurrentLevel, LAYER_EFFECTPARTICLE, m_pPlayerSworldParticle);
        Safe_Release(m_pPlayerSworldParticle);
    }
    END_SINGLETON(pGameInstance);

    __super::Free();
}
