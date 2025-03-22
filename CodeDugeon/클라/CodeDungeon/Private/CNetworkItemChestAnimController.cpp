#include "ClientDefines.h"
#include "CNetworkItemChestAnimController.h"
#include "CItemChest.h"
#include "UGameInstance.h"
#include "UAnimModel.h"
#include "UCharacter.h"

CNetworkItemChestAnimController::CNetworkItemChestAnimController(CSHPTRREF<UDevice> _spDevice)
	: CMonsterAnimController(_spDevice)
{
}


CNetworkItemChestAnimController::CNetworkItemChestAnimController(const CNetworkItemChestAnimController& _rhs)
	: CMonsterAnimController(_rhs)
{
}


void CNetworkItemChestAnimController::Free()
{
}

HRESULT CNetworkItemChestAnimController::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT CNetworkItemChestAnimController::NativeConstructClone(const VOIDDATAS& _tDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_tDatas), E_FAIL);

	m_wpChestMob = std::dynamic_pointer_cast<CItemChest>(GetOwnerCharacter());

	return S_OK;
}

void CNetworkItemChestAnimController::Tick(const _double& _dTimeDelta)
{
}
