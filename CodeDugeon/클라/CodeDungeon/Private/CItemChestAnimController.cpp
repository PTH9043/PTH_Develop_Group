#include "ClientDefines.h"
#include "CItemChestAnimController.h"
#include "CItemChest.h"
#include "UGameInstance.h"
#include "UAnimModel.h"
#include "UCharacter.h"

CItemChestAnimController::CItemChestAnimController(CSHPTRREF<UDevice> _spDevice)
	: CMonsterAnimController(_spDevice)
{
}

CItemChestAnimController::CItemChestAnimController(const CItemChestAnimController& _rhs)
	: CMonsterAnimController(_rhs)
{
}

void CItemChestAnimController::Free()
{
}

HRESULT CItemChestAnimController::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT CItemChestAnimController::NativeConstructClone(const VOIDDATAS& _tDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_tDatas), E_FAIL);

	m_wpChestMob = std::dynamic_pointer_cast<CItemChest>(GetOwnerCharacter());

	return S_OK;
}

void CItemChestAnimController::Tick(const _double& _dTimeDelta)
{
	// Reset Trigger
	ClearTrigger();
	/* Reset */
	SetAnimState(-1);

	SHPTR<CItemChest> spChest = m_wpChestMob.lock();
	SHPTR<UAnimModel> spAnimModel = spChest->GetAnimModel();

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

	//_bool isIdle = false;
	//_bool isMove = false;

	//SetTrigger(L"IDLE");
	//SetAnimState(ANIM_IDLE);

	spAnimModel->TickEvent(spChest.get(), GetTrigger(), _dTimeDelta);
}
