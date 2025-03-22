#include "CMonsterState.h"

CMonsterState::CMonsterState(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CStateMachine(_pDevice, _pContext)
{
}

HRESULT CMonsterState::NativeConstruct(CGameObject* _pOwnerObject)
{
    if (FAILED(__super::NativeConstruct(_pOwnerObject)))
        return E_FAIL;

    return S_OK;
}

void CMonsterState::Tick(const _double& _dTimeDelta, CGameObject* _pOwner)
{
}

void CMonsterState::Late_Tick(const _double& _dTimeDelta, CGameObject* _pOwner)
{
}

void CMonsterState::Reset_FindAttackState(const _double& _dDeltaTime)
{
    if (true == m_bFindAttackState)
    {
        if (m_fFindAttackStateResetTime >= 3.f)
        {
            m_bFindAttackState = false;
        }
        m_fFindAttackStateResetTime += (_float)_dDeltaTime;
    }
}

void CMonsterState::Free()
{
    __super::Free();
}
