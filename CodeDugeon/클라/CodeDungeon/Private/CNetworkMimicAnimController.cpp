#include "ClientDefines.h"
#include "CNetworkMimicAnimController.h"
#include "CMimic.h"
#include "UGameInstance.h"
#include "UAnimModel.h"
#include "UCharacter.h"
#include "UAnimation.h"
#include "UPlayer.h"
#include "UTransform.h"


CNetworkMimicAnimController::CNetworkMimicAnimController(CSHPTRREF<UDevice> _spDevice)
    : CMonsterAnimController(_spDevice),
    m_bAttackMode{ false },
    m_bTauntMode{ false },
    m_dlastHitTime{ 0 },
    m_dlastAttackTime{ 0 },
    m_bstartlastHitTime{ false },
    m_blastAttackWasFirst{ false },
    m_dIdleTimer{ 0 },
    m_bFoundPlayerFirsttime{ false },
    m_didleRandomValueChoosingTimer{ 0 },
    m_iRandomValue{ 0 },
    m_dRecvAnimDuration{ 0 }
{
}

CNetworkMimicAnimController::CNetworkMimicAnimController(const CNetworkMimicAnimController& _rhs)
    : CMonsterAnimController(_rhs),
    m_bAttackMode{ false },
    m_bTauntMode{ false },
    m_dlastHitTime{ 0 },
    m_dlastAttackTime{ 0 },
    m_bstartlastHitTime{ false },
    m_blastAttackWasFirst{ false },
    m_dIdleTimer{ 0 },
    m_bFoundPlayerFirsttime{ false },
    m_didleRandomValueChoosingTimer{ 0 },
    m_iRandomValue{ 0 },
    m_dRecvAnimDuration{ 0 }
{
}

void CNetworkMimicAnimController::Free()
{
}

HRESULT CNetworkMimicAnimController::NativeConstruct()
{
    return __super::NativeConstruct();
}

HRESULT CNetworkMimicAnimController::NativeConstructClone(const VOIDDATAS& _tDatas)
{
    RETURN_CHECK_FAILED(__super::NativeConstructClone(_tDatas), E_FAIL);

    m_wpMimicMob = std::dynamic_pointer_cast<CMimic>(GetOwnerCharacter());

    return S_OK;
}

void CNetworkMimicAnimController::Tick(const _double& _dTimeDelta)
{
}
