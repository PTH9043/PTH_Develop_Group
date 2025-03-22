#pragma once
#include "UUserInterface.h"

BEGIN(Engine)
class UTexGroup;
class UShaderConstantBuffer;
END

BEGIN(Client)

class CHpBarUI final : public UUserInterface {
	struct HPDESC
	{
		_float _MaxHp;
		_float _CurHp;
		_float _durationTime;
		_float _padding;
	};
public:
	CHpBarUI(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	CHpBarUI(const CHpBarUI& _rhs);
	DESTRUCTOR(CHpBarUI)
public:
	CLONE_MACRO(CHpBarUI, "CHpBarUI::Clone To Failed")
		virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;
protected:
	// Tick, LateTick, Render
	virtual void TickActive(const _double& _dTimeDelta) override;
	virtual void LateTickActive(const _double& _dTimeDelta) override;
	virtual HRESULT RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	// Ready 
	virtual HRESULT ReadyRectTransform(const VOIDDATAS& _vecDatas) override;

	// UUserInterface을(를) 통해 상속됨
	HRESULT RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	HRESULT RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass) override;
	void Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta) override;
private:
	virtual void Free() override;

public:
	void SetMaxHp(_float _MaxHp);
	void SetCurHp(_float _CurHp);
	

private:
	// UI Texture Group 
	SHPTR<UTexGroup>			m_spUITextureGroup;
	SHPTR< UShaderConstantBuffer>	m_spHpBarUIBuffer;
	HPDESC _HpDesc;

};


END
