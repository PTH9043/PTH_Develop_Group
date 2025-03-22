#include "UUserInterface.h"

BEGIN(Engine)
class UTexGroup;
class UShaderConstantBuffer;
END

BEGIN(Client)

class CLoadingUI final : public UUserInterface {
	struct LOADINGDESC
	{
		_float ifPressed;
		_float _durationTime;
		_float _DefaultcoolTime=1.f;
		_float _LeftcoolTime=1.f;
	};
public:
	CLoadingUI(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	CLoadingUI(const CLoadingUI& _rhs);
	DESTRUCTOR(CLoadingUI)
public:
	CLONE_MACRO(CLoadingUI, "CLoadingUI::Clone To Failed")
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

	_bool GetIfPicked() { return _LoadingDesc.ifPressed; }

	void SetDurationTimePlus(_float _coolTime) {
		_LoadingDesc._durationTime += _coolTime;
	}
	void SetDurationTime(_float _coolTime) {
		_LoadingDesc._durationTime = _coolTime;
	}

	void SetIfPicked(_bool _picked) {
		_LoadingDesc.ifPressed = _picked;
	}
	void SetDefaultCoolTime(_float _coolTime) {
		_LoadingDesc._DefaultcoolTime = _coolTime;
	}
	void SetLeftCoolTime(_float _leftTime) {
		_LoadingDesc._LeftcoolTime = _leftTime;
	}

private:
	// UI Texture Group 
	SHPTR<UTexGroup>			m_spUITextureGroup;
	SHPTR< UShaderConstantBuffer>	m_spLoadingUIBuffer;
	LOADINGDESC _LoadingDesc;

};


END
