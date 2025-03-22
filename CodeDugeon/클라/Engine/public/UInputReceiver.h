#pragma once
#include "UComponent.h"

BEGIN(Engine)
using KEYCONTAINER = VECTOR<_bool>;
class UProcessedData;
class UController;

class UInputReceiver abstract : public UComponent{
public:

public:
	UInputReceiver(CSHPTRREF<UDevice> _spDevice, _int _KeyContainerNUmber);
	UInputReceiver(const UInputReceiver& _rhs);
	DESTRUCTOR(UInputReceiver)
public:
	virtual void Free() override PURE;
	virtual SHPTR<UCloneObject> Clone(const VOIDDATAS& _tDatas) PURE;
public:
	// Native Construct 
	virtual HRESULT NativeConstruct() override PURE;
	// Clone 
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _tDatas) override PURE;
	// TickActive
	virtual void Tick(const _double& _dTimeDelta, UController* _pController);
	// 네트워크 정보를 받아오는 함수
	virtual void ReceiveNetworkProcessData(void* _pData);
	// Enable Keys
	_bool IsCheckEnableKey(_int _iIndex);
	void BindKeyValue(const _int _iIndex, const _bool _isValue);
private:
	KEYCONTAINER					m_KeyContainer;
};

END

