#include "EngineDefine.h"
#include "UInputReceiver.h"

UInputReceiver::UInputReceiver(CSHPTRREF<UDevice> _spDevice, _int _KeyContainerNumber) :
	UComponent(_spDevice)
{
	m_KeyContainer.resize(_KeyContainerNumber);
}

UInputReceiver::UInputReceiver(const UInputReceiver& _rhs) : 
	UComponent(_rhs), m_KeyContainer{_rhs.m_KeyContainer}
{
}

void UInputReceiver::Free()
{
}

HRESULT UInputReceiver::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT UInputReceiver::NativeConstructClone(const VOIDDATAS& _tDatas)
{
	return __super::NativeConstructClone(_tDatas);
}

void UInputReceiver::Tick(const _double& _dTimeDelta, UController* _pController)
{

}

void UInputReceiver::ReceiveNetworkProcessData(void* _pData)
{
}

_bool UInputReceiver::IsCheckEnableKey(_int _iIndex)
{
	RETURN_CHECK(_iIndex >= m_KeyContainer.size(), false);
	return m_KeyContainer[_iIndex];
}

void UInputReceiver::BindKeyValue(const _int _iIndex, const _bool _isValue)
{
	m_KeyContainer[_iIndex] = _isValue;
}
