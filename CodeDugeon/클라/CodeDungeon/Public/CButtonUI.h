#include "UUserInterface.h"

BEGIN(Engine)
class UTexGroup;
class UShaderConstantBuffer;
END

BEGIN(Client)

class CButtonUI final : public UUserInterface {
	struct BUTTONDESC
	{
		_float ifPressed;
		_float _durationTime;
		_float2 _padding;
	};
public:
	CButtonUI(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	CButtonUI(const CButtonUI& _rhs);
	DESTRUCTOR(CButtonUI)
public:
	CLONE_MACRO(CButtonUI, "CButtonUI::Clone To Failed")
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
	
	void SetIfPicked(_bool _picked) {
		_buttonDesc.ifPressed= _picked;
	}

private:
	// UI Texture Group 
	SHPTR<UTexGroup>			m_spUITextureGroup;
	SHPTR< UShaderConstantBuffer>	m_spButtonUIBuffer;
	BUTTONDESC _buttonDesc;

};


END
