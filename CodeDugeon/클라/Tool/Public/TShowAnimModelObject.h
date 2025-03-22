#pragma once

#include "UPawn.h"

BEGIN(Engine)
class UAnimModel;
END

BEGIN(Tool)
/*
@ Date: 2024-02-04, Writer: 박태현
@ Explain
- Animation이 들어간 모델을 보여주기 위한 클래스
*/
class TShowAnimModelObject final  : public UPawn {
public:
	TShowAnimModelObject(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	TShowAnimModelObject(const TShowAnimModelObject& _rhs);
	DESTRUCTOR(TShowAnimModelObject)
public:
	CSHPTRREF<UAnimModel> GetAnimModel() const { return m_spModel; }

public:
	// UPawn을(를) 통해 상속됨
	CLONE_MACRO(TShowAnimModelObject, "TShowAnimModelObject::Clone To Failed")
	virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;

	void SetShowModel(CSHPTRREF<UAnimModel> _spModel) { m_spModel = _spModel; }
protected:
	virtual void TickActive(const _double& _dTimeDelta) override;
	virtual void LateTickActive(const _double& _dTimeDelta) override;
	virtual HRESULT RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass = true) override;
	virtual void Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta) override;
private:
	SHPTR<UAnimModel>				m_spModel;

};

END