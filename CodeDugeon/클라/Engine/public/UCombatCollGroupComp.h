#pragma once

#include "UComponent.h"

BEGIN(Engine)
class UCollider;
/*
@ Date: 2024-04-27, Writer: 박태현
@ Explain
-  배틀을 위한 콜라이더들에 대한 정보를 모아놓은 클래스이다. 
*/
class UCombatCollGroupComp final : public UComponent{
public:
	UCombatCollGroupComp(CSHPTRREF<UDevice> _spDevice);
	UCombatCollGroupComp(const UCombatCollGroupComp& _rhs);
	virtual ~UCombatCollGroupComp() = default;
public:
	virtual void Free() override;
	virtual SHPTR<UCloneObject> Clone(const VOIDDATAS& _tDatas) override;
public:
	// Native Construct 
	virtual HRESULT NativeConstruct() override;
	// Clone 
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _tDatas) override;
private:

};

END