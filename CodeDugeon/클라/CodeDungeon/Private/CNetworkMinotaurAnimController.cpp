#include "ClientDefines.h"
#include "CNetworkMinotaurAnimController.h"
#include "CMinotaur.h"
#include "UGameInstance.h"
#include "UAnimModel.h"
#include "UCharacter.h"
#include "UAnimation.h"
#include "UTransform.h"
#include "UPlayer.h"
#include "CWarriorPlayer.h"

CNetworkMinotaurAnimController::CNetworkMinotaurAnimController(CSHPTRREF<UDevice> _spDevice)
    : CMonsterAnimController(_spDevice),
    m_bAttackMode{ false },
    m_dlastHitTime{ 0 },
    m_dlastAttackTime{ 0 },
    m_bstartlastHitTime{ false },
    m_blastAttackWasFirst{ false },
    m_dIdleTimer{ 0 },
    m_bRushMode{ false },
    m_dRushModeTimer{ 0 },
    m_dRushAttackTimer{ 0 },
    m_didleRandomValueChoosingTimer{ 0 },
    m_iRandomValue{ 0 }
{
}

CNetworkMinotaurAnimController::CNetworkMinotaurAnimController(const CNetworkMinotaurAnimController& _rhs)
    : CMonsterAnimController(_rhs),
    m_bAttackMode{ false },
    m_dlastHitTime{ 0 },
    m_dlastAttackTime{ 0 },
    m_bstartlastHitTime{ false },
    m_blastAttackWasFirst{ false },
    m_dIdleTimer{ 0 },
    m_bRushMode{ false },
    m_dRushModeTimer{ 0 },
    m_dRushAttackTimer{ 0 },
    m_didleRandomValueChoosingTimer{ 0 },
    m_iRandomValue{ 0 }
{
}

void CNetworkMinotaurAnimController::Free()
{
}

HRESULT CNetworkMinotaurAnimController::NativeConstruct()
{
    return __super::NativeConstruct();
}

HRESULT CNetworkMinotaurAnimController::NativeConstructClone(const VOIDDATAS& _tDatas)
{
    RETURN_CHECK_FAILED(__super::NativeConstructClone(_tDatas), E_FAIL);

    m_wpMinotaurMob = std::dynamic_pointer_cast<CMinotaur>(GetOwnerCharacter());

    return S_OK;
}

void CNetworkMinotaurAnimController::Tick(const _double& _dTimeDelta)
{
}

