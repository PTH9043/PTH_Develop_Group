#pragma once
#include "CMonsterAnimController.h"

BEGIN(Client)
class CItemChest;

class CItemChestAnimController final : public CMonsterAnimController {
public:
	CItemChestAnimController(CSHPTRREF<UDevice> _spDevice);
	CItemChestAnimController(const CItemChestAnimController& _rhs);
	virtual ~CItemChestAnimController() = default;
public:
	virtual void Free() override;
	CLONE_MACRO(CItemChestAnimController, "CItemChestAnimController::Clone To Failed")
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