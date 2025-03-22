#include "ClientDefines.h"
#include "CSarcophagusAnimController.h"
#include "CSarcophagus.h"
#include "UGameInstance.h"
#include "UAnimModel.h"
#include "UCharacter.h"
#include "UPlayer.h"

CSarcophagusAnimController::CSarcophagusAnimController(CSHPTRREF<UDevice> _spDevice)
	: CMonsterAnimController(_spDevice), m_dRecvAnimDuration{0}
{
}

CSarcophagusAnimController::CSarcophagusAnimController(const CSarcophagusAnimController& _rhs)
	: CMonsterAnimController(_rhs), m_dRecvAnimDuration{0}
{
}

void CSarcophagusAnimController::Free()
{
}

HRESULT CSarcophagusAnimController::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT CSarcophagusAnimController::NativeConstructClone(const VOIDDATAS& _tDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_tDatas), E_FAIL);

	m_wpSarcophagusMob = std::dynamic_pointer_cast<CSarcophagus>(GetOwnerCharacter());

	return S_OK;
}

void CSarcophagusAnimController::Tick(const _double& _dTimeDelta)
{
	// Reset Trigger
	ClearTrigger();
	/* Reset */
	SetAnimState(-1);

	SHPTR<CSarcophagus> spSarcophagus = m_wpSarcophagusMob.lock();
	SHPTR<UAnimModel> spAnimModel = spSarcophagus->GetAnimModel();

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);


	//_bool isIdle = false;
	//_bool isMove = false;

	//SetTrigger(L"IDLE");
	//SetAnimState(ANIM_IDLE);
#ifdef _ENABLE_PROTOBUFF
	spAnimModel->TickEventToRatio(spSarcophagus.get(), L"", m_dRecvAnimDuration, _dTimeDelta);
#else
	spAnimModel->TickEvent(spSarcophagus.get(), GetTrigger(), _dTimeDelta);
#endif
}