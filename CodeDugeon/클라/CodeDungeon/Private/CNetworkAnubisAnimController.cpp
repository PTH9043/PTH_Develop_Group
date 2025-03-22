#include "ClientDefines.h"
#include "CNetworkAnubisAnimController.h"
#include "CAnubis.h"
#include "UGameInstance.h"
#include "UAnimModel.h"
#include "UCharacter.h"
#include "UAnimation.h"
#include "UTransform.h"
#include "UGuard.h"
#include "UPlayer.h"
#include "UMat.h"

CNetworkAnubisAnimController::CNetworkAnubisAnimController(CSHPTRREF<UDevice> _spDevice)
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
    m_dRecvAnimDuration{ 0 },
    m_iRandomValueforAttack{ 0 },
    m_bAttackStart{ false },
    m_dTimerForFireCircle{ 0 },
    m_dShieldCooltime{ 3 }
{
}

CNetworkAnubisAnimController::CNetworkAnubisAnimController(const CNetworkAnubisAnimController& _rhs)
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
    m_dRecvAnimDuration{ 0 },
    m_iRandomValueforAttack{ 0 },
    m_bAttackStart{ false },
    m_dTimerForFireCircle{ 0 },
    m_dShieldCooltime{ 3 }
{
}

void CNetworkAnubisAnimController::Free()
{
}

HRESULT CNetworkAnubisAnimController::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT CNetworkAnubisAnimController::NativeConstructClone(const VOIDDATAS& _tDatas)
{
    RETURN_CHECK_FAILED(__super::NativeConstructClone(_tDatas), E_FAIL);

    m_wpAnubisMob = std::dynamic_pointer_cast<CAnubis>(GetOwnerCharacter());

    return S_OK;
}

void CNetworkAnubisAnimController::Tick(const _double& _dTimeDelta)
{
    SHPTR<CAnubis> spAnubis = m_wpAnubisMob.lock();
    SHPTR<UAnimModel> spAnimModel = spAnubis->GetAnimModel();
    const _wstring& CurAnimName = spAnimModel->GetCurrentAnimation()->GetAnimName();

    if (CurAnimName == L"Attack3")
    {
        if (spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() >= 0.65 && spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() < 0.66)
        {
            m_dShieldCooltime = 0;
            spAnubis->SetShieldState(true);
        }
    }

    if (CurAnimName == L"Cast")
    {
        if (spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() >= 0.65 && spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() < 0.66)
        {
            spAnubis->SetFireAttackState(true);
        }
    }
    if (spAnubis->GetFireAttackState())
    {
        m_dTimerForFireCircle += _dTimeDelta;
        float scale_factor = 1 + 100 * m_dTimerForFireCircle * m_dTimerForFireCircle;
        if (m_dTimerForFireCircle < 1)
        {
            spAnubis->GetFireCircle()->GetTransform()->SetScale(_float3(scale_factor, scale_factor, 2));
            spAnubis->GetFireCircle()->SetActive(true);
            spAnubis->GetFireCircle1()->GetTransform()->SetScale(_float3(scale_factor - 5, scale_factor - 5, 2));
            spAnubis->GetFireCircle1()->SetActive(true);
        }
        else
        {
            m_dTimerForFireCircle = 0;
            spAnubis->GetFireCircle()->GetTransform()->SetScale(_float3(1, 1, 1));
            spAnubis->GetFireCircle()->SetActive(false);
            spAnubis->GetFireCircle1()->GetTransform()->SetScale(_float3(1, 1, 1));
            spAnubis->GetFireCircle1()->SetActive(false);
            spAnubis->SetFireAttackState(false);
        }
    }
}
