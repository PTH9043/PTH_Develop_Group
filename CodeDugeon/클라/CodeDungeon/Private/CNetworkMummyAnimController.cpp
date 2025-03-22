#include "ClientDefines.h"
#include "CNetworkMummyAnimController.h"
#include "CMummy.h"
#include "UGameInstance.h"
#include "UAnimModel.h"
#include "UCharacter.h"
#include "UAnimation.h"
#include "UTransform.h"
#include "UPlayer.h"
#include "USound.h"

CNetworkMummyAnimController::CNetworkMummyAnimController(CSHPTRREF<UDevice> _spDevice) 
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

CNetworkMummyAnimController::CNetworkMummyAnimController(const CNetworkMummyAnimController& _rhs)
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

void CNetworkMummyAnimController::Free()
{
}

HRESULT CNetworkMummyAnimController::NativeConstruct()
{
    return __super::NativeConstruct();
}

HRESULT CNetworkMummyAnimController::NativeConstructClone(const VOIDDATAS& _tDatas)
{
    RETURN_CHECK_FAILED(__super::NativeConstructClone(_tDatas), E_FAIL);

    m_wpMummyMob = std::dynamic_pointer_cast<CMummy>(GetOwnerCharacter());

    return S_OK;
}

void CNetworkMummyAnimController::Tick(const _double& _dTimeDelta)
{
    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
    SHPTR<CMummy> spMummy = m_wpMummyMob.lock();
    SHPTR<UAnimModel> spAnimModel = spMummy->GetAnimModel();
    const _wstring& CurAnimName = spAnimModel->GetCurrentAnimation()->GetAnimName();

    if (CurAnimName == L"attack1")
    {
        USound* AttackSound1 = spGameInstance->BringSound(L"Attack1_VO_1").get();
        USound* SwooshSound1 = spGameInstance->BringSound(L"ClothWhoosh_1").get();
        if (spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() >= 0.8)
        {
            AttackSound1->StopWithManyChannels();
            SwooshSound1->StopWithManyChannels();
        }
        else if (spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() >= 0.1 && spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() < 0.11)
        {
            AttackSound1->PlayWithManyChannels();
            // AttackSound1->UpdateSound3D(spMummy->GetTransform(), _float3(0, 0, 0), spMummy->GetTargetPlayer()->GetTransform());
            SwooshSound1->PlayWithManyChannels();
            // SwooshSound1->UpdateSound3D(spMummy->GetTransform(), _float3(0, 0, 0), spMummy->GetTargetPlayer()->GetTransform());
        }
    }

    if (CurAnimName == L"attack2")
    {
        USound* AttackSound2 = spGameInstance->BringSound(L"Attack1_VO_2").get();
        USound* SwooshSound2 = spGameInstance->BringSound(L"ClothWhoosh_2").get();
        if (spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() >= 0.8)
        {
            AttackSound2->StopWithManyChannels();
            SwooshSound2->StopWithManyChannels();
        }
        else if (spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() >= 0.1 && spAnimModel->GetCurrentAnimation()->GetAnimationProgressRate() < 0.11)
        {
            AttackSound2->PlayWithManyChannels();
            //AttackSound2->UpdateSound3D(spMummy->GetTransform(), _float3(0, 0, 0), spMummy->GetTargetPlayer()->GetTransform());
            SwooshSound2->PlayWithManyChannels();
            //SwooshSound2->UpdateSound3D(spMummy->GetTransform(), _float3(0, 0, 0), spMummy->GetTargetPlayer()->GetTransform());
        }
    }
}
