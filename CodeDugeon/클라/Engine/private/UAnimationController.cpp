#include "EngineDefine.h"
#include "UAnimationController.h"
#include "UCharacter.h"
#include "UAnimModel.h"
#include "UAnimation.h"
#include "UMethod.h"
#include "UGameInstance.h"

UAnimationController::UAnimationController(CSHPTRREF<UDevice> _spDevice)
	: UController(_spDevice), m_iAnimState{0}
{
}

UAnimationController::UAnimationController(const UAnimationController& _rhs) :
	UController(_rhs), m_iAnimState{0}
{
}

void UAnimationController::Free()
{
}

HRESULT UAnimationController::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT UAnimationController::NativeConstructClone(const VOIDDATAS& _tDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_tDatas), E_FAIL);

	assert(_tDatas.size() > 0);

	ANIMCONTROLLERDESC Desc = UMethod::ConvertTemplate_Index<ANIMCONTROLLERDESC>(_tDatas, 0);
	assert(nullptr != Desc.spCharacter);

	m_wpOwnerCharacter = Desc.spCharacter;
	return S_OK;

	return S_OK;
}

void UAnimationController::Tick(const _double& _dTimeDelta)
{
}

void UAnimationController::ReceiveNetworkProcessData(void* _pData)
{
}

void UAnimationController::UpdateState(CSHPTRREF<UAnimModel> _spAnimModel, const _int _iAnimState, const _wstring& _wstrTrigger)
{
	assert(nullptr != _spAnimModel);
	if (_iAnimState != ANIM_IDLE)
	{
		_spAnimModel->ResetCurAnimEvent();
	}

	m_wstrTrigger = _wstrTrigger;
	m_iAnimState = _iAnimState;
}

