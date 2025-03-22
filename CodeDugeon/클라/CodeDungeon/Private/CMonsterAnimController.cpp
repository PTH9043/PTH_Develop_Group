#include "ClientDefines.h"
#include "CMonsterAnimController.h"
#include "UCharacter.h"
#include "UAnimModel.h"
#include "UAnimation.h"

CMonsterAnimController::CMonsterAnimController(CSHPTRREF<UDevice> _spDevice)
    : UAnimationController(_spDevice)
{
}

CMonsterAnimController::CMonsterAnimController(const CMonsterAnimController& _rhs)
    : UAnimationController(_rhs)
{
}

void CMonsterAnimController::Free()
{
}

HRESULT CMonsterAnimController::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT CMonsterAnimController::NativeConstructClone(const VOIDDATAS& _tDatas)
{
	return __super::NativeConstructClone(_tDatas);
}

void CMonsterAnimController::Tick(const _double& _dTimeDelta)
{
}

void CMonsterAnimController::ReceiveNetworkProcessData(void* _pData)
{
#ifdef _ENABLE_PROTOBUFF
    SHPTR<UCharacter> spMonster = GetOwnerCharacter();
    SHPTR<UAnimModel> spAnimModel = spMonster->GetAnimModel();
    {
        MOBSTATE* pMonsterData = static_cast<MOBSTATE*>(_pData);
        SetAnimState(pMonsterData->state());

        if (pMonsterData->animationindex() != spAnimModel->GetCurrentAnimIndex() || 1 == pMonsterData->triggeron())
            spAnimModel->SetAnimation(pMonsterData->animationindex());

        spAnimModel->GetCurrentAnimation()->SetAnimTimeAcc(pMonsterData->animtime());
    }
#endif
}
