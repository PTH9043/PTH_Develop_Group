
#pragma once
#include "CMonsterAnimController.h"

BEGIN(Client)
class CSarcophagus;

class CSarcophagusAnimController final : public CMonsterAnimController {
public:
	CSarcophagusAnimController(CSHPTRREF<UDevice> _spDevice);
	CSarcophagusAnimController(const CSarcophagusAnimController& _rhs);
	virtual ~CSarcophagusAnimController() = default;
public:
	virtual void Free() override;
	CLONE_MACRO(CSarcophagusAnimController, "CSarcophagusAnimController::Clone To Failed")
public:
	// Native Construct 
	virtual HRESULT NativeConstruct() override;
	// Clone 
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _tDatas) override;
	// Tick
	virtual void Tick(const _double& _dTimeDelta) override;
private:
	WKPTR< CSarcophagus>			m_wpSarcophagusMob;
	_double											m_dRecvAnimDuration;
};

END
