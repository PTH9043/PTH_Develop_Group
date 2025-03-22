#pragma once
#include "CMonsterAnimController.h"

BEGIN(Client)
class CItemChest;

class CNetworkItemChestAnimController final : public CMonsterAnimController {
public:
	CNetworkItemChestAnimController(CSHPTRREF<UDevice> _spDevice);
	CNetworkItemChestAnimController(const CNetworkItemChestAnimController& _rhs);
	virtual ~CNetworkItemChestAnimController() = default;
public:
	virtual void Free() override;
	CLONE_MACRO(CNetworkItemChestAnimController, "CNetworkChestAnimController::Clone To Failed")
public:
	// Native Construct 
	virtual HRESULT NativeConstruct() override;
	// Clone 
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _tDatas) override;
	// Tick
	virtual void Tick(const _double& _dTimeDelta) override;
private:
	WKPTR< CItemChest>			m_wpChestMob;
};

END