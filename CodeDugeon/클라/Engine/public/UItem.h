#pragma once
#include "UPawn.h"

BEGIN(Engine)
class UPawn;

class UItem abstract : public UPawn {
public: 
	UItem(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, 
		const CLONETYPE& _eCloneType, const BACKINGTYPE _eBackingType = BACKINGTYPE::STATIC, 
		const ITEMTYPE _eItemType = ITEMTYPE::ITEM_END);
	UItem(const UItem& _rhs);
	DESTRUCTOR(UItem)
public:
	virtual SHPTR<UCloneObject> Clone(const VOIDDATAS& _vecDatas) override PURE;
	virtual void Free() override PURE;
	virtual HRESULT NativeConstruct() override PURE;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override PURE;
public: /* get set */
	const ITEMTYPE GetItemType() const { return m_eItemType; }
protected:
	// Tick, LateTick, Render
	virtual void TickActive(const _double& _dTimeDelta) override PURE;
	virtual void LateTickActive(const _double& _dTimeDelta) override PURE;
	virtual HRESULT RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override PURE;
	virtual HRESULT RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override PURE;
	virtual HRESULT RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass = true) override;
	// Damaged
	virtual void Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta) PURE;
private:
	ITEMTYPE						m_eItemType;
};

END

