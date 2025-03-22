#pragma once
#include "UAnimationController.h"

BEGIN(Client)

class CMonsterAnimController abstract : public UAnimationController {
public:
	CMonsterAnimController(CSHPTRREF<UDevice> _spDevice);
	CMonsterAnimController(const CMonsterAnimController& _rhs);
	virtual ~CMonsterAnimController() = default;
public:
	virtual void Free() override;
public:
	// Native Construct 
	virtual HRESULT NativeConstruct() override;
	// Clone 
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _tDatas) override;
	// Tick
	virtual void Tick(const _double& _dTimeDelta) override;
	// 네트워크 정보를 받아오는 함수
	virtual void ReceiveNetworkProcessData(void* _pData) override;
private:

};

END