#pragma once
#include "UWeapon.h"

BEGIN(Engine)
class UGameInstance;
END

BEGIN(Client)
/*
@ Date: 2024-07-13 Writer: 이성현
@ Explain
- 아누비스의 무기
*/
class CAnubisStaff final : public UWeapon {
public:
	CAnubisStaff(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	CAnubisStaff(const CAnubisStaff& _rhs);
	DESTRUCTOR(CAnubisStaff)
public:
	// UPlayer을(를) 통해 상속됨
	CLONE_MACRO(CAnubisStaff, "CAnubisStaff::CloneToFailed")
		virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _Datas) override;
protected:
	virtual void TickActive(const _double& _dTimeDelta) override;
	virtual void LateTickActive(const _double& _dTimeDelta) override;
	virtual HRESULT RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass = true) override;
	virtual void Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta) override;
private:
};

END