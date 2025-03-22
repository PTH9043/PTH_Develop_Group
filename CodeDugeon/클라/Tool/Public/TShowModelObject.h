#pragma once
#include "UPawn.h"

BEGIN(Engine)
class UModel;
class UShaderConstantBuffer;
END

BEGIN(Tool)
/*
@ Date: 2024-02-04, Writer: 박태현
@ Explain
- Model을 보여주기 위한 클래스
*/
class TShowModelObject final : public UPawn {
public:
	TShowModelObject(CSHPTRREF<UDevice> _spDevice,
		const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	TShowModelObject(const TShowModelObject& _rhs);
	DESTRUCTOR(TShowModelObject)
		// UPawn을(를) 통해 상속됨
		CLONE_MACRO(TShowModelObject, "TShowModelObject::Clone To Failed")
		virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;

	void SetShowModel(CSHPTRREF<UModel> _spModel) { m_spModel = _spModel; }
	CSHPTRREF<UModel> GetShowModel() { return m_spModel; }
	void SetOutline(_bool draw) {
		m_bOutline = draw;}
	_bool GetIfDraw() {
		return m_bOutline;
	}
protected:
	virtual void TickActive(const _double& _dTimeDelta) override;
	virtual void LateTickActive(const _double& _dTimeDelta) override;
	virtual HRESULT RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescripto) override;
	virtual HRESULT RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescripto) override;
	virtual HRESULT RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescripto, _bool _pass = true) override;
	virtual void Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta) override;
private:
	SHPTR<UModel>										m_spModel;
	SHPTR<UShaderConstantBuffer>		m_spShaderTexCheckBuffer;
	int HasTex[4]{};
	_bool m_bOutline{ 0 };
};

END
