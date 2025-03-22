#pragma once
#include "CMob.h"
BEGIN(Engine)
class UParticle;
class UParticleSystem;
END
BEGIN(Client)

class CItemChest final : public CMob {
public:
	CItemChest(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	CItemChest(const CItemChest& _rhs);
	DESTRUCTOR(CItemChest)
	enum CHESTTYPE {
		TYPE_MIMIC, TYPE_CHEST, TYPE_END
	};
public:
	// UPlayer을(를) 통해 상속됨
	CLONE_MACRO(CItemChest, "CItemChest::CloneToFailed")
		virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _Datas) override;
	void SetOpeningState(const _bool& _newState) { m_bisOpen = _newState; }

	void SetChestType(CHESTTYPE _type) { m_ChestType = _type; }
	CHESTTYPE GetChestType() { return m_ChestType; }

protected:
	virtual void TickActive(const _double& _dTimeDelta) override;
	virtual void LateTickActive(const _double& _dTimeDelta) override;
	virtual HRESULT RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual void Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta) override;

private:
	CHESTTYPE m_ChestType;
	_bool			m_bisOpen;
	_bool			m_bisOpenSound=false;
	SHPTR<UParticle>										m_spOpenChestParticle;
	_float ParticleActiveTime = 0;
};

END