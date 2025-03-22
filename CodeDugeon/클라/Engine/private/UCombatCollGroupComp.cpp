#include "EngineDefine.h"
#include "UCombatCollGroupComp.h"

UCombatCollGroupComp::UCombatCollGroupComp(CSHPTRREF<UDevice> _spDevice)
	: UComponent(_spDevice)
{
}

UCombatCollGroupComp::UCombatCollGroupComp(const UCombatCollGroupComp& _rhs)
	: UComponent(_rhs)
{
}

void UCombatCollGroupComp::Free()
{
}

SHPTR<UCloneObject> UCombatCollGroupComp::Clone(const VOIDDATAS& _tDatas)
{
	return SHPTR<UCloneObject>();
}

HRESULT UCombatCollGroupComp::NativeConstruct()
{
	return E_NOTIMPL;
}

HRESULT UCombatCollGroupComp::NativeConstructClone(const VOIDDATAS& _tDatas)
{
	return E_NOTIMPL;
}
