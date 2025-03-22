#include "EngineDefine.h"
#include "UCloneObject.h"
//#include "UDevice.h"
//#include "UGpuCommand.h"
//#include "UMethod.h"

UCloneObject::UCloneObject(CSHPTRREF<UDevice> _spDevice)
	: UObject(_spDevice), m_isClone{ false }
{
}

UCloneObject::UCloneObject(const UCloneObject& _rhs)
	: UObject(_rhs)
{
	m_isClone = true;
}


HRESULT UCloneObject::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT UCloneObject::NativeConstructClone(const VOIDDATAS& _stDatas)
{
	return S_OK;
}